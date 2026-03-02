# 上层下发敌方整体状态

# 消息生成时间
builtin_interfaces/Time timestamp

# 当前处于“无敌状态”的敌方 ID 列表
# 例如：[3, 4] 表示 3 号与 4 号暂时打不动
int8[] invincible_enemy_ids

# ===== 以下为可选扩展字段（可以根据需要删减或不用） =====

# 比赛阶段：0=未知, 1=准备, 2=对战中, 3=加时, 4=结束
int8 game_stage

# 场上存活的敌方单位数量
int8 enemy_count

# 是否允许开火：true=允许，false=禁止（比如裁判区等）
bool fire_allowed

# 敌方阵营/颜色：0=未知, 1=RED, 2=BLUE
int8 enemy_team

# 上层认为当前存在的敌方 ID 列表（用于和视觉检测结果对比）
int8[] detected_enemy_ids