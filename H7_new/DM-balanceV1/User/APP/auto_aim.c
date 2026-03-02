#include "main.h"
#include "usart.h"
#include "bsp_usart.h"
#include "usbd_cdc_if.h"
#include "auto_aim.h"
#include "cmsis_os.h"
#include "stm32h7xx_hal.h"

#include "usb_common.h"
#include "uproto.h"

#include "arm_math.h"

#include "gimbal_task.h"
#include "gimbal_behaviour.h"
#include "chassis_power_control.h"
#include "chassis_task.h"
#include "chassis_behaviour.h"
#include "referee.h"
#include "INS_task.h"
#include "shoot.h"
#include "struct_typedef.h"
#include "bsp_vofa+.h"
//#include "voltage_task.h"
#include "CRC8_CRC16.h"
#include "GRAVITY.h"
#include "tim.h"

extern uproto_context_t proto_ctx;
// [SYNC_FROM_H] Synced auto-aim control logic from H:\DM-balanceV1\User\APP

// global auto-aim state
auto_aim_t aim;

fp32 yaw = 0.0f;
fp32 CompensationAngle = 0.0f;

// ---------------- Auto-aim internal control state (MCU-side trajectory shaping) ----------------
// [SYNC_FROM_H] New trajectory shaping state replaces direct motor writes
typedef struct
{
    float err_rad;        // latest vision error (rad)
    float omega_cmd_rad;  // commanded angular velocity (rad/s)
} auto_aim_axis_ctrl_t;

typedef struct
{
    auto_aim_axis_ctrl_t yaw_axis;
    auto_aim_axis_ctrl_t pitch_axis;
    uint32_t             last_tick_ms;
} auto_aim_ctrl_t;

typedef struct {
    fp32 buf[5];     /* 环形缓冲区 */
    uint8_t idx;        /* 写指针 0~4 */
    fp32 last_out;   /* 上次有效输出 */
} MedianFilter5;
		MedianFilter5 yawfilter = {0}; 
		MedianFilter5 pitchfilter = {0};

#define SWAP_F(a, b) do{ float _t=(a); (a)=(b); (b)=_t; }while(0)

float mf5_update(MedianFilter5 *f, float new_val)
{
    /* 1. 若检测到 0.0f，视为异常/丢帧，直接返回上次有效值 */
    if (new_val == 0.0f)
        return f->last_out;

    /* 2. 写环形缓冲区 */
    f->buf[f->idx] = new_val;
    f->idx = (f->idx + 1) % 5;

    /* 3. 复制到局部变量，排序网络（5 元素 9 次比较） */
    float a = f->buf[0];
    float b = f->buf[1];
    float c = f->buf[2];
    float d = f->buf[3];
    float e = f->buf[4];

    if (a > b) SWAP_F(a, b);
    if (d > e) SWAP_F(d, e);
    if (a > c) SWAP_F(a, c);
    if (b > c) SWAP_F(b, c);
    if (a > d) SWAP_F(a, d);
    if (c > d) SWAP_F(c, d);
    if (b > e) SWAP_F(b, e);
    if (b > c) SWAP_F(b, c);
    if (d > e) SWAP_F(d, e);

    /* 此时 c 为中值 */
    f->last_out = c;
    return c;
}

static auto_aim_ctrl_t s_auto_aim_ctrl = {0};

//自瞄增益
// basic tuning (can be adjusted during testing)
// Kv: position error -> target angular velocity (rad/s per rad)
#define AA_KV_YAW          (1.0f)
#define AA_KV_PITCH        (1.5f)
// Kd: derivative gain on error，用于在高 Kv 下增加阻尼
#define AA_KD_YAW          (0.4f)
#define AA_KD_PITCH        (0.2f)
// 非线性区间：误差小于该角度时自动降低 Kv、加大阻尼
#define AA_ERR_SOFT_ZONE_RAD   (5.0f * PI / 180.0f)       // ~5 deg
// D 项一阶低通滤波系数（0~1，越小越平滑）
#define AA_D_LPF_ALPHA     (0.5f)

//最大转动速度
#define AA_OMEGA_MAX_YAW   (9.0f * PI / 180.0f)         // 18 deg/s
#define AA_OMEGA_MAX_PITCH (9.0f * PI / 180.0f)

