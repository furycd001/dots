#!/usr/bin/env bash

set -euo pipefail

# ---- Configuration ----
DURATION_SECONDS=300
BAR_WIDTH=40
LOG_CHANCE=25
THEME_CHANCE=8
FILE_OP_CHANCE=12
MIN_SLEEP=0.05
MAX_SLEEP=0.4
# ------------------------

# --- Terminal-Native Colors ---
C_RESET='\033[0m'
C_DIM='\033[90m'      # Dark Grey / Bright Black
C_INFO='\033[32m'     # Green
C_WARN='\033[33m'     # Yellow
C_ERROR='\033[31m'    # Red
C_TIME='\033[36m'     # Cyan
C_ACCENT='\033[34m'   # Blue
C_BOLD='\033[1m'

# Word banks
VERBS=(initializing synchronizing processing indexing validating compressing optimizing calibrating scanning routing)
OBJECTS=(dataset shard index frame segment registry cache pipeline metadata manifest cluster)
ACTIONS=(applied queued retained purged compacted checkpointed committed)
THEME_THEATRIC=("starting neural scheduler" "aligning tensors" "warming GPU kernels" "finalizing artifact bundle" "publishing metrics")
FAKE_PATHS=("/var/log/syslog" "/var/log/nginx/access.log" "/var/lib/service/state.db" "/etc/config/cluster.yaml" "/tmp/worker.pid")

SPIN='⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏'
SP=0

cleanup() {
  printf '\n%bDone.%b\n' "$C_INFO" "$C_RESET"
  tput cnorm 2>/dev/null || true
}
trap cleanup EXIT INT TERM
tput civis 2>/dev/null || true

now_ts() { date '+%H:%M:%S'; }

clear_line() {
  printf '\r\033[K'
}

emit_log() {
  clear_line
  local t verb obj suf
  t="$(now_ts)"
  verb="${VERBS[RANDOM % ${#VERBS[@]}]}"
  obj="${OBJECTS[RANDOM % ${#OBJECTS[@]}]}"
  suf="${ACTIONS[RANDOM % ${#ACTIONS[@]}]}"
  
  case $((RANDOM % 5)) in
    0) printf '%b%s %b[INFO]%b %b%s %s%b\n' "$C_DIM" "$t" "$C_INFO" "$C_RESET" "$C_ACCENT" "$verb" "$obj $suf" "$C_RESET" ;;
    1) printf '%b%s %b[DEBUG]%b %b%s %s%b\n' "$C_DIM" "$t" "$C_DIM" "$C_RESET" "$C_DIM" "$verb" "$obj: ok" "$C_RESET" ;;
    2) printf '%b%s %b[WARN]%b %b%s %s%b\n' "$C_DIM" "$t" "$C_WARN" "$C_RESET" "$C_WARN" "retrying $verb" "$obj..." "$C_RESET" ;;
    3) printf '%b%s %b[TRACE]%b %b%s %s%b\n' "$C_DIM" "$t" "$C_DIM" "$C_RESET" "$C_DIM" "$verb" "$obj delta=$((RANDOM%4096))" "$C_RESET" ;;
    4) printf '%b%s %b[ERROR]%b %bFailed to %s %s: timeout%b\n' "$C_DIM" "$t" "$C_ERROR" "$C_RESET" "$C_ERROR" "$verb" "$obj" "$C_RESET" ;;
  esac
}

emit_theme() {
  clear_line
  local t phrase
  t="$(now_ts)"
  phrase="${THEME_THEATRIC[RANDOM % ${#THEME_THEATRIC[@]}]}"
  printf '%b%s %b[SYSTEM]%b %b%s...%b\n' "$C_DIM" "$t" "$C_ACCENT" "$C_RESET" "$C_BOLD" "$phrase" "$C_RESET"
}

emit_fileop() {
  clear_line
  local t p op
  t="$(now_ts)"
  p="${FAKE_PATHS[RANDOM % ${#FAKE_PATHS[@]}]}"
  case $((RANDOM % 3)) in
    0) op="OPEN" ;; 1) op="STAT" ;; 2) op="READ" ;;
  esac
  printf '%b%s %b[IO]%b %b%s%b %s\n' "$C_DIM" "$t" "$C_DIM" "$C_RESET" "$C_DIM" "$op" "$C_RESET" "$p"
}

draw_bar() {
  local percent=$1
  local width=$BAR_WIDTH
  local filled=$(( percent * width / 100 ))
  local empty=$(( width - filled ))
  
  local bar_chars
  bar_chars=$(printf "%.0s#" $(seq 1 $filled))
  local empty_chars
  empty_chars=$(printf "%.0s-" $(seq 1 $empty))
  
  printf ' %b%s%b%s %3d%%' "$C_ACCENT" "$bar_chars" "$C_DIM" "$empty_chars" "$percent"
}

main_loop() {
  local start_ts=$(date +%s)
  while true; do
    local now=$(date +%s)
    local elapsed=$(( now - start_ts ))
    local percent=$(( (elapsed * 100 / DURATION_SECONDS) ))
    (( percent > 100 )) && percent=100

    SP=$(( (SP + 1) % 10 ))
    local spinch="${SPIN:SP:1}"

    (( RANDOM % 100 < LOG_CHANCE ))     && emit_log
    (( RANDOM % 100 < THEME_CHANCE ))    && emit_theme
    (( RANDOM % 100 < FILE_OP_CHANCE ))  && emit_fileop

    printf '\r%b%s%b %s %b[%ss]%b' "$C_INFO" "$spinch" "$C_RESET" "$(draw_bar "$percent")" "$C_DIM" "$elapsed" "$C_RESET"
    
    # Simple jitter
    sleep 0.$((RANDOM%4 + 1))

    if (( percent >= 100 )); then
      clear_line
      printf '%b%s %b[DONE]%b Cycle complete.\n' "$(now_ts)" "$C_DIM" "$C_INFO" "$C_RESET"
      sleep 1
      start_ts=$(date +%s)
    fi
  done
}

main_loop