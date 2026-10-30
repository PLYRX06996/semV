#!/usr/bin/env python3
"""
Automated OpenGL Graphics Output Capture Utility
================================================
Discovers C++ OpenGL assignments across lab directories, runs each program
headlessly in a virtual X11 display (Xvfb), captures the rendered OpenGL window
as a high-resolution PNG image, and saves the image directly into the
'output snapshots' sub-directory of each respective lab.

Usage:
    python3 capture_outputs.py                  # Capture all labs
    python3 capture_outputs.py "LAB 11"         # Capture specific lab
    python3 capture_outputs.py --clean          # Remove generated PNG outputs
"""

import os
import sys
import glob
import time
import signal
import socket
import subprocess
from pathlib import Path

KNOWN_BINARY_MAP = {
    "LAB 01": {
        "main.cpp": "triangle",
    },
    "LAB 03": {
        "demoAssign01.cpp": "demo",
    },
    "LAB 07": {
        "assignment1.cpp": "a1",
        "assignment2.cpp": "a2",
        "assignment3.cpp": "a3",
        "assignment4.cpp": "a4",
        "assignment5.cpp": "a5",
    },
    "LAB 08": {
        "assignment01.cpp": "A01",
        "assignment02.cpp": "A02",
        "assignment03.cpp": "A03",
        "assignment04.cpp": "A04",
        "assignment05.cpp": "A05",
    },
    "LAB 09": {
        "assignment01.cpp": "a1",
        "lab9_assignment2_transformations.cpp": "a2",
        "lab9_assignment3_shearing.cpp": "a3",
        "lab9_assignment4_reflection.cpp": "a4",
        "lab9_assignment5_walle.cpp": "a5",
    },
    "LAB 10": {
        "lab10_assignment1_demo.cpp": "l10a1",
        "lab10_assignment2_flat_shading.cpp": "l10a2",
        "lab10_assignment3_gouraud_shading.cpp": "l10a3",
        "lab10_assignment4_shading_comparison.cpp": "l10a4",
        "lab10_assignment5_walle_lighting.cpp": "l10a5",
    },
    "LAB 11": {
        "assignment01_demo_traffic_signal.cpp": "a1",
    },
}

def find_free_display():
    """Finds an unused X11 display number."""
    for port in range(99, 150):
        lock_file = f"/tmp/.X11-unix/X{port}"
        if not os.path.exists(lock_file):
            return f":{port}"
    return ":99"

def start_xvfb(display):
    """Starts an Xvfb process with a high-resolution virtual screen."""
    cmd = ["Xvfb", display, "-screen", "0", "1920x1080x24", "-ac"]
    proc = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(1.0)
    return proc

def get_child_window_id(env):
    """Queries xwininfo to locate the client window ID under root."""
    try:
        res = subprocess.run(["xwininfo", "-root", "-tree"], env=env, capture_output=True, text=True)
        for line in res.stdout.splitlines():
            line = line.strip()
            if line.startswith("0x") and ":" in line:
                return line.split()[0]
    except Exception:
        pass
    return None

def wait_for_no_window(env, timeout=2.0):
    """Waits until child windows on the display are destroyed."""
    start = time.time()
    while time.time() - start < timeout:
        if not get_child_window_id(env):
            return True
        time.sleep(0.05)
    return False

def compile_if_needed(cpp_path, bin_path):
    """Ensures binary is built and up to date with respect to the source."""
    if not bin_path.exists() or bin_path.stat().st_mtime < cpp_path.stat().st_mtime:
        bin_path.parent.mkdir(parents=True, exist_ok=True)
        cmd = ["g++", "-O2", str(cpp_path), "-lGL", "-lGLU", "-lglut", "-lm", "-o", str(bin_path)]
        res = subprocess.run(cmd, capture_output=True, text=True)
        if res.returncode != 0:
            return False, res.stderr
    return True, ""

