#!/usr/bin/env bash

ex () {
  local archive="$1"
  local archive_name=$(basename "$archive" | sed 's/\.[^.]*$//') # Extract name without extension

  if [ -f "$archive" ]; then
    case "$archive" in
      *.zip)
        # Check if the zip contains files in the root
        if unzip -l "$archive" | grep -q '^    '; then
          mkdir -p "$archive_name"
          unzip "$archive" -d "$archive_name"
        else
          unzip "$archive"
        fi
        ;;
      *.tar.bz2) tar xjf "$archive" ;;
      *.tar.gz) tar xzf "$archive" ;;
      *.bz2) bunzip2 "$archive" ;;
      *.rar) unrar x "$archive" ;;
      *.gz) gunzip "$archive" ;;
      *.tar) tar xf "$archive" ;;
      *.tbz2) tar xjf "$archive" ;;
      *.tgz) tar xzf "$archive" ;;
      *.Z) uncompress "$archive" ;;
      *.7z) 7z x "$archive" ;;
      *.tar.xz) tar xf "$archive" ;;
      *.tar.zst) unzstd "$archive" ;;
      *) echo "'$archive' cannot be extracted via ex()" ;;
    esac
  else
    echo "'$archive' is not a valid file"
  fi
}

extract_recursive () {
  local item

  for item in "$1"/*; do
    if [ -f "$item" ]; then
      ex "$item"
    elif [ -d "$item" ]; then
      extract_recursive "$item"
    fi
  done
}

# Check if a directory argument is provided, if not, use the current directory.
if [ -z "$1" ]; then
  extract_recursive "."
else
  extract_recursive "$1"
fi