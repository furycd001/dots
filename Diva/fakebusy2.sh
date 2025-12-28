#!/usr/bin/env bash

# ---- Configuration ----
TITLE=" MABEL CORE SUBSYSTEM "
TOTAL_SECONDS=600  
# ------------------------

cleanup() {
    clear
    tput cnorm
    exit 0
}

trap cleanup EXIT INT TERM

generate_git_data() {
    local STAGES=(
        "Scanning inode metadata for repository drift..."
        "Calculating delta-chain integrity checksums..."
        "Pruning unreachable loose objects..."
        "Repacking pack-backend with O(n) compression..."
        "Synchronizing remote reflog with local HEAD..."
        "Validating workspace tree consistency..."
    )

    local step_sleep=6

    for ((i=0; i<=100; i++)); do
        local stage_idx=$(( i / 17 )) 
        [ $stage_idx -gt 5 ] && stage_idx=5
        local msg="${STAGES[$stage_idx]}"

        echo "$i"
        echo "XXX"
        echo -e "Subsystem: git-mabel-core\nOperation: $msg"
        echo "XXX"
        
        sleep "$step_sleep"
    done
}

tput civis

# Using --no-lines to remove the frame entirely.
# This gives a clean, centered box without drawing characters.
generate_git_data | dialog --backtitle "Arch Linux - System Maintenance" \
           --title "$TITLE" \
           --no-lines \
           --nocancel \
           --gauge "\nInitializing hardware-level handshake..." 8 75 0

sleep 1
cleanup