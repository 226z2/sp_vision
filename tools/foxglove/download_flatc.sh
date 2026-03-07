#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="${1:-v24.3.25}"
ASSET="${2:-Linux.flatc.binary.g++-13.zip}"
TARGET_DIR="${ROOT_DIR}/tools/bin/flatc/linux-x86_64"
TARGET_BIN="${TARGET_DIR}/flatc"
TMP_DIR="$(mktemp -d)"
ARCHIVE="${TMP_DIR}/flatc.zip"

if ! command -v curl >/dev/null 2>&1; then
  echo "[flatc] curl not found" >&2
  exit 1
fi
if ! command -v unzip >/dev/null 2>&1; then
  echo "[flatc] unzip not found" >&2
  exit 1
fi

URL="https://github.com/google/flatbuffers/releases/download/${VERSION}/${ASSET}"
echo "[flatc] downloading ${URL}"
curl -fsSL "${URL}" -o "${ARCHIVE}"

mkdir -p "${TARGET_DIR}"
unzip -o "${ARCHIVE}" -d "${TMP_DIR}" >/dev/null

if [ ! -f "${TMP_DIR}/flatc" ]; then
  echo "[flatc] flatc binary not found in archive ${ASSET}" >&2
  exit 1
fi

cp "${TMP_DIR}/flatc" "${TARGET_BIN}"
chmod +x "${TARGET_BIN}"

echo "[flatc] installed: ${TARGET_BIN}"
echo "[flatc] version: $("${TARGET_BIN}" --version)"
