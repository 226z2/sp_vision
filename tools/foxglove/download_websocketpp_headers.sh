#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="${1:-0.8.2}"
TARGET_DIR="${ROOT_DIR}/3rdparty/websocketpp"
TMP_DIR="$(mktemp -d)"
ARCHIVE="${TMP_DIR}/websocketpp.tar.gz"
URL="https://github.com/zaphoyd/websocketpp/archive/refs/tags/${VERSION}.tar.gz"

if ! command -v curl >/dev/null 2>&1; then
  echo "[websocketpp] curl not found" >&2
  exit 1
fi
if ! command -v tar >/dev/null 2>&1; then
  echo "[websocketpp] tar not found" >&2
  exit 1
fi

echo "[websocketpp] downloading ${URL}"
curl -fsSL "${URL}" -o "${ARCHIVE}"
tar -xzf "${ARCHIVE}" -C "${TMP_DIR}"

SRC_DIR="$(find "${TMP_DIR}" -maxdepth 1 -type d -name 'websocketpp-*' | head -n1)"
if [ -z "${SRC_DIR}" ] || [ ! -f "${SRC_DIR}/websocketpp/config/asio_no_tls.hpp" ]; then
  echo "[websocketpp] invalid archive layout for ${VERSION}" >&2
  exit 1
fi

mkdir -p "${TARGET_DIR}"
rsync -a --delete "${SRC_DIR}/" "${TARGET_DIR}/"

echo "[websocketpp] headers installed: ${TARGET_DIR}"
