#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -lt 3 ]; then
  echo "Usage: $0 <flatc_path> <schema_dir> <out_dir>" >&2
  exit 1
fi

FLATC_PATH="$1"
SCHEMA_DIR="$2"
OUT_DIR="$3"

if [ ! -x "${FLATC_PATH}" ]; then
  echo "[foxglove] flatc not executable: ${FLATC_PATH}" >&2
  exit 1
fi
if [ ! -d "${SCHEMA_DIR}" ]; then
  echo "[foxglove] schema dir not found: ${SCHEMA_DIR}" >&2
  exit 1
fi

mkdir -p "${OUT_DIR}"

mapfile -t SCHEMAS < <(find "${SCHEMA_DIR}" -maxdepth 1 -name '*.fbs' -type f | sort)
if [ "${#SCHEMAS[@]}" -eq 0 ]; then
  echo "[foxglove] no .fbs files found in ${SCHEMA_DIR}" >&2
  exit 1
fi

echo "[foxglove] generating C++ headers into ${OUT_DIR}"
"${FLATC_PATH}" --cpp -I "${SCHEMA_DIR}" -o "${OUT_DIR}" "${SCHEMAS[@]}"

echo "[foxglove] generating .bfbs schemas into ${OUT_DIR}"
for schema in "${SCHEMAS[@]}"; do
  "${FLATC_PATH}" --schema --bfbs-comments --bfbs-builtins -b -I "${SCHEMA_DIR}" -o "${OUT_DIR}" "${schema}"
done

echo "[foxglove] done"
