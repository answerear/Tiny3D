"""
Compile Builtin Shaders Script

Compiles all .shader files under the builtin shaders directory into .tshader
files using scc.exe (ShaderCrossCompiler).

Usage:
    python compile_builtin_shaders.py
    python compile_builtin_shaders.py --scc <scc_path> --input <shader_dir> --output <output_dir>
    python compile_builtin_shaders.py --clean --target hlsl
"""

import argparse
import glob
import os
import shutil
import subprocess
import sys


def find_project_root():
    """Find the Tiny3D project root by walking up from this script's location."""
    current = os.path.dirname(os.path.abspath(__file__))
    while current != os.path.dirname(current):
        if os.path.isdir(os.path.join(current, ".codebuddy")):
            return current
        current = os.path.dirname(current)
    return None


def compile_shader(scc_path, shader_file, output_dir, target):
    """Compile a single .shader file using scc.exe.

    Args:
        scc_path: Path to scc.exe
        shader_file: Path to the .shader source file
        output_dir: Output directory for the compiled .tshader
        target: Compilation target language (hlsl or glsl)

    Returns:
        tuple: (success: bool, message: str)
    """
    shader_name = os.path.basename(shader_file)
    cmd = [scc_path, shader_file, "-t", target, "-o", output_dir]

    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=120,
        )
        if result.returncode == 0:
            return True, f"  [OK] {shader_name}"
        else:
            error_msg = result.stderr.strip() or result.stdout.strip() or f"exit code {result.returncode}"
            return False, f"  [FAIL] {shader_name}: {error_msg}"
    except subprocess.TimeoutExpired:
        return False, f"  [FAIL] {shader_name}: compilation timed out (120s)"
    except FileNotFoundError:
        return False, f"  [FAIL] {shader_name}: scc.exe not found at {scc_path}"
    except Exception as e:
        return False, f"  [FAIL] {shader_name}: {e}"


def main():
    parser = argparse.ArgumentParser(
        description="Compile builtin .shader files to .tshader using scc.exe"
    )
    parser.add_argument(
        "--scc", default=None,
        help="Path to scc.exe (default: source/bin/Windows/Debug/scc.exe)"
    )
    parser.add_argument(
        "--input", default=None,
        help="Input directory containing .shader files (default: assets/editor/builtin/shaders)"
    )
    parser.add_argument(
        "--output", default=None,
        help="Output directory for .tshader files (default: assets/editor/builtin/TempShaders)"
    )
    parser.add_argument(
        "--clean", action="store_true",
        help="Remove output directory before compilation"
    )
    parser.add_argument(
        "--target", default="hlsl", choices=["hlsl", "glsl"],
        help="Compilation target language (default: hlsl)"
    )
    args = parser.parse_args()

    # Resolve project root
    project_root = find_project_root()
    if project_root is None:
        print("ERROR: Could not find Tiny3D project root (.codebuddy directory not found).")
        sys.exit(1)

    # Resolve paths
    scc_path = args.scc or os.path.join(project_root, "source", "bin", "Windows", "Debug", "scc.exe")
    input_dir = args.input or os.path.join(project_root, "assets", "editor", "builtin", "shaders")
    output_dir = args.output or os.path.join(project_root, "assets", "editor", "builtin", "TempShaders")

    # Verify scc.exe exists
    if not os.path.isfile(scc_path):
        print(f"ERROR: scc.exe not found at: {scc_path}")
        print("Please build the ShaderCrossCompiler project first via CMake.")
        sys.exit(1)

    # Verify input directory exists
    if not os.path.isdir(input_dir):
        print(f"ERROR: Shader source directory not found: {input_dir}")
        sys.exit(1)

    # Find all .shader files
    shader_files = sorted(glob.glob(os.path.join(input_dir, "*.shader")))
    if not shader_files:
        print(f"WARNING: No .shader files found in: {input_dir}")
        sys.exit(0)

    # Prepare output directory
    if args.clean and os.path.exists(output_dir):
        print(f"Cleaning output directory: {output_dir}")
        shutil.rmtree(output_dir)

    os.makedirs(output_dir, exist_ok=True)

    # Compile each shader
    print(f"Compiling {len(shader_files)} shader(s) with target '{args.target}'...")
    print(f"  Input:  {input_dir}")
    print(f"  Output: {output_dir}")
    print(f"  SCC:    {scc_path}")
    print()

    success_count = 0
    fail_count = 0

    for shader_file in shader_files:
        ok, msg = compile_shader(scc_path, shader_file, output_dir, args.target)
        print(msg)
        if ok:
            success_count += 1
        else:
            fail_count += 1

    # Summary
    print()
    print(f"Compilation complete: {success_count} succeeded, {fail_count} failed, {len(shader_files)} total.")

    if fail_count > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
