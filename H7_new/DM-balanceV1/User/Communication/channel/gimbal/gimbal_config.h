#ifndef GIMBAL_CONFIG_H
#define GIMBAL_CONFIG_H

#include <stdint.h>
#include "../../core/comm_utils.h"


#define gmb_le16 comm_write_u16_le
#define gmb_le32(p,v) comm_write_i32_le(p,v)
#define gmb_le64 comm_write_u64_le

#ifdef __cplusplus
extern "C" {
#endif

/* 逻辑通道号与默认优先级 */
#ifndef GIMBAL_CH_ID
#define GIMBAL_CH_ID 4u
#endif

#ifndef GIMBAL_PRIORITY
#define GIMBAL_PRIORITY 3u
#endif

/* 默认状态发布周期（毫秒） */
#ifndef GIMBAL_PUB_PERIOD_MS
#define GIMBAL_PUB_PERIOD_MS 10u
#endif

/* 状态负载字段开关（1 启用 / 0 关闭） */
#ifndef GIMBAL_STATE_HAS_ENCODERS
#define GIMBAL_STATE_HAS_ENCODERS 1
#endif
#ifndef GIMBAL_STATE_HAS_IMU
#define GIMBAL_STATE_HAS_IMU 1
#endif

/* 是否在收到主机 DELTA 命令后回 ACK（1=回执，0=不回，省带宽） */
#ifndef GIMBAL_DELTA_ACK_ENABLE
#define GIMBAL_DELTA_ACK_ENABLE 0
#endif


/* 可选扩展钩子：定义为一个宏，用于在状态负载末尾追加自定义字节，并返回追加的长度（字节数）。
 * 示例：
 *   #define GIMBAL_STATE_PACK_EXT(p,enc_y,enc_p,yaw,pitch,roll,ts) \\
 *           ( gmb_le32((p), my_temp_cdeg), 4 )
 */
#ifndef GIMBAL_STATE_PACK_EXT

/*
 * 扩展云台状态负载（设备 -> 主机），追加额外的 32 位字。
 * 主机驱动逐个解析这些字，顺序为：
 *   yaw_cmd_current, pitch_cmd_current, yaw_meas_current, pitch_meas_current,
 *   gyro_yaw_udeps, gyro_pitch_udeps
 */
#include "gimbal_task.h"
#include "shoot.h"
#include "auto_aim.h"
#include "referee.h" /* 用于 shoot_data / projectile_allowance / heat */

/* referee.c 全局变量（在通信负载扩展中声明） */
extern shoot_data_t shoot_data;
extern projectile_allowance_t projectile_allowance;

/* 云台状态扩展负载打包函数 */
static inline uint16_t gimbal_state_pack_ext(uint8_t *p)
{
    uint16_t pos = 0;

    /* 索引 0..5: 与主机侧解析器兼容 */
    gmb_le32(&p[pos], (int32_t)yaw_can_set_current); pos += 4;      /* Yaw 命令电流 */
    gmb_le32(&p[pos], (int32_t)pitch_can_set_current); pos += 4;    /* Pitch 命令电流 */

    /* 获取云台电机指针 */
    const gimbal_motor_t *yaw = get_yaw_motor_point();
    const gimbal_motor_t *pit = get_pitch_motor_point();

    /* 读取实际电流值 */
    int32_t yaw_meas_cur = 0;
    int32_t pit_meas_cur = 0;
    if (yaw && yaw->gimbal_motor_measure) yaw_meas_cur = (int32_t)yaw->gimbal_motor_measure->given_current;
    if (pit && pit->gimbal_motor_measure) pit_meas_cur = (int32_t)pit->gimbal_motor_measure->given_current;

    gmb_le32(&p[pos], yaw_meas_cur); pos += 4;                      /* Yaw 实际电流 */
    gmb_le32(&p[pos], pit_meas_cur); pos += 4;                      /* Pitch 实际电流 */

    /* 陀螺仪速率转换：弧度 -> 微度/秒 */
    const float RAD2UDEG = 180000000.0f / 3.14159265358979323846f;
    int32_t gyro_yaw_udeps = 0;
    int32_t gyro_pit_udeps = 0;
    if (yaw) gyro_yaw_udeps = (int32_t)(yaw->motor_gyro * RAD2UDEG);
    if (pit) gyro_pit_udeps = (int32_t)(pit->motor_gyro * RAD2UDEG);

    gmb_le32(&p[pos], gyro_yaw_udeps); pos += 4;                    /* Yaw 陀螺仪速率 */
    gmb_le32(&p[pos], gyro_pit_udeps); pos += 4;                    /* Pitch 陀螺仪速率 */

    /* 索引 6+: 应用状态反馈 */
    
    /* 弹丸初速（单位：cm/s） */
    int32_t bullet_speed_x100 = 0;
    if (shoot_data.initial_speed > 0.1f) {
        bullet_speed_x100 = (int32_t)(shoot_data.initial_speed * 100.0f);
    }
    gmb_le32(&p[pos], bullet_speed_x100); pos += 4;

    /* 射击弹丸计数 */
    gmb_le32(&p[pos], (int32_t)shoot_bullet_count); pos += 4;

    /* 云台模式：0=空闲,1=自动瞄准,2=小符,3=大符（目前仅使用空闲/自动瞄准） */
    int32_t gimbal_mode = (aim.auto_aim_flag == AIM_ON) ? 1 : 0;
    gmb_le32(&p[pos], gimbal_mode); pos += 4;

    /* 射击模式 */
    gmb_le32(&p[pos], (int32_t)shoot_control.shoot_mode); pos += 4;

    /* 获取当前热量和热量限制 */
    uint16_t heat_limit = 0;
    uint16_t heat = 0;
    get_shoot_heat0_limit_and_heat0(&heat_limit, &heat);
    gmb_le32(&p[pos], (int32_t)heat); pos += 4;                     /* 当前热量 */
    gmb_le32(&p[pos], (int32_t)heat_limit); pos += 4;               /* 热量限制 */

    /* 弹丸余量（17mm 和 42mm） */
    gmb_le32(&p[pos], (int32_t)projectile_allowance.projectile_allowance_17mm); pos += 4;
    gmb_le32(&p[pos], (int32_t)projectile_allowance.projectile_allowance_42mm); pos += 4;

    return pos;
}

/* 宏定义：调用扩展打包函数 */
#define GIMBAL_STATE_PACK_EXT(p, enc_y, enc_p, yaw_u, pitch_u, roll_u, ts_u) (gimbal_state_pack_ext((p)))
#endif

/* 按照上述开关把云台状态打包到缓冲区，返回总长度（字节） */
static inline uint16_t gimbal_pack_state(uint8_t *buf, uint16_t sid,
                                         int32_t enc_yaw, int32_t enc_pitch,
                                         int32_t yaw_udeg, int32_t pitch_udeg, int32_t roll_udeg,
                                         uint64_t ts_us)
{
    uint16_t pos = 0;
    gmb_le16(&buf[pos], sid); pos += 2;
#if GIMBAL_STATE_HAS_ENCODERS
    gmb_le32(&buf[pos], enc_yaw);   pos += 4;
    gmb_le32(&buf[pos], enc_pitch); pos += 4;
#endif
#if GIMBAL_STATE_HAS_IMU
    gmb_le32(&buf[pos], yaw_udeg);   pos += 4;
    gmb_le32(&buf[pos], pitch_udeg); pos += 4;
    gmb_le32(&buf[pos], roll_udeg);  pos += 4;
#endif
    gmb_le64(&buf[pos], ts_us); pos += 8;
    /* Optional extension controlled by config */
    pos = (uint16_t)(pos + GIMBAL_STATE_PACK_EXT(&buf[pos], enc_yaw, enc_pitch, yaw_udeg, pitch_udeg, roll_udeg, ts_us));
    return pos;
}

#ifdef __cplusplus
}
#endif

#endif /* GIMBAL_CONFIG_H */
