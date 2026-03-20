#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="${SPV_ROOT_DIR:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
IMAGE_NAME="${IMAGE_NAME:-sp_vision_25}"
TAG="${TAG:-dev_5}"
CONTAINER_NAME="${SPV_SUPERVISOR_CONTAINER_NAME:-spv_supervised_app}"
DATA_DIR="${DATA_DIR:-${ROOT_DIR}/data}"
USE_HOST_NET="${USE_HOST_NET:-yes}"
FORWARD_PORTS="${FORWARD_PORTS:-4000 4002 9877 8765}"
DOCKER_PRIVILEGED_FLAGS="${DOCKER_PRIVILEGED_FLAGS:---privileged}"
ENABLE_GPU="${ENABLE_GPU:-0}"
GIMBAL_DEV_CONT="${GIMBAL_DEV_CONT:-/dev/gimbal}"
SPV_BUILD_DIR="${SPV_BUILD_DIR:-build}"
SPV_RUN_TARGET="${SPV_RUN_TARGET:-auto_aim_test}"
SPV_RUN_ARGS="${SPV_RUN_ARGS:---source=camera --config-path=configs/sentry.yaml --dm-send=true --allow-shoot=false --bullet-speed=27}"
SPV_AUTOSTART="${SPV_AUTOSTART:-1}"
SPV_SUPERVISE="${SPV_SUPERVISE:-1}"
SPV_REQUIRED_CAMERA="${SPV_REQUIRED_CAMERA:-1}"
SPV_REQUIRED_SERIAL="${SPV_REQUIRED_SERIAL:-1}"
SPV_SERIAL_CANDIDATES="${SPV_SERIAL_CANDIDATES:-/dev/gimbal,/dev/ttyACM0,/dev/ttyACM1,/dev/ttyACM2,/dev/ttyUSB0,/dev/ttyUSB1,/dev/ttyUSB2}"
SPV_SERIAL_BAUD="${SPV_SERIAL_BAUD:-115200}"
SPV_RESTART_DELAY_MS="${SPV_RESTART_DELAY_MS:-1500}"
SPV_STARTUP_TIMEOUT_SEC="${SPV_STARTUP_TIMEOUT_SEC:-0}"
SPV_CONFIG_PATH="${SPV_CONFIG_PATH:-}"
SPV_FOXGLOVE_PORT="${SPV_FOXGLOVE_PORT:-8765}"
SPV_WS_ENABLE="${SPV_WS_ENABLE:-1}"
SPV_WS_PORT="${SPV_WS_PORT:-8878}"
SPV_WS_BIND_HOST="${SPV_WS_BIND_HOST:-0.0.0.0}"
SPV_FAIL_NOTIFY_ENABLE="${SPV_FAIL_NOTIFY_ENABLE:-1}"
SPV_FAIL_NOTIFY_HOST="${SPV_FAIL_NOTIFY_HOST:-127.0.0.1}"
SPV_FAIL_NOTIFY_PORT="${SPV_FAIL_NOTIFY_PORT:-18878}"
SPV_FAIL_NOTIFY_SERVICE_NAME="${SPV_FAIL_NOTIFY_SERVICE_NAME:-spv_supervisor}"
SPV_LOG_DIR="${SPV_LOG_DIR:-${ROOT_DIR}/logs/supervisor}"

LAST_FAILURE_SIGNATURE=""
CURRENT_SERIAL_HOST=""

info() {
  printf '[spv-supervisor] %s\n' "$*"
}

warn() {
  printf '[spv-supervisor] warning: %s\n' "$*" >&2
}

error() {
  printf '[spv-supervisor] error: %s\n' "$*" >&2
}

bool_is_true() {
  case "${1:-}" in
    1|true|TRUE|True|yes|YES|on|ON) return 0 ;;
    *) return 1 ;;
  esac
}