def capture_program(lab_dir, cpp_file, bin_name, env):
    """Runs a program and captures its OpenGL frame."""
    lab_path = Path(lab_dir)

    code_dir = lab_path / "c++ codes" if (lab_path / "c++ codes").is_dir() else lab_path
    bin_dir = lab_path / "output files" if (lab_path / "output files").is_dir() else lab_path
    snap_dir = lab_path / "output snapshots" if (lab_path / "output snapshots").is_dir() else lab_path
    snap_dir.mkdir(parents=True, exist_ok=True)
    bin_dir.mkdir(parents=True, exist_ok=True)

    cpp_path = code_dir / cpp_file
    bin_path = bin_dir / bin_name

    ok, err = compile_if_needed(cpp_path, bin_path)
    if not ok:
        return False, f"Compilation error: {err.strip()}", None

    proc = subprocess.Popen([f"./{bin_name}"], cwd=str(bin_dir), env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    child_id = None
    start = time.time()
    while time.time() - start < 4.0:
        child_id = get_child_window_id(env)
        if child_id:
            break
        time.sleep(0.05)

    if not child_id:
        proc.kill()
        proc.wait()
        return False, "Window did not appear within timeout", None

    time.sleep(0.5)

    out_img_name = f"{cpp_path.stem}.png"
    out_img_path = snap_dir / out_img_name

    try:
        subprocess.run(["import", "-window", child_id, str(out_img_path)], env=env, check=True, capture_output=True)
    except subprocess.CalledProcessError as e:
        proc.kill()
        proc.wait()
        return False, f"Import capture failed: {e}", None

    proc.terminate()
    try:
        proc.wait(timeout=1.0)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.wait()

    wait_for_no_window(env)

    bin_img_name = f"{bin_name}.png"
    bin_img_path = snap_dir / bin_img_name
    if bin_img_name != out_img_name:
        try:
            if bin_img_path.exists() or bin_img_path.is_symlink():
                bin_img_path.unlink()
            bin_img_path.symlink_to(out_img_name)
        except Exception:
            pass

    dim = "unknown"
    try:
        id_res = subprocess.run(["magick", "identify", "-format", "%wx%h", str(out_img_path)], capture_output=True, text=True)
        if id_res.returncode == 0 and id_res.stdout.strip():
            dim = id_res.stdout.strip()
    except Exception:
        pass

    return True, out_img_name, dim

def clean_images(target_labs):
    """Removes generated PNG images from specified lab directories."""
    count = 0
    for lab in target_labs:
        for png in glob.glob(os.path.join(lab, "**", "*.png"), recursive=True):
            os.remove(png)
            count += 1
            print(f"Removed: {png}")
    print(f"\nTotal removed: {count} images.")

def main():
    args = sys.argv[1:]

    if "--clean" in args:
        target_labs = [a for a in args if a != "--clean"]
        if not target_labs:
            target_labs = sorted(glob.glob("LAB *"))
        clean_images(target_labs)
        return

    target_labs = [a for a in args if not a.startswith("-")]
    if not target_labs:
        target_labs = sorted([d for d in glob.glob("LAB *") if os.path.isdir(d)])

    target_labs = [lab for lab in target_labs if glob.glob(os.path.join(lab, "**", "*.cpp"), recursive=True)]

    display = find_free_display()
    print(f"Starting headless Xvfb display on {display}...")
    xvfb_proc = start_xvfb(display)

    env = os.environ.copy()
    env["DISPLAY"] = display
    env["LIBGL_ALWAYS_SOFTWARE"] = "1"

    results = []

    def cleanup(signum=None, frame=None):
        print("\nTerminating Xvfb...")
        xvfb_proc.terminate()
        try:
            xvfb_proc.wait(timeout=2.0)
        except subprocess.TimeoutExpired:
            xvfb_proc.kill()
        if signum:
            sys.exit(1)

    signal.signal(signal.SIGINT, cleanup)
    signal.signal(signal.SIGTERM, cleanup)

    try:
        print("\n" + "=" * 80)
        print(f"{'LAB':<10} {'SOURCE':<35} {'OUTPUT IMAGE':<25} {'DIMENSIONS':<10}")
        print("=" * 80)

        for lab in target_labs:
            code_dir = os.path.join(lab, "c++ codes") if os.path.isdir(os.path.join(lab, "c++ codes")) else lab
            cpp_files = sorted(glob.glob(os.path.join(code_dir, "*.cpp")))
            for cpp_path_str in cpp_files:
                cpp_file = os.path.basename(cpp_path_str)
                lab_name = os.path.basename(lab)

                bin_name = Path(cpp_file).stem
                if lab_name in KNOWN_BINARY_MAP and cpp_file in KNOWN_BINARY_MAP[lab_name]:
                    bin_name = KNOWN_BINARY_MAP[lab_name][cpp_file]

                success, info, dim = capture_program(lab, cpp_file, bin_name, env)
                if success:
                    print(f"{lab_name:<10} {cpp_file:<35} {info:<25} {dim:<10}")
                    results.append((lab_name, cpp_file, info, dim, "OK"))
                else:
                    print(f"{lab_name:<10} {cpp_file:<35} [FAILED: {info}]")
                    results.append((lab_name, cpp_file, info, "-", "FAILED"))

        print("=" * 80)
        successful = sum(1 for r in results if r[4] == "OK")
        print(f"Capture completed: {successful}/{len(results)} graphics successfully saved.")
        print("=" * 80 + "\n")
    finally:
        cleanup()

if __name__ == "__main__":
    main()
