#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SERVICE_NAME="${SPV_SYSTEMD_SERVICE_NAME:-spv-supervisor.service}"
UNIT_TEMPLATE="${ROOT_DIR}/systemd/spv-supervisor.service.template"
ENV_TEMPLATE="${ROOT_DIR}/systemd/spv-supervisor.env.example"
UNIT_TARGET="/etc/systemd/system/${SERVICE_NAME}"
ENV_TARGET="/etc/default/spv-supervisor"

if [[ ! -f "${UNIT_TEMPLATE}" ]]; then
  echo "missing unit template: ${UNIT_TEMPLATE}" >&2
  exit 1
fi

if [[ "${EUID}" -ne 0 ]]; then
  if command -v sudo >/dev/null 2>&1; then
    exec sudo env SPV_SYSTEMD_SERVICE_NAME="${SERVICE_NAME}" "$0" "$@"
  fi
  echo "please run as root (or install sudo)" >&2
  exit 1
fi

install -d /etc/systemd/system /etc/default
sed "s|__SPV_ROOT_DIR__|${ROOT_DIR}|g" "${UNIT_TEMPLATE}" > "${UNIT_TARGET}"
if [[ ! -f "${ENV_TARGET}" ]]; then
  install -m 0644 "${ENV_TEMPLATE}" "${ENV_TARGET}"
fi

systemctl daemon-reload
systemctl enable --now "${SERVICE_NAME}"

echo "installed ${SERVICE_NAME}"
echo "unit file : ${UNIT_TARGET}"
echo "env file  : ${ENV_TARGET}"
echo "status    : systemctl status ${SERVICE_NAME}"