#define AA_A_MAX_YAW       (720.0f * PI / 180.0f)         // 720 deg/s^2
#define AA_A_MAX_PITCH     (720.0f * PI / 180.0f)

#define AA_EPS_TH_RAD      (0.5f * PI / 180.0f)           // ~0.5 deg
#define AA_EPS_OMEGA_RAD   (3.0f * PI / 180.0f)           // ~3 deg/s

// internal derivative state for PD-like shaping
static float s_last_err_yaw = 0.0f;
static float s_last_err_pitch = 0.0f;
static float s_err_d_yaw = 0.0f;
static float s_err_d_pitch = 0.0f;

static inline float aa_clamp(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}


static void auto_aim_control_tick_internal(auto_aim_t *aim_loop);


// Legacy example function (kept for compatibility, currently unused in main loop)
void auto_aim_loop(auto_aim_t* aim_loop)
{
    (void)aim_loop;
}

void auto_aim_init(auto_aim_t *aim_init)
{
    if (aim_init == NULL)
    {
        return;
    }

    /* 初始化自瞄目标有效标志 */
    s_target_valid = 0; 
    /* 初始化射击脉冲待处理标志 */
    s_fire_pulse_pending = 0;
    /* 初始化锁定进入时间戳 */
    s_lock_enter_ms = 0;
    /* 初始化上次射击时间戳 */
    s_last_fire_ms = 0;

    /* Auto-aim disabled by default (RC has priority). */
    /* 默认关闭自瞄，优先听遥控器。 */
    aim_init->auto_aim_flag = AIM_ON;
    aim_init->last_fdb = 0;
    aim_init->online = 1;

    aim_init->shoot_delay = 0;
    aim_init->yaw_delay = 0;
    aim_init->pitch_delay = 0;

    aim_init->receive.distance = 0.0f;
    aim_init->receive.pitch = 0.0f;
    aim_init->receive.shoot_delay = 0.0f;
    aim_init->receive.yaw = 0.0f;

    aim_init->aim_rc = get_remote_control_point();
		
}

void auto_aim_set(auto_aim_t *aim_set)
{
    if (aim_set == NULL)
    {
        return;
    }

    static uint32_t press_r_time = 0;

#if ROBOT_MODE == release
    // timeout check: if host not updated for a while, mark offline and disable auto-aim
    if (HAL_GetTick() - aim_set->last_fdb > AUTO_AIM_TIMEOUT)
    {
        aim_set->online = 0;
        aim_set->auto_aim_flag = AIM_OFF;
        aim_set->receive.yaw = 0.0f;
        aim_set->receive.pitch = 0.0f;
        s_target_valid = 0;              // 清除目标有效标志
        s_fire_pulse_pending = 0;        // 清除待发射脉冲标志
        s_lock_enter_ms = 0;             // 清除锁定进入时间戳
        return;
    }
#endif

    // right mouse press toggles auto-aim
    if (aim_set->aim_rc && aim_set->aim_rc->mouse.press_r)
    {
        press_r_time += AUTO_AIM_TIME;
    }
    else
    {
        press_r_time = 0;
    }

    if (press_r_time > PRESS_TIME)
    {
        aim_set->auto_aim_flag = (aim_set->auto_aim_flag == AIM_OFF) ? AIM_ON : AIM_OFF;
    }
}

void auto_aim_feedback_update(auto_aim_t *aim_update)
{
    if (aim_update == NULL)
    {
        return;
    }

    if (aim_update->auto_aim_flag == AIM_OFF)
    {
        s_target_valid = 0;              // 清除目标有效标志
        s_fire_pulse_pending = 0;        // 清除待发射脉冲标志
        s_lock_enter_ms = 0;             // 清除锁定进入时间戳
        return;
    }

    uint8_t id = get_robot_id();
    (void)id;

#if AUTO_AIM_UPROTO_TICK_ENABLE
    /* let comm_app drive protocol tick to avoid re-entrancy */
    /* 由comm_app驱动协议tick，避免重复调用 */
    uproto_tick(&proto_ctx);
#endif
		
    // clamp raw receive values to configured bounds
    if (aim_update->receive.yaw > MAX_YAW)   aim_update->receive.yaw = MAX_YAW;
    else if (aim_update->receive.yaw < MIN_YAW)   aim_update->receive.yaw = MIN_YAW;
		else aim_update->receive.yaw = 0.0f;

    if (aim_update->receive.pitch > MAX_PITCH) aim_update->receive.pitch = MAX_PITCH;
    else if (aim_update->receive.pitch < MIN_PITCH) aim_update->receive.pitch = MIN_PITCH;
    else aim_update->receive.pitch = 0.0f;
			
    aim_update->shoot_delay = 0;
    aim_update->yaw_delay = 0;
    aim_update->pitch_delay = 0;
		aim.last_fdb = HAL_GetTick();
}

