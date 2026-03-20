// 自瞄复制然后运行 （只复制其中一个，第一个不行换第二个，按ctrl+C退出程序）
make spv-run \
    USE_HOST_NET=no \
    FORWARD_PORTS="8877" \
    SPV_FOXGLOVE_PORT=8877 \
    SPV_KILL_BEFORE_RUN=1 \
    SPV_BUILD_DIR=build \
    SPV_RUN_TARGET=auto_aim_test \
    GIMBAL_DEV_HOST=/dev/ttyACM0 \
    SPV_RUN_ARGS="--source=camera --config-path=configs/sentry.yaml --dm-endpoint=serial:/dev/ttyACM0?baud=115200 --dm-send=true --allow-shoot=false --bullet-speed=27"

make spv-run \
    USE_HOST_NET=no \
    FORWARD_PORTS="8877" \
    SPV_FOXGLOVE_PORT=8877 \
    SPV_KILL_BEFORE_RUN=1 \
    SPV_BUILD_DIR=build \
    SPV_RUN_TARGET=auto_aim_test \
    GIMBAL_DEV_HOST=/dev/ttyACM1 \
    SPV_RUN_ARGS="--source=camera --config-path=configs/sentry.yaml --dm-endpoint=serial:/dev/ttyACM1?baud=115200 --dm-send=true --allow-shoot=false --bullet-speed=27"