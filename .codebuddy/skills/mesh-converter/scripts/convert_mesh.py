#!/usr/bin/env python3
"""
Convert FBX files to Tiny3D engine format using mconv.exe.

Usage:
    python convert_mesh.py --workspace <root> --input <fbx_file_or_dir> [options]

Example:
    python convert_mesh.py \
        --workspace D:\\private\\Tiny3D \
        --input D:\\private\\Tiny3D\\assets\\samples\\raw\\tortoise.fbx \
        --output-dir D:\\private\\Tiny3D\\assets\\samples\\meshes
"""

import argparse
import glob
import os
import subprocess
import sys


# ── Defaults ──────────────────────────────────────────────────────────────────
DEFAULT_MCONV = os.path.join("source", "bin", "Windows", "Debug", "mconv.exe")
DEFAULT_OUTPUT_TYPE = "Tiny3D"
DEFAULT_BOUND = "sphere"
DEFAULT_MATERIAL = os.path.join("materials", "Tiny3DStandard.tmat")
DEFAULT_RESOURCE_ROOT = os.path.join("assets", "editor", "builtin")

VALID_OUTPUT_TYPES = ["Tiny3D", "tmesh", "tskin", "tskel", "tani"]
VALID_BOUND_TYPES = ["sphere", "AABB"]


def find_fbx_files(directory):
    """Find all .fbx files (case-insensitive) in a directory (non-recursive)."""
    fbx_files = []
    if not os.path.isdir(directory):
        return fbx_files
    for fname in os.listdir(directory):
        if fname.lower().endswith(".fbx"):
            fbx_files.append(os.path.join(directory, fname))
    return sorted(fbx_files)


def build_command(mconv_path, input_path, output_dir, output_type, bound,
                  material, resource_root, text_mode, gen_meta, verbose):
    """Assemble the mconv.exe command line."""
    cmd = [mconv_path, "-o", output_type, "-b", bound]

    if text_mode:
        cmd.append("-t")
    if gen_meta:
        cmd.append("-a")
    if verbose:
        cmd.append("-v")

    cmd.extend(["-r", resource_root])
    cmd.extend(["-d", material])
    cmd.append(input_path)

    if output_dir:
        cmd.append(output_dir)

    return cmd


def run_conversion(cmd, dry_run=False):
    """Execute a single conversion command. Returns True on success."""
    cmd_str = " ".join(cmd)

    if dry_run:
        print(f"[DRY-RUN] {cmd_str}")
        return True

    print(f"Running: {cmd_str}")
    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.stdout.strip():
            print(result.stdout.strip())
        if result.returncode != 0:
            print(f"  ERROR (exit {result.returncode})")
            if result.stderr.strip():
                print(f"  stderr: {result.stderr.strip()}")
            return False
        print("  OK")
        return True
    except FileNotFoundError:
        print(f"  ERROR: mconv.exe not found at {cmd[0]}")
        return False
    except Exception as e:
        print(f"  ERROR: {e}")
        return False


def validate_paths(mconv_path, resource_root, material, workspace):
    """Validate that required paths exist. Returns list of error messages."""
    errors = []

    if not os.path.isfile(mconv_path):
        errors.append(
            f"mconv.exe not found: {mconv_path}\n"
            f"  Please build the MeshConverter project first."
        )

    if not os.path.isdir(resource_root):
        errors.append(f"Resource root directory not found: {resource_root}")

    material_full = os.path.join(resource_root, material)
    if os.path.isdir(resource_root) and not os.path.isfile(material_full):
        errors.append(
            f"Material template not found: {material_full}\n"
            f"  (resource root: {resource_root}, material: {material})"
        )

    return errors