static fp32 orn_yaw = 0.0f;

static fp32 orn_pitch = 0.0f;

// DM-02 云台 DELTA 状态位
// - bit0: target_valid（主机有效目标/命令）
static volatile uint8_t s_target_valid = 0;
// 自动射击脉冲待处理标志（单次脉冲）
static volatile uint8_t s_fire_pulse_pending = 0;

// 自动射击调参（可根据需要调整）
#define AA_FIRE_YAW_ERR_RAD   (1.5f * PI / 180.0f)      // 偏航轴允许误差阈值（约1.5度）
#define AA_FIRE_PITCH_ERR_RAD (1.5f * PI / 180.0f)      // 俯仰轴允许误差阈值（约1.5度）
#define AA_FIRE_HOLD_MS       80U                        // 目标锁定保持时间（毫秒）
#define AA_FIRE_PERIOD_MS     200U                       // 连续射击最小间隔（毫秒）

// 目标锁定进入时间戳（毫秒）
static uint32_t s_lock_enter_ms = 0;
// 上次射击时间戳（毫秒）
static uint32_t s_last_fire_ms = 0;

void auto_aim_task(void const *pvParameters)
{
    (void)pvParameters;
		fp32 debug_vofa[10] = {0.0f, 0.0f};
    osDelay(AIM_INIT_TIME);
    auto_aim_init(&aim);
		
    while (1)
    {
        // [SYNC_FROM_H] Task loop now drives internal control tick each cycle
        auto_aim_set(&aim);
        auto_aim_feedback_update(&aim);
        auto_aim_control_tick_internal(&aim);
				debug_vofa[0] = aim.receive.pitch;
				debug_vofa[1] = gimbal_control.gimbal_pitch_motor.relative_angle_set;
				debug_vofa[2] = gimbal_control.gimbal_pitch_motor.relative_angle;
				debug_vofa[3] = gimbal_control.gimbal_pitch_motor.gimbal_motor_relative_angle_pid.out;
				debug_vofa[4] = aim.receive.yaw;
				debug_vofa[5] = gimbal_control.gimbal_yaw_motor.absolute_angle_set;
				debug_vofa[6] = gimbal_control.gimbal_yaw_motor.absolute_angle;
				debug_vofa[7] =aim.yaw_delay   ;
				debug_vofa[8] =aim.pitch_delay ;
				debug_vofa[9] =aim.shoot_delay ;
				Vofa_JustFloat_AUTO(debug_vofa);
			
			
        osDelay(AUTO_AIM_TIME);
    }
}

static float dyaw_rad;
static float dpitch_rad;

