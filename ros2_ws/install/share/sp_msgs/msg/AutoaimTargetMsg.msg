# 上层下发自瞄目标信息

# 消息生成时间
builtin_interfaces/Time timestamp

# 当前关注/候选目标 ID 列表，前面的优先级更高
# 例如：[5, 3, 1] 表示优先打 5 号，再是 3 号和 1 号
int8[] target_ids

# ===== 以下为可选扩展字段 =====

# 当前首要目标 ID
# 如果没有明确首要目标，可以设为 -1
int8 main_target_id

# 是否当前有有效目标
bool has_target

# 期望云台大致朝向的水平角度（度）
float32 yaw_deg

# 期望云台大致朝向的俯仰角（度）
float32 pitch_deg

# 目标大致距离（米）
float32 distance_m

# 目标在机器人坐标系下的速度估计（米/秒）
float32 vx
float32 vy
float32 vz

# 上层对当前目标集合的“信心”，范围 0.0 ~ 1.0
float32 confidence

# 射击指令：0=不干预, 1=建议开火, 2=禁止开火
uint8 shoot_command