def main():
    parser = argparse.ArgumentParser(
        description="Convert FBX files to Tiny3D engine format using mconv.exe."
    )
    parser.add_argument(
        "--workspace", required=True,
        help="Workspace root path (e.g., D:\\private\\Tiny3D)"
    )
    parser.add_argument(
        "--input", required=True,
        help="Path to a .fbx file, or a directory when used with --batch"
    )
    parser.add_argument(
        "--output-dir", default=None,
        help="Output directory (default: same as input file directory)"
    )
    parser.add_argument(
        "--output-type", default=DEFAULT_OUTPUT_TYPE,
        choices=VALID_OUTPUT_TYPES,
        help=f"Output format (default: {DEFAULT_OUTPUT_TYPE})"
    )
    parser.add_argument(
        "--bound", default=DEFAULT_BOUND,
        choices=VALID_BOUND_TYPES,
        help=f"Bounding volume type (default: {DEFAULT_BOUND})"
    )
    parser.add_argument(
        "--material", default=DEFAULT_MATERIAL,
        help=f"Default material path relative to resource root (default: {DEFAULT_MATERIAL})"
    )
    parser.add_argument(
        "--resource-root", default=DEFAULT_RESOURCE_ROOT,
        help=f"Resource root directory relative to workspace (default: {DEFAULT_RESOURCE_ROOT})"
    )
    parser.add_argument(
        "--mconv", default=DEFAULT_MCONV,
        help=f"Path to mconv.exe relative to workspace (default: {DEFAULT_MCONV})"
    )
    parser.add_argument(
        "--text", action="store_true", default=True, dest="text",
        help="Output text format (default: enabled)"
    )
    parser.add_argument(
        "--no-text", action="store_false", dest="text",
        help="Output binary format instead of text"
    )
    parser.add_argument(
        "--meta", action="store_true", default=True, dest="meta",
        help="Generate .meta files (default: enabled)"
    )
    parser.add_argument(
        "--no-meta", action="store_false", dest="meta",
        help="Do not generate .meta files"
    )
    parser.add_argument(
        "--batch", action="store_true",
        help="Batch mode: scan --input directory for all .fbx files"
    )
    parser.add_argument(
        "--dry-run", action="store_true",
        help="Print commands without executing them"
    )
    parser.add_argument(
        "--verbose", action="store_true",
        help="Enable verbose output from mconv.exe"
    )

    args = parser.parse_args()

    workspace = os.path.abspath(args.workspace)

    # Resolve paths relative to workspace
    mconv_path = os.path.abspath(os.path.join(workspace, args.mconv))
    resource_root = os.path.abspath(os.path.join(workspace, args.resource_root))
    input_path = os.path.abspath(args.input)

    # Validate paths (skip in dry-run for convenience)
    if not args.dry_run:
        errors = validate_paths(mconv_path, resource_root, args.material, workspace)
        if errors:
            for err in errors:
                print(f"ERROR: {err}", file=sys.stderr)
            sys.exit(1)

    # Collect FBX files
    if args.batch:
        if not os.path.isdir(input_path):
            print(f"ERROR: --batch requires --input to be a directory: {input_path}",
                  file=sys.stderr)
            sys.exit(1)
        fbx_files = find_fbx_files(input_path)
        if not fbx_files:
            print(f"No .fbx files found in {input_path}")
            sys.exit(0)
        print(f"Found {len(fbx_files)} FBX file(s) in {input_path}")
    else:
        if not os.path.isfile(input_path) and not args.dry_run:
            print(f"ERROR: Input file not found: {input_path}", file=sys.stderr)
            sys.exit(1)
        fbx_files = [input_path]

    # Run conversions
    success = 0
    fail = 0

    for fbx in fbx_files:
        fbx_abs = os.path.abspath(fbx)

        # Determine output directory
        if args.output_dir:
            out_dir = os.path.abspath(args.output_dir)
        else:
            out_dir = os.path.dirname(fbx_abs)

        # Ensure output directory exists
        if not args.dry_run:
            os.makedirs(out_dir, exist_ok=True)

        cmd = build_command(
            mconv_path=mconv_path,
            input_path=fbx_abs,
            output_dir=out_dir,
            output_type=args.output_type,
            bound=args.bound,
            material=args.material,
            resource_root=resource_root,
            text_mode=args.text,
            gen_meta=args.meta,
            verbose=args.verbose,
        )

        if run_conversion(cmd, dry_run=args.dry_run):
            success += 1
        else:
            fail += 1

    # Summary
    total = success + fail
    print(f"\nDone: {success} succeeded, {fail} failed out of {total} conversion(s).")
    if fail > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
