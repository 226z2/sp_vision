#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="${1:-v24.3.25}"
TARGET_DIR="${ROOT_DIR}/3rdparty/flatbuffers/include"
TMP_DIR="$(mktemp -d)"
ARCHIVE="${TMP_DIR}/flatbuffers.tar.gz"
URL="https://github.com/google/flatbuffers/archive/refs/tags/${VERSION}.tar.gz"

if ! command -v curl >/dev/null 2>&1; then
  echo "[flatbuffers] curl not found" >&2
  exit 1
fi
if ! command -v tar >/dev/null 2>&1; then
  echo "[flatbuffers] tar not found" >&2
  exit 1
fi

echo "[flatbuffers] downloading ${URL}"
curl -fsSL "${URL}" -o "${ARCHIVE}"
tar -xzf "${ARCHIVE}" -C "${TMP_DIR}"

SRC_INCLUDE="$(find "${TMP_DIR}" -maxdepth 2 -type d -name include | head -n1)"
if [ -z "${SRC_INCLUDE}" ] || [ ! -d "${SRC_INCLUDE}/flatbuffers" ]; then
  echo "[flatbuffers] include/flatbuffers not found in archive ${VERSION}" >&2
  exit 1
fi

mkdir -p "${TARGET_DIR}"
rsync -a --delete "${SRC_INCLUDE}/flatbuffers/" "${TARGET_DIR}/flatbuffers/"

echo "[flatbuffers] headers installed: ${TARGET_DIR}/flatbuffers"
