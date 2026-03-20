#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_NAME="${IMAGE_NAME:-sp_vision_25:dev_5}"
CONTAINER_NAME="${CONTAINER_NAME:-spv_dm_loop}"
WS_PORT="${WS_PORT:-8878}"
BUILD_DIR="${BUILD_DIR:-build}"
RECEIVER_BIN="/app/sp_vision/${BUILD_DIR}/dm_ws_bridge_test"
SENDER_BIN="/app/sp_vision/${BUILD_DIR}/dm_virtual_sender_test"
VENV_PYTHON="${VENV_PYTHON:-/tmp/spv-ws-venv/bin/python}"

info() {
  printf '[dm-ws-demo] %s\n' "$*"
}

cleanup_container() {
  docker rm -f "${CONTAINER_NAME}" >/dev/null 2>&1 || true
}

cleanup_port() {
  if command -v fuser >/dev/null 2>&1; then
    fuser -k "${WS_PORT}/tcp" >/dev/null 2>&1 || true
  fi
}

wait_for_file_line() {
  local container_path="$1"
  local prefix="$2"
  local attempts=50
  while (( attempts > 0 )); do
    local line
    line="$(docker exec "${CONTAINER_NAME}" /bin/bash -lc "grep '^${prefix}' '${container_path}' 2>/dev/null | tail -n1" || true)"
    if [[ -n "${line}" ]]; then
      printf '%s\n' "${line#${prefix}}"
      return 0
    fi
    sleep 0.2
    attempts=$((attempts - 1))
  done
  return 1
}

docker_exec_bg() {
  local log_file="$1"
  shift
  docker exec -d "${CONTAINER_NAME}" /bin/bash -lc "$* >'${log_file}' 2>&1"
}

ensure_binaries() {
  docker exec "${CONTAINER_NAME}" /bin/bash -lc "
    test -x '${RECEIVER_BIN}' && test -x '${SENDER_BIN}'
  "
}

main() {
  info "cleaning previous container and port ${WS_PORT}"
  cleanup_container
  cleanup_port

  info "starting container ${CONTAINER_NAME}"
  docker run -d \
    --name "${CONTAINER_NAME}" \
    --privileged \
    -p "${WS_PORT}:${WS_PORT}" \
    -e SPV_WS_ENABLE=1 \
    -e SPV_WS_PORT="${WS_PORT}" \
    -v "${ROOT_DIR}:/app/sp_vision" \
    -v "${ROOT_DIR}/data:/app/data" \
    -w /app/sp_vision \
    --entrypoint /bin/bash \
    "${IMAGE_NAME}" \
    -lc 'trap : TERM INT; sleep infinity & wait' >/dev/null

  info "checking test binaries"
  if ! ensure_binaries; then
    info "missing binaries, building dm_virtual_sender_test and dm_ws_bridge_test"
    docker exec "${CONTAINER_NAME}" /bin/bash -lc "
      source /opt/ros/jazzy/setup.bash
      if [ -f /app/sp_vision/ros2_ws/install/setup.bash ]; then source /app/sp_vision/ros2_ws/install/setup.bash; fi
      cmake -S /app/sp_vision -B /app/sp_vision/${BUILD_DIR}
      cmake --build /app/sp_vision/${BUILD_DIR} -j 4 --target dm_virtual_sender_test dm_ws_bridge_test
    "
  fi

  info "starting virtual serial relay"
  docker_exec_bg /tmp/dm_virtual_serial_pair.log "python3 /app/sp_vision/tools/virtual_serial_pair.py"

  local serial_a serial_b
  serial_a="$(wait_for_file_line /tmp/dm_virtual_serial_pair.log 'SERIAL_A=')"
  serial_b="$(wait_for_file_line /tmp/dm_virtual_serial_pair.log 'SERIAL_B=')"
  if [[ -z "${serial_a}" || -z "${serial_b}" ]]; then
    info "failed to get virtual serial pair paths"
    docker exec "${CONTAINER_NAME}" /bin/bash -lc "cat /tmp/dm_virtual_serial_pair.log || true"
    exit 2
  fi

  info "virtual serial pair: A=${serial_a} B=${serial_b}"

  info "starting DM->WS receiver"
  docker_exec_bg /tmp/dm_ws_bridge_test.log "
    source /opt/ros/jazzy/setup.bash
    if [ -f /app/sp_vision/ros2_ws/install/setup.bash ]; then source /app/sp_vision/ros2_ws/install/setup.bash; fi
    exec '${RECEIVER_BIN}' 'serial:${serial_b}?baud=115200'
  "

  sleep 1

  info "starting virtual DM sender"
  docker_exec_bg /tmp/dm_virtual_sender_test.log "
    exec '${SENDER_BIN}' 'serial:${serial_a}?baud=115200' --quiet
  "

  sleep 1

  info "demo is running"
  info "container logs:"
  info "  docker exec ${CONTAINER_NAME} /bin/bash -lc 'tail -f /tmp/dm_ws_bridge_test.log /tmp/dm_virtual_sender_test.log /tmp/dm_virtual_serial_pair.log'"
  info "host receiver:"
  info "  ${VENV_PYTHON} ${ROOT_DIR}/tools/ws_receive.py --uri ws://127.0.0.1:${WS_PORT} --type dm02_serial_copy"
  info "frontend websocket:"
  info "  ws://127.0.0.1:${WS_PORT}"
  info "stop:"
  info "  docker rm -f ${CONTAINER_NAME}"
}

main "$@"
