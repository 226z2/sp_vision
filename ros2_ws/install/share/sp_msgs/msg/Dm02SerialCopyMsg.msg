# Message timestamp
builtin_interfaces/Time timestamp

# Link/driver status
bool established
bool have_state
uint64 send_ok
uint64 send_fail

# Gimbal state copied from DM02 serial
float32 yaw_rad
float32 pitch_rad
float32 roll_rad
int32 enc_yaw
int32 enc_pitch
float32 yaw_vel_rad_s
float32 pitch_vel_rad_s
float32 bullet_speed_mps
uint16 bullet_count
int32 gimbal_mode
int32 shoot_state
int32 shooter_heat
int32 shooter_heat_limit
int32 projectile_allowance_17mm
int32 projectile_allowance_42mm
uint64 state_device_ts_us
uint64 state_host_ts_ns

# Referee state copied from DM02 serial
bool referee_valid
int32 referee_enemy_team
bool referee_fire_allowed
int32 referee_robot_id
int32 referee_game_stage
uint16 referee_status
uint64 referee_device_ts_us
uint64 referee_host_ts_ns

# ToF/TFMini state copied from DM02 serial
bool tof_valid
uint16 tof_distance_cm
uint16 tof_strength
int16 tof_temp_cdeg
uint16 tof_status
uint64 tof_device_ts_us
uint64 tof_host_ts_ns

# Time sync state copied from DM02 serial
bool timesync_valid
int64 timesync_offset_us
uint32 timesync_rtt_us
uint32 timesync_version
uint64 timesync_last_device_time_us
uint64 timesync_last_host_time_us
