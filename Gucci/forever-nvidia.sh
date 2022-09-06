#!/usr/bin/env bash

for file in *.zst
do
cp "$file" "$file.bak"
unzstd -f "$file"
base=$(basename "$file" .zst)
objcopy --remove-section .BTF "$base"
zstd -f "$base"
rm -f "$base"
done