void auto_aim_apply_delta_udeg(int32_t dyaw_udeg,
                               int32_t dpitch_udeg,
                               uint16_t status,
                               uint64_t ts_us)
{
    // 状态位0：目标有效标志
    // status & 0x0001u：提取状态字的第0位
    // 三元运算符：如果第0位为1则target_valid=1，否则为0
    const uint8_t target_valid = (status & 0x0001u) ? 1u : 0u;
    // 更新全局目标有效标志
    s_target_valid = target_valid;
    (void)ts_us;

    // micro-degree -> rad
    // float dyaw_rad   = ((float)dyaw_udeg)   * PI / 180000000.0f;
    // float dpitch_rad = ((float)dpitch_udeg) * PI / 180000000.0f;
	
		dyaw_rad   = ((float)dyaw_udeg)   * PI / 180000000.0f;
		dpitch_rad = ((float)dpitch_udeg) * PI / 180000000.0f;
		
		orn_pitch = dpitch_rad;
		orn_yaw = dyaw_rad;
	
    // Interpret host command directly as “需要转动的误差”（正误差 → 正向转动）
    // [SYNC_FROM_H] Host deltas now feed internal controller instead of direct motor commands
    s_auto_aim_ctrl.yaw_axis.err_rad   = dyaw_rad;
    s_auto_aim_ctrl.pitch_axis.err_rad = dpitch_rad;
    // 自动射击权限判断：目标有效 + 锁定（误差小且持续一定时间） + 射击频率限制
    uint32_t now_ms = HAL_GetTick();
    
    // 如果自瞄关闭、离线或无有效目标，清除锁定状态和待发射标志
    if (aim.auto_aim_flag != AIM_ON || !aim.online || !target_valid) {
        s_lock_enter_ms = 0;              // 清除锁定进入时间戳
        s_fire_pulse_pending = 0;         // 清除待发射脉冲标志
    } else {
        // 检查偏航和俯仰轴误差是否都在允许范围内
        if (fabsf(dyaw_rad) < AA_FIRE_YAW_ERR_RAD && fabsf(dpitch_rad) < AA_FIRE_PITCH_ERR_RAD) {
            // 如果刚进入锁定状态，记录当前时刻
            if (s_lock_enter_ms == 0U) s_lock_enter_ms = now_ms;
            
            // 如果锁定时间已达到保持时间阈值
            if ((now_ms - s_lock_enter_ms) >= AA_FIRE_HOLD_MS) {
                // 检查是否满足射击频率限制（与上次射击间隔足够大）
                if ((now_ms - s_last_fire_ms) >= AA_FIRE_PERIOD_MS) {
                    s_fire_pulse_pending = 1;  // 置位待发射脉冲标志
                    s_last_fire_ms = now_ms;   // 更新上次射击时间戳
                }
            }
        } else {
            // 如果误差超出范围，清除锁定状态
            s_lock_enter_ms = 0;
        }
    }

    // keep auto-aim online / alive
    aim.last_fdb = HAL_GetTick();
}

// 获取自瞄目标有效状态
// 返回值：1 表示目标有效，0 表示目标无效
uint8_t auto_aim_target_valid(void)
{
    return s_target_valid ? 1u : 0u;
}

// 获取自动射击脉冲标志并清除
// 返回值：1 表示有待发射的脉冲，0 表示无待发射脉冲
// 调用此函数会自动清除标志位，确保单次脉冲只被处理一次
uint8_t auto_aim_fire_pulse(void)
{
    uint8_t p = s_fire_pulse_pending ? 1u : 0u;
    s_fire_pulse_pending = 0;  // 清除待发射脉冲待处理标志
    return p;
}

void auto_aim_control_tick(auto_aim_t *aim_loop)
{
    auto_aim_control_tick_internal(aim_loop);
}