trim() {
  local value="${1:-}"
  value="${value#"${value%%[![:space:]]*}"}"
  value="${value%"${value##*[![:space:]]}"}"
  printf '%s' "${value}"
}

json_escape() {
  python3 -c 'import json,sys; print(json.dumps(sys.argv[1]))' "${1:-}"
}

maybe_map_port() {
  local port="${1:-}"
  local normalized=" ${FORWARD_PORTS} "
  if bool_is_true "${USE_HOST_NET}"; then
    return 0
  fi
  if [[ -n "${port}" && "${normalized}" != *" ${port} "* ]]; then
    printf ' -p %s:%s' "${port}" "${port}"
  fi
}

network_args() {
  if bool_is_true "${USE_HOST_NET}"; then
    printf ' --network=host'
    return 0
  fi

  local arg
  for arg in ${FORWARD_PORTS}; do
    printf ' -p %s:%s' "${arg}" "${arg}"
  done
  printf '%s' "$(maybe_map_port "${SPV_FOXGLOVE_PORT}")"
  printf '%s' "$(maybe_map_port "${SPV_WS_PORT}")"
}

gpu_args() {
  if [[ "${ENABLE_GPU}" == "1" ]]; then
    printf ' -v /dev/dri:/dev/dri'
  fi
}

yaml_value() {
  local file="$1"
  local key="$2"
  if [[ ! -f "${file}" ]]; then
    return 1
  fi
  python3 - "$file" "$key" <<'PY'
import re
import sys

path, key = sys.argv[1], sys.argv[2]
pattern = re.compile(r"^\s*" + re.escape(key) + r"\s*:\s*(.+?)\s*$")
with open(path, "r", encoding="utf-8") as f:
    for line in f:
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        match = pattern.match(line)
        if not match:
            continue
        value = match.group(1).strip()
        if value.startswith(("\"", "'")) and value.endswith(("\"", "'")) and len(value) >= 2:
            value = value[1:-1]
        print(value)
        break
PY
}

detect_config_path() {
  if [[ -n "${SPV_CONFIG_PATH}" ]]; then
    printf '%s' "${SPV_CONFIG_PATH}"
    return 0
  fi
  python3 - "${SPV_RUN_ARGS}" <<'PY'
import shlex
import sys

args = shlex.split(sys.argv[1])
value = ""
for index, token in enumerate(args):
    if token in ("--config-path", "-c") and index + 1 < len(args):
        value = args[index + 1]
    elif token.startswith("--config-path="):
        value = token.split("=", 1)[1]
print(value)
PY
}

resolve_target_host_path() {
  local target="${SPV_RUN_TARGET}"
  local candidate=""
  if [[ -x "${target}" ]]; then
    printf '%s' "${target}"
    return 0
  fi
  for candidate in \
    "${ROOT_DIR}/${target}" \
    "${ROOT_DIR}/${SPV_BUILD_DIR}/${target}" \
    "${ROOT_DIR}/${SPV_BUILD_DIR}/bin/${target}"
  do
    if [[ -x "${candidate}" ]]; then
      printf '%s' "${candidate}"
      return 0
    fi
  done
  return 1
}

notify_udp_json() {
  local type="$1"
  local reason_code="$2"
  local reason="$3"
  local details_json="$4"

  if ! bool_is_true "${SPV_FAIL_NOTIFY_ENABLE}"; then
    return 0
  fi

  local signature="${type}|${reason_code}|${reason}|${details_json}"
  if [[ "${LAST_FAILURE_SIGNATURE}" == "${signature}" ]]; then
    return 0
  fi
  LAST_FAILURE_SIGNATURE="${signature}"

  python3 - "${SPV_FAIL_NOTIFY_HOST}" "${SPV_FAIL_NOTIFY_PORT}" "${SPV_FAIL_NOTIFY_SERVICE_NAME}" \
    "${SPV_RUN_TARGET}" "${type}" "${reason_code}" "${reason}" "${details_json}" <<'PY'
import json
import socket
import sys
import time

host, port, service, target, msg_type, reason_code, reason, details = sys.argv[1:]
payload = {
    "type": msg_type,
    "service": service,
    "target": target,
    "reason_code": reason_code,
    "reason": reason,
    "timestamp_ns": time.time_ns(),
    "details": {},
}
try:
    payload["details"] = json.loads(details) if details else {}
except json.JSONDecodeError:
    payload["details"] = {"raw": details}

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
try:
    sock.sendto(json.dumps(payload, ensure_ascii=False).encode("utf-8"), (host, int(port)))
finally:
    sock.close()
PY
}

clear_failure_state() {
  LAST_FAILURE_SIGNATURE=""
}

report_failure() {
  local reason_code="$1"
  local reason="$2"
  local details_json="$3"
  error "${reason_code}: ${reason}"
  notify_udp_json "startup_failed" "${reason_code}" "${reason}" "${details_json}"
}

check_image() {
  if docker image inspect "${IMAGE_NAME}:${TAG}" >/dev/null 2>&1; then
    return 0
  fi
  report_failure "image_missing" "docker image not found: ${IMAGE_NAME}:${TAG}" \
    "{\"image\": $(json_escape "${IMAGE_NAME}:${TAG}")}"
  return 1
}

check_target() {
  local target_path
  if target_path="$(resolve_target_host_path)"; then
    return 0
  fi
  report_failure "binary_not_found" "target not found or not executable: ${SPV_RUN_TARGET}" \
    "{\"target\": $(json_escape "${SPV_RUN_TARGET}"), \"build_dir\": $(json_escape "${SPV_BUILD_DIR}")}"
  return 1
}

check_ros_setup() {
  local ros_setup="${ROOT_DIR}/ros2_ws/install/setup.bash"
  if [[ -f "${ros_setup}" ]]; then
    return 0
  fi
  report_failure "ros_setup_missing" "missing ROS workspace setup: ${ros_setup}" \
    "{\"ros_setup\": $(json_escape "${ros_setup}")}"
  return 1
}

check_ports() {
  local port=""
  local port_list=()

  if [[ -n "${SPV_FOXGLOVE_PORT}" ]]; then
    port_list+=("${SPV_FOXGLOVE_PORT}")
  fi
  if bool_is_true "${SPV_WS_ENABLE}" && [[ -n "${SPV_WS_PORT}" ]]; then
    port_list+=("${SPV_WS_PORT}")
  fi

  for port in "${port_list[@]}"; do
    local self_containers
    self_containers="$(docker ps --filter "name=^/${CONTAINER_NAME}$" --format '{{.ID}}' 2>/dev/null || true)"
    local conflicting
    conflicting="$(docker ps --filter "publish=${port}" --format '{{.Names}}' 2>/dev/null | grep -vx "${CONTAINER_NAME}" || true)"
    if [[ -n "${conflicting}" ]]; then
      report_failure "port_conflict" "docker container already publishes port ${port}" \
        "{\"port\": ${port}, \"containers\": $(python3 -c 'import json,sys; print(json.dumps(sys.argv[1].splitlines()))' "${conflicting}")}"
      return 1
    fi
    if command -v fuser >/dev/null 2>&1 && fuser -n tcp "${port}" >/dev/null 2>&1; then
      report_failure "port_conflict" "host TCP port is already in use: ${port}" \
        "{\"port\": ${port}}"
      return 1
    fi
  done

  return 0
}

check_camera() {
  if ! bool_is_true "${SPV_REQUIRED_CAMERA}"; then
    return 0
  fi

  local config_path
  config_path="$(detect_config_path)"
  if [[ -z "${config_path}" ]]; then
    warn "config path not found in SPV_RUN_ARGS; skipping camera VID/PID specific detection"
    if [[ -d /dev/bus/usb ]] && find /dev/bus/usb -type c | grep -q .; then
      return 0
    fi
    report_failure "camera_not_found" "camera is required but no USB camera bus device is visible" "{}"
    return 1
  fi
  if [[ "${config_path}" != /* ]]; then
    config_path="${ROOT_DIR}/${config_path}"
  fi
  if [[ ! -f "${config_path}" ]]; then
    report_failure "camera_not_found" "config file not found for camera detection: ${config_path}" \
      "{\"config_path\": $(json_escape "${config_path}")}"
    return 1
  fi

  local camera_name=""
  local vid_pid=""
  camera_name="$(yaml_value "${config_path}" "camera_name" || true)"
  vid_pid="$(yaml_value "${config_path}" "vid_pid" || true)"

  if [[ -n "${vid_pid}" ]] && command -v lsusb >/dev/null 2>&1; then
    if lsusb -d "${vid_pid}" >/dev/null 2>&1; then
      return 0
    fi
    report_failure "camera_not_found" "required camera VID:PID not detected on host USB bus" \
      "{\"camera_name\": $(json_escape "${camera_name}"), \"vid_pid\": $(json_escape "${vid_pid}")}"
    return 1
  fi

  if [[ -d /dev/bus/usb ]] && find /dev/bus/usb -type c | grep -q .; then
    return 0
  fi

  report_failure "camera_not_found" "camera is required but no visible USB device matches current config" \
    "{\"camera_name\": $(json_escape "${camera_name}"), \"vid_pid\": $(json_escape "${vid_pid}")}"
  return 1
}

select_serial() {
  if ! bool_is_true "${SPV_REQUIRED_SERIAL}"; then
    CURRENT_SERIAL_HOST=""
    return 0
  fi

  local old_ifs="${IFS}"
  local item=""
  IFS=','
  for item in ${SPV_SERIAL_CANDIDATES}; do
    item="$(trim "${item}")"
    if [[ -n "${item}" && -c "${item}" ]]; then
      CURRENT_SERIAL_HOST="${item}"
      IFS="${old_ifs}"
      return 0
    fi
  done
  IFS="${old_ifs}"

  report_failure "serial_not_found" "no serial device matched candidates" \
    "{\"serial_candidates\": $(python3 -c 'import json,sys; print(json.dumps([x.strip() for x in sys.argv[1].split(",") if x.strip()]))' "${SPV_SERIAL_CANDIDATES}")}"
  return 1
}

cleanup_container() {
  docker rm -f "${CONTAINER_NAME}" >/dev/null 2>&1 || true
}

wait_for_dependencies() {
  local started_at
  started_at="$(date +%s)"

  while true; do
    if check_image && check_target && check_ros_setup && check_ports && check_camera && select_serial; then
      clear_failure_state
      return 0
    fi

    if ! bool_is_true "${SPV_SUPERVISE}"; then
      return 1
    fi

    if [[ "${SPV_STARTUP_TIMEOUT_SEC}" != "0" ]]; then
      local now
      now="$(date +%s)"
      if (( now - started_at >= SPV_STARTUP_TIMEOUT_SEC )); then
        report_failure "dependency_timeout" "startup timeout while waiting for dependencies" \
          "{\"timeout_sec\": ${SPV_STARTUP_TIMEOUT_SEC}}"
        return 1
      fi
    fi

    sleep 2
  done
}

start_container() {
  local runtime_dm_endpoint=""
  local docker_args=()
  local runtime_cmd=""

  if [[ -n "${CURRENT_SERIAL_HOST}" ]]; then
    runtime_dm_endpoint="serial:${GIMBAL_DEV_CONT}?baud=${SPV_SERIAL_BAUD}"
  fi

  mkdir -p "${DATA_DIR}" "${SPV_LOG_DIR}"
  cleanup_container

  docker_args+=(run -d --name "${CONTAINER_NAME}")
  if bool_is_true "${USE_HOST_NET}"; then
    docker_args+=(--network=host)
  else
    local port
    for port in ${FORWARD_PORTS}; do
      docker_args+=(-p "${port}:${port}")
    done
    if [[ " ${FORWARD_PORTS} " != *" ${SPV_FOXGLOVE_PORT} "* ]]; then
      docker_args+=(-p "${SPV_FOXGLOVE_PORT}:${SPV_FOXGLOVE_PORT}")
    fi
    if bool_is_true "${SPV_WS_ENABLE}" && [[ " ${FORWARD_PORTS} " != *" ${SPV_WS_PORT} "* ]]; then
      docker_args+=(-p "${SPV_WS_PORT}:${SPV_WS_PORT}")
    fi
  fi
  if [[ -n "${DOCKER_PRIVILEGED_FLAGS}" ]]; then
    docker_args+=(${DOCKER_PRIVILEGED_FLAGS})
  fi
  docker_args+=(-v /dev/bus/usb:/dev/bus/usb)
  if [[ "${ENABLE_GPU}" == "1" ]]; then
    docker_args+=(-v /dev/dri:/dev/dri)
  fi
  if [[ -n "${CURRENT_SERIAL_HOST}" ]]; then
    docker_args+=(--device="${CURRENT_SERIAL_HOST}:${GIMBAL_DEV_CONT}")
  fi
  docker_args+=(
    -e "SPV_FOXGLOVE_PORT=${SPV_FOXGLOVE_PORT}"
    -e "SPV_WS_ENABLE=${SPV_WS_ENABLE}"
    -e "SPV_WS_PORT=${SPV_WS_PORT}"
    -e "SPV_WS_BIND_HOST=${SPV_WS_BIND_HOST}"
    -e "SPV_RUN_TARGET=${SPV_RUN_TARGET}"
    -e "SPV_RUN_ARGS=${SPV_RUN_ARGS}"
    -e "SPV_BUILD_DIR=${SPV_BUILD_DIR}"
    -e "SPV_RUNTIME_DM_ENDPOINT=${runtime_dm_endpoint}"
    -v "${ROOT_DIR}:/app/sp_vision"
    -v "${DATA_DIR}:/app/data"
    -w /app/sp_vision
    --entrypoint /bin/bash
    "${IMAGE_NAME}:${TAG}"
  )

  runtime_cmd='set -eo pipefail;
    source /opt/ros/jazzy/setup.bash;
    if [ -f /app/sp_vision/ros2_ws/install/setup.bash ]; then
      source /app/sp_vision/ros2_ws/install/setup.bash;
    else
      echo "missing ROS workspace setup: /app/sp_vision/ros2_ws/install/setup.bash" >&2;
      exit 120;
    fi;
    cd /app/sp_vision;
    t="${SPV_RUN_TARGET}";
    target_path="";
    if [ -x "${t}" ]; then target_path="${t}"; fi;
    if [ -z "${target_path}" ] && [ -x "./${t}" ]; then target_path="./${t}"; fi;
    if [ -z "${target_path}" ] && [ -x "${SPV_BUILD_DIR}/${t}" ]; then target_path="${SPV_BUILD_DIR}/${t}"; fi;
    if [ -z "${target_path}" ] && [ -x "${SPV_BUILD_DIR}/bin/${t}" ]; then target_path="${SPV_BUILD_DIR}/bin/${t}"; fi;
    if [ -z "${target_path}" ]; then
      echo "target not found: ${t}" >&2;
      exit 127;
    fi;
    cmd="exec \"${target_path}\" ${SPV_RUN_ARGS}";
    if [ -n "${SPV_RUNTIME_DM_ENDPOINT}" ] && [[ " ${SPV_RUN_ARGS} " != *" --dm-endpoint"* ]]; then
      cmd="${cmd} --dm-endpoint=${SPV_RUNTIME_DM_ENDPOINT}";
    fi;
    eval "${cmd}"'

  if ! docker "${docker_args[@]}" -lc "${runtime_cmd}" >/dev/null; then
    report_failure "container_start_failed" "docker failed to start the supervised container" \
      "{\"container\": $(json_escape "${CONTAINER_NAME}")}"
    return 1
  fi

  info "started container ${CONTAINER_NAME}"
  return 0
}

run_supervisor_loop() {
  if ! bool_is_true "${SPV_AUTOSTART}"; then
    info "SPV_AUTOSTART=0, supervisor stays idle without launching target"
    while true; do
      sleep 3600
    done
  fi

  while true; do
    cleanup_container

    if ! wait_for_dependencies; then
      if bool_is_true "${SPV_SUPERVISE}"; then
        sleep 2
        continue
      fi
      return 1
    fi

    if ! start_container; then
      if bool_is_true "${SPV_SUPERVISE}"; then
        sleep 2
        continue
      fi
      return 1
    fi

    local started_at
    started_at="$(date +%s)"
    local exit_code
    exit_code="$(docker wait "${CONTAINER_NAME}" 2>/dev/null || printf '125')"
    local finished_at
    finished_at="$(date +%s)"
    local runtime_sec=$((finished_at - started_at))
    info "container ${CONTAINER_NAME} exited with code ${exit_code} after ${runtime_sec}s"

    if [[ "${exit_code}" != "0" && "${runtime_sec}" -lt 5 ]]; then
      case "${exit_code}" in
        120)
          report_failure "ros_setup_missing" "container exited during startup because ROS setup is missing" \
            "{\"container\": $(json_escape "${CONTAINER_NAME}"), \"exit_code\": ${exit_code}}"
          ;;
        127)
          report_failure "binary_not_found" "container exited during startup because target was not found" \
            "{\"container\": $(json_escape "${CONTAINER_NAME}"), \"target\": $(json_escape "${SPV_RUN_TARGET}"), \"exit_code\": ${exit_code}}"
          ;;
        *)
          report_failure "container_start_failed" "container exited during startup before becoming stable" \
            "{\"container\": $(json_escape "${CONTAINER_NAME}"), \"exit_code\": ${exit_code}, \"runtime_sec\": ${runtime_sec}}"
          ;;
      esac
    fi

    cleanup_container

    if ! bool_is_true "${SPV_SUPERVISE}"; then
      return "${exit_code}"
    fi

    sleep "$(python3 -c 'import sys; print(max(float(sys.argv[1]) / 1000.0, 0.0))' "${SPV_RESTART_DELAY_MS}")"
  done
}

trap 'cleanup_container' EXIT

run_supervisor_loop
