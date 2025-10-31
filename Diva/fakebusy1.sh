#!/usr/bin/env bash


set -euo pipefail

# ---- Configuration ----
DURATION_SECONDS=300
BAR_WIDTH=30
LOG_CHANCE=30
THEME_CHANCE=10
FILE_OP_CHANCE=8
MIN_SLEEP=0.12
MAX_SLEEP=0.65
# ------------------------

# --- Colors ---
C_RESET='\033[0m'
C_DIM='\033[2m'
C_INFO='\033[38;5;118m'
C_WARN='\033[38;5;214m'
C_ERROR='\033[38;5;196m'
C_TIME='\033[38;5;246m'
C_ACCENT='\033[38;5;81m'

# Word banks
VERBS=(initializing synchronizing processing indexing validating compressing optimizing calibrating scanning)
OBJECTS=(dataset shard index frame segment registry cache pipeline metadata manifest)
ACTIONS=(applied queued retained purged compacted checkpointed)
THEME_THEATRIC=("starting neural scheduler" "aligning tensors" "warming GPU kernels" "finalizing artifact bundle" "publishing metrics")
THEME_SUBTLE=("heartbeat check" "light compaction" "minor retention pass" "background maintenance" "index delta applied")
FAKE_PATHS=(
  "/var/log/syslog"
  "/var/log/kern.log"
  "/var/log/auth.log"
  "/var/log/nginx/access.log"
  "/var/log/nginx/error.log"
  "/var/lib/service/state.db"
  "/data/ingest/shard-0123.seg"
  "/mnt/storage/snapshots/snap-2025-10-30.tar"
)

SPIN='|/-\\'
SP=0

cleanup() {
  printf '\n%s\n' "$C_RESET"
  tput cnorm 2>/dev/null || true
}
trap cleanup EXIT INT TERM
tput civis 2>/dev/null || true

now_ts() { date '+%Y-%m-%d %H:%M:%S'; }

emit_log() {
  local t verb obj suf
  t="$(now_ts)"
  verb="${VERBS[RANDOM % ${#VERBS[@]}]}"
  obj="${OBJECTS[RANDOM % ${#OBJECTS[@]}]}"
  suf="${ACTIONS[RANDOM % ${#ACTIONS[@]}]}"
  case $((RANDOM % 4)) in
    0) printf '%s %b[INFO]%b %b%s %s%b\n' "$t" "$C_TIME" "$C_RESET" "$C_ACCENT" "$verb" "$obj $suf" "$C_RESET" ;;
    1) printf '%s %b[DEBUG]%b %b%s %s%b\n' "$t" "$C_TIME" "$C_RESET" "$C_INFO" "$verb" "$obj: ok" "$C_RESET" ;;
    2) printf '%s %b[WARN]%b %b%s %s%b\n' "$t" "$C_TIME" "$C_RESET" "$C_WARN" "$verb" "$obj: noisy but recovered" "$C_RESET" ;;
    3) printf '%s %b[TRACE]%b %b%s %s%b\n' "$t" "$C_TIME" "$C_RESET" "$C_DIM" "$verb" "$obj delta=$((RANDOM%4096))" "$C_RESET" ;;
  esac
}

emit_theme() {
  local t phrase
  t="$(now_ts)"
  if (( RANDOM % 2 == 0 )); then
    phrase="${THEME_THEATRIC[RANDOM % ${#THEME_THEATRIC[@]}]}"
    printf '%s %b[DEBUG]%b %b%s%b\n' "$t" "$C_TIME" "$C_RESET" "$C_INFO" "$phrase" "$C_RESET"
  else
    phrase="${THEME_SUBTLE[RANDOM % ${#THEME_SUBTLE[@]}]}"
    printf '%s %b[TRACE]%b %b%s%b\n' "$t" "$C_TIME" "$C_RESET" "$C_DIM" "$phrase" "$C_RESET"
  fi
}

emit_fileop() {
  local t p op
  t="$(now_ts)"
  p="${FAKE_PATHS[RANDOM % ${#FAKE_PATHS[@]}]}"
  case $((RANDOM % 3)) in
    0) op="open" ;;
    1) op="stat" ;;
    2) op="read-meta" ;;
  esac
  printf '%s %b[IO]%b %b%s %s%b\n' "$t" "$C_TIME" "$C_RESET" "$C_DIM" "$op" "$p" "$C_RESET"
}

draw_bar() {
  local percent=$1
  local width=$BAR_WIDTH
  (( percent > 100 )) && percent=100
  local filled=$(( percent * width / 100 ))
  local empty=$(( width - filled ))
  local bar
  bar="$(printf '%*s' "$filled" '' | tr ' ' '#')"
  bar="${bar}$(printf '%*s' "$empty" '' | tr ' ' '-')"
  printf '[%s] %3d%%' "$bar" "$percent"
}

sleep_jitter() {
  local rand=$(awk -v min="$MIN_SLEEP" -v max="$MAX_SLEEP" 'BEGIN{srand(); print min+rand()*(max-min)}')
  sleep "$rand"
}

main_loop() {
  local start_ts=$(date +%s)
  while true; do
    local now=$(date +%s)
    local elapsed=$(( now - start_ts ))
    local percent=$(( (elapsed * 100 / DURATION_SECONDS) % 101 ))

    SP=$(( (SP + 1) % 4 ))
    local spinch="${SPIN:SP:1}"

    # Now correctly evaluated with arithmetic context:
    (( RANDOM % 100 < LOG_CHANCE ))   && emit_log
    (( RANDOM % 100 < THEME_CHANCE )) && emit_theme
    (( RANDOM % 100 < FILE_OP_CHANCE )) && emit_fileop

    printf '\r%s %b%s%b %sElapsed: %ss' "$spinch" "$C_ACCENT" "$(draw_bar "$percent")" "$C_RESET" "$C_DIM" "$elapsed"
    sleep_jitter

    if (( percent >= 100 )); then
      printf '\n%s %b[INFO]%b checkpoint: stable state saved%b\n' "$(now_ts)" "$C_TIME" "$C_RESET" "$C_RESET"
      sleep 0.8
      start_ts=$(date +%s)
    fi
  done
}

main_loop