// Core MCU-side control: error -> limited velocity -> per-cycle angle increment (rad)
// [SYNC_FROM_H] New PD-like shaping with slew limits for smooth motion
static void auto_aim_control_tick_internal(auto_aim_t *aim_loop)
{
    if (aim_loop == NULL)
    {
        return;
    }

    uint32_t now_ms = HAL_GetTick();
    float dt_s;
    if (s_auto_aim_ctrl.last_tick_ms == 0U)
    {
        dt_s = (float)AUTO_AIM_TIME / 1000.0f;
    }
    else
    {
        uint32_t diff_ms = now_ms - s_auto_aim_ctrl.last_tick_ms;
        dt_s = (float)diff_ms / 1000.0f;
    }
    s_auto_aim_ctrl.last_tick_ms = now_ms;
    dt_s = aa_clamp(dt_s, 0.0005f, 0.02f); // 0.5–20 ms safety clamp

    // When auto-aim is off or offline, do not inject motion
    if (aim_loop->auto_aim_flag == AIM_OFF || !aim_loop->online)
    {
        s_auto_aim_ctrl.yaw_axis.omega_cmd_rad   = 0.0f;
        s_auto_aim_ctrl.pitch_axis.omega_cmd_rad = 0.0f;
        aim.receive.yaw   = 0.0f;
        aim.receive.pitch = 0.0f;
        return;
    }

    // Yaw axis: error -> omega_ref -> omega_cmd with accel limit -> angle increment
    {
        float err = s_auto_aim_ctrl.yaw_axis.err_rad;
        float e_abs = fabsf(err);

        // 非线性 Kv：远处用大增益，接近目标自动降低增益以减小冲击与振荡
        float kv = AA_KV_YAW;
        if (e_abs < AA_ERR_SOFT_ZONE_RAD)
        {
            float scale = e_abs / (AA_ERR_SOFT_ZONE_RAD + 1e-6f); // 0~1
            // Kv 在 [0.5, 1.0] * AA_KV_YAW 之间线性变化
            kv *= (0.5f + 0.5f * scale);
        }

        // 误差导数（简单 D），并做一阶低通
        float derr = (err - s_last_err_yaw) / dt_s;
        s_last_err_yaw = err;
        s_err_d_yaw = AA_D_LPF_ALPHA * derr + (1.0f - AA_D_LPF_ALPHA) * s_err_d_yaw;

        float omega_ref = kv * err + AA_KD_YAW * s_err_d_yaw;
        omega_ref = aa_clamp(omega_ref, -AA_OMEGA_MAX_YAW, AA_OMEGA_MAX_YAW);

        // 误差越小，加速度上限越小 → 目标附近更平滑
        float a_max = AA_A_MAX_YAW;
        const float soft_a_err = 10.0f * PI / 180.0f; // ~10deg
        if (e_abs < soft_a_err)
        {
            float scale = e_abs / (soft_a_err + 1e-3f); // 0~1
            a_max *= (0.3f + 0.7f * scale);             // 保留至少 30% 的加速度能力
        }
        float domega_max = a_max * dt_s;
        float domega = omega_ref - s_auto_aim_ctrl.yaw_axis.omega_cmd_rad;
        domega = aa_clamp(domega, -domega_max, domega_max);
        s_auto_aim_ctrl.yaw_axis.omega_cmd_rad += domega;

        if ((fabsf(err) < AA_EPS_TH_RAD) &&
            (fabsf(s_auto_aim_ctrl.yaw_axis.omega_cmd_rad) < AA_EPS_OMEGA_RAD))
        {
            s_auto_aim_ctrl.yaw_axis.omega_cmd_rad = 0.0f;
        }

        float dtheta = s_auto_aim_ctrl.yaw_axis.omega_cmd_rad * dt_s;
        aim.receive.yaw = dtheta;
    }

    // Pitch axis
    {
        float err = s_auto_aim_ctrl.pitch_axis.err_rad;
        float e_abs = fabsf(err);

        float kv = AA_KV_PITCH;
        if (e_abs < AA_ERR_SOFT_ZONE_RAD)
        {
            float scale = e_abs / (AA_ERR_SOFT_ZONE_RAD + 1e-6f);
            kv *= (0.5f + 0.5f * scale);
        }

        float derr = (err - s_last_err_pitch) / dt_s;
        s_last_err_pitch = err;
        s_err_d_pitch = AA_D_LPF_ALPHA * derr + (1.0f - AA_D_LPF_ALPHA) * s_err_d_pitch;

        float omega_ref = kv * err + AA_KD_PITCH * s_err_d_pitch;
        omega_ref = aa_clamp(omega_ref, -AA_OMEGA_MAX_PITCH, AA_OMEGA_MAX_PITCH);

        float a_max = AA_A_MAX_PITCH;
        const float soft_a_err = 10.0f * PI / 180.0f;
        if (e_abs < soft_a_err)
        {
            float scale = e_abs / (soft_a_err + 1e-3f);
            a_max *= (0.3f + 0.7f * scale);
        }
        float domega_max = a_max * dt_s;
        float domega = omega_ref - s_auto_aim_ctrl.pitch_axis.omega_cmd_rad;
        domega = aa_clamp(domega, -domega_max, domega_max);
        s_auto_aim_ctrl.pitch_axis.omega_cmd_rad += domega;

        if ((fabsf(err) < AA_EPS_TH_RAD) &&
            (fabsf(s_auto_aim_ctrl.pitch_axis.omega_cmd_rad) < AA_EPS_OMEGA_RAD))
        {
            s_auto_aim_ctrl.pitch_axis.omega_cmd_rad = 0.0f;
        }

        float dtheta = s_auto_aim_ctrl.pitch_axis.omega_cmd_rad * dt_s;
        aim.receive.pitch = dtheta;
    }
		aim.receive.yaw   = mf5_update(&yawfilter, aim.receive.yaw);
		aim.receive.pitch = mf5_update(&pitchfilter, aim.receive.pitch);
}

