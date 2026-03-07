#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="${1:-asio-1-31-0}"
TARGET_DIR="${ROOT_DIR}/3rdparty/asio/asio/include"
TMP_DIR="$(mktemp -d)"
ARCHIVE="${TMP_DIR}/asio.tar.gz"
URL="https://github.com/chriskohlhoff/asio/archive/refs/tags/${VERSION}.tar.gz"

if ! command -v curl >/dev/null 2>&1; then
  echo "[asio] curl not found" >&2
  exit 1
fi
if ! command -v tar >/dev/null 2>&1; then
  echo "[asio] tar not found" >&2
  exit 1
fi

echo "[asio] downloading ${URL}"
curl -fsSL "${URL}" -o "${ARCHIVE}"
tar -xzf "${ARCHIVE}" -C "${TMP_DIR}"

SRC_INCLUDE="$(find "${TMP_DIR}" -maxdepth 3 -type d -path '*/asio/include' | head -n1)"
if [ -z "${SRC_INCLUDE}" ] || [ ! -d "${SRC_INCLUDE}/asio" ] || [ ! -f "${SRC_INCLUDE}/asio.hpp" ]; then
  echo "[asio] asio include files not found in archive ${VERSION}" >&2
  exit 1
fi

mkdir -p "${TARGET_DIR}"
rsync -a --delete "${SRC_INCLUDE}/" "${TARGET_DIR}/"

echo "[asio] headers installed: ${TARGET_DIR}"
