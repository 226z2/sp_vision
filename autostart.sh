#!/usr/bin/env bash
set -e

ARGS=("$@")

# 容器内自启脚本：
# - 可选：补一条 CAN 口配置（宿主已通过 udev up can0，失败不影响启动）
# - 切到项目根目录
# - 启动主程序（可自行改为 standard / sentry 等）

# 1. 激活ROS2环境
echo "激活ROS2环境..."
if [ -f "/opt/ros/jazzy/setup.bash" ]; then
    source /opt/ros/jazzy/setup.bash
    echo "✓ ROS2基础环境已激活 (ROS_DISTRO=${ROS_DISTRO})"
else
    echo "警告: 未找到ROS2安装" >&2
fi

# 2. 激活自定义消息包
echo "激活自定义消息包..."
if [ -f "/ros2_ws/install/setup.bash" ]; then
    source /ros2_ws/install/setup.bash
    echo "✓ 自定义消息包已激活"
elif [ -f "/app/sp_vision/ros2_ws/install/setup.bash" ]; then
    source /app/sp_vision/ros2_ws/install/setup.bash
    echo "✓ 项目内消息包已激活"
else
    echo "提示: 未找到自定义消息包" >&2
fi

ip link set can0 up type can bitrate 1000000 2>/dev/null || true

cd /app/sp_vision

# 默认主程序：自瞄测试
# - 允许通过 docker run 的参数透传给可执行文件，例如：
#   docker run ... <image> --config-path configs/standard_mpc_dm02.yaml --source camera ...
# - 允许通过 docker run 的参数透传给可执行文件，例如：
#   docker run ... <image> --config-path configs/standard_mpc_dm02.yaml --source camera ...
if [ "${#ARGS[@]}" -eq 0 ]; then
  exec ./build/camera_detect_test
else
  exec ./build/camera_detect_test "${ARGS[@]}"
fi
