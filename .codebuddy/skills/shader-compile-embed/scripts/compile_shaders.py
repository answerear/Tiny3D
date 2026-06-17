"""
Compile .vshader and .pshader files using scc.exe to generate HLSL, GLSL, SPIR-V, and ESSL output.

Usage:
    python compile_shaders.py --scc <scc_path> --src <shader_dir> --hlsl-out <hlsl_dir> --glsl-out <glsl_dir> [--spirv-out <spirv_dir>] [--essl-out <essl_dir>] [--extra-args <args>]

Example:
    python compile_shaders.py \
        --scc source/bin/Windows/Debug/scc.exe \
        --src assets/samples/shaders \
        --hlsl-out assets/samples/shaders/output/HLSL \
        --glsl-out assets/samples/shaders/output/OpenGL4 \
        --spirv-out assets/samples/shaders/output/Vulkan \
        --essl-out assets/samples/shaders/output/OpenGLES3 \
        --extra-args "-N -O0"
"""

import argparse
import glob
import os
import shutil
import subprocess
import sys
import tempfile


def find_shaders(src_dir):
    """Find all .vshader and .pshader files in the source directory."""
    patterns = ['*.vshader', '*.pshader']
    files = []
    for pat in patterns:
        files.extend(glob.glob(os.path.join(src_dir, pat)))
    return sorted(files)


def compile_shader_with_define(scc_path, shader_path, target, output_dir, extra_args, define_macro):
    """Compile a single shader file with scc.exe.

    Instead of passing -D to scc.exe (which appends the macro name to the
    output filename), we inject a #define line at the top of a temporary
    copy of the shader source and compile that temporary file.
    """
    basename = os.path.basename(shader_path)
    print(f'  [{target.upper()}] {basename}')

    # Read original source
    with open(shader_path, 'r', encoding='utf-8') as f:
        original = f.read()

    # Prepend #define
    modified = f'#define {define_macro}\n{original}'

    # Write to a temp file that keeps the original filename (scc uses it
    # to derive the output filename).
    tmp_dir = tempfile.mkdtemp()
    tmp_path = os.path.join(tmp_dir, basename)
    try:
        with open(tmp_path, 'w', encoding='utf-8') as f:
            f.write(modified)

        cmd = [scc_path, tmp_path, '-t', target, '-o', output_dir] + extra_args
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f'    ERROR (exit {result.returncode}): {result.stderr.strip()}')
            return False
        return True
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def main():
    parser = argparse.ArgumentParser(description='Batch compile shaders using scc.exe')
    parser.add_argument('--scc', required=True, help='Path to scc.exe')
    parser.add_argument('--src', required=True, help='Source directory containing .vshader/.pshader files')
    parser.add_argument('--hlsl-out', required=True, help='Output directory for HLSL files')
    parser.add_argument('--glsl-out', required=True, help='Output directory for GLSL files')
    parser.add_argument('--spirv-out', default='', help='Output directory for SPIR-V files (optional)')
    parser.add_argument('--essl-out', default='', help='Output directory for ESSL (GLES3) files (optional)')
    parser.add_argument('--extra-args', default='-N -O0', help='Extra arguments for scc.exe (default: -N -O0)')
    args = parser.parse_args()

    extra = args.extra_args.split()
    shaders = find_shaders(args.src)

    if not shaders:
        print(f'No .vshader or .pshader files found in {args.src}')
        sys.exit(1)

    print(f'Found {len(shaders)} shader files in {args.src}')

    # Ensure output dirs exist
    os.makedirs(args.hlsl_out, exist_ok=True)
    os.makedirs(args.glsl_out, exist_ok=True)
    if args.spirv_out:
        os.makedirs(args.spirv_out, exist_ok=True)
    if args.essl_out:
        os.makedirs(args.essl_out, exist_ok=True)

    # Build target list
    targets = [
        ('hlsl', args.hlsl_out, 'TINY3D_DIRECTX'),
        ('glsl', args.glsl_out, 'TINY3D_OPENGL'),
    ]
    if args.spirv_out:
        targets.append(('spirv', args.spirv_out, 'TINY3D_VULKAN'))
    if args.essl_out:
        targets.append(('essl', args.essl_out, 'TINY3D_OPENGLES'))

    success = 0
    fail = 0

    for shader in shaders:
        for target, out_dir, define_macro in targets:
            if compile_shader_with_define(args.scc, shader, target, out_dir, extra, define_macro):
                success += 1
            else:
                fail += 1

    total = len(shaders) * len(targets)
    print(f'\nDone: {success} succeeded, {fail} failed out of {total} total compilations.')
    if fail > 0:
        sys.exit(1)


if __name__ == '__main__':
    main()
