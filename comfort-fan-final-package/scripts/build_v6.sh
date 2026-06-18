#!/usr/bin/env bash
set -euo pipefail
export PATH="$HOME/.local/bin:$HOME/tools/gcc_riscv32/bin:$PATH"
ROOT="${ROOT:-/home/ohos/workspace/openharmony-3.2-release}"
cd "$ROOT"
hb build -f
OUT="$ROOT/out/qihang/qihang"
sha256sum "$OUT/Hi3861_wifiiot_app_allinone.bin"
