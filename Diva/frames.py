#!/usr/bin/env python3

from concurrent.futures import ProcessPoolExecutor
from pathlib import Path
import subprocess
import sys
import shlex

DEFAULT_MAX_WORKERS = 2
FAST_MAX_WORKERS = 4
USE_NICE_IONICE = True

VIDEO_EXTS = {".mp4", ".webm", ".m4v", ".mov", ".mkv"}

def _wrap_with_nice_ionice(cmd_list, use_wrappers=True):

    if not use_wrappers:
        return cmd_list
    return ["nice", "-n", "10", "ionice", "-c2", "-n7"] + cmd_list

def run_ffmpeg(cmd_list):
    try:
        proc = subprocess.run(cmd_list, check=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        return proc.returncode, proc.stdout.decode(errors="replace"), proc.stderr.decode(errors="replace")
    except FileNotFoundError:
        return 127, "", "ffmpeg not found (FileNotFoundError). Is ffmpeg installed?"

    name = video_path.stem
    target_folder = output_base / name
    target_folder.mkdir(parents=True, exist_ok=True)
    out_pattern = str(target_folder / "%04d.jpg")

    gpu_cmd = [
        "ffmpeg",
        "-hide_banner",
        "-loglevel", "error",
        "-hwaccel", "vaapi",
        "-vaapi_device", "/dev/dri/renderD128",
        "-i", str(video_path),
        "-vf", "hwdownload,format=bgr24",
        "-q:v", "3",
        out_pattern
    ]
    full_gpu_cmd = _wrap_with_nice_ionice(gpu_cmd, use_wrappers=nice_ionice)
    rc, out, err = run_ffmpeg(full_gpu_cmd)
    if rc == 0:
        print(f"[GPU ✓] {video_path.name}")
        return True, f"GPU ok: {video_path.name}"
    else:
        err_snip = "\n".join(err.splitlines()[:20])
        print(f"[GPU ✗] {video_path.name}  — falling back to CPU.")
        print("ffmpeg GPU stderr (first lines):")
        print(err_snip or "<no stderr>")

    cpu_cmd = [
        "ffmpeg",
        "-hide_banner",
        "-loglevel", "error",
        "-i", str(video_path),
        "-q:v", "3",
        out_pattern
    ]
    full_cpu_cmd = _wrap_with_nice_ionice(cpu_cmd, use_wrappers=nice_ionice)
    rc2, out2, err2 = run_ffmpeg(full_cpu_cmd)
    if rc2 == 0:
        print(f"[CPU ✓] {video_path.name}")
        return True, f"CPU ok: {video_path.name}"
    else:
        err2_snip = "\n".join(err2.splitlines()[:30])
        print(f"[CPU ✗] {video_path.name}  — BOTH GPU and CPU attempts failed.")
        print("ffmpeg CPU stderr (first lines):")
        print(err2_snip or "<no stderr>")
        return False, f"Both failed: {video_path.name}"

def main():
    fast_mode = len(sys.argv) > 1 and sys.argv[1].lower() == "fast"
    max_workers = FAST_MAX_WORKERS if fast_mode else DEFAULT_MAX_WORKERS
    nice_ionice = (not fast_mode) and USE_NICE_IONICE

    cwd = Path.cwd()
    output_dir = cwd / "output_frames"
    output_dir.mkdir(exist_ok=True)

    videos = [p for p in cwd.iterdir() if p.suffix.lower() in VIDEO_EXTS and p.is_file()]
    if not videos:
        print("No videos found in", cwd)
        return

    print(f"Found {len(videos)} videos. Mode: {'fast' if fast_mode else 'gentle'}  workers={max_workers}")
    with ProcessPoolExecutor(max_workers=max_workers) as pool:
        futures = []
        for v in videos:
            futures.append(pool.submit(extract_frames_for_video, v, output_dir, True, nice_ionice))
        success_count = 0
        for f in futures:
            ok, msg = f.result()
            if ok:
                success_count += 1
        print(f"Done. Success: {success_count}/{len(videos)}")

if __name__ == "__main__":
    main()