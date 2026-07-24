"""
Deploy Sample Assets to Debug Build Script

Deploys the sample assets to the Debug build output. First runs bundlebuilder
to rebuild assets/samples/bundle from assets/samples/meshes (same as ResourceApp
POST_BUILD), then copies assets/samples (including the fresh bundle) to the
Debug output.

Mapping:
    assets/samples/      -> source/bin/Windows/Debug/Assets/samples/

Usage:
    python deploy_samples.py --workspace D:/private/Tiny3D
    python deploy_samples.py --workspace D:/private/Tiny3D --skip-bundle
    python deploy_samples.py --workspace D:/private/Tiny3D --keep-languages hlsl,glsl,spirv
    python deploy_samples.py --workspace D:/private/Tiny3D --clean --exclude "*.fbx,*.FBX"
    python deploy_samples.py --workspace D:/private/Tiny3D --dry-run
"""

import argparse
import fnmatch
import os
import shutil
import subprocess
import sys


# Copy mapping for sample assets: (source_relative, dest_relative)
SAMPLES_SRC = os.path.join("assets", "samples")
SAMPLES_DST = os.path.join("source", "bin", "Windows", "Debug", "Assets", "samples")

DEFAULT_BUNDLEBUILDER = os.path.join("source", "bin", "Windows", "Debug", "bundlebuilder.exe")
DEFAULT_MESHES_DIR = os.path.join("assets", "samples", "meshes")
DEFAULT_BUNDLE_DIR = os.path.join("assets", "samples", "bundle")


def should_exclude(filename, exclude_patterns):
    """Check if a filename matches any exclusion pattern."""
    for pattern in exclude_patterns:
        if fnmatch.fnmatch(filename, pattern):
            return True
    return False


def copy_tree(src, dst, exclude_patterns, dry_run=False):
    """Recursively copy src to dst, returning (copied, skipped, failed) counts."""
    copied = 0
    skipped = 0
    failed = 0

    for root, dirs, files in os.walk(src):
        rel_dir = os.path.relpath(root, src)
        dest_dir = os.path.join(dst, rel_dir) if rel_dir != "." else dst

        if not dry_run:
            os.makedirs(dest_dir, exist_ok=True)

        for filename in files:
            if should_exclude(filename, exclude_patterns):
                skipped += 1
                continue

            src_file = os.path.join(root, filename)
            dst_file = os.path.join(dest_dir, filename)

            if dry_run:
                print(f"  [DRY-RUN] {src_file} -> {dst_file}")
                copied += 1
                continue

            try:
                shutil.copy2(src_file, dst_file)
                copied += 1
            except Exception as e:
                print(f"  [FAIL] {src_file}: {e}")
                failed += 1

    return copied, skipped, failed


def build_samples_bundle(workspace, bundlebuilder, meshes_rel, bundle_rel,
                         keep_languages=None, dry_run=False):
    """
    Run bundlebuilder to generate assets/samples/bundle from meshes.

    Working directory must be the exe directory so editor plugins load via
    pluginPath="." (same requirement as ResourceApp POST_BUILD).
    """
    exe = bundlebuilder if os.path.isabs(bundlebuilder) else os.path.join(workspace, bundlebuilder)
    meshes_dir = os.path.join(workspace, meshes_rel)
    bundle_dir = os.path.join(workspace, bundle_rel)

    if not os.path.isfile(exe):
        print(f"ERROR: bundlebuilder not found: {exe}")
        print("  Please build BundleBuilder first (source/bin/Windows/Debug/bundlebuilder.exe).")
        return False

    if not os.path.isdir(meshes_dir):
        print(f"ERROR: Samples meshes directory not found: {meshes_dir}")
        return False

    cmd = [exe, "--assets", meshes_dir, "--out", bundle_dir, "--binary"]
    if keep_languages:
        cmd.extend(["--keep-languages", keep_languages])

    exe_dir = os.path.dirname(exe)
    print(f"\n{'[DRY-RUN] ' if dry_run else ''}Building samples bundle:")
    print(f"  Command: {' '.join(cmd)}")
    print(f"  CWD:     {exe_dir}")

    if dry_run:
        return True

    os.makedirs(bundle_dir, exist_ok=True)
    result = subprocess.run(cmd, cwd=exe_dir)
    if result.returncode != 0:
        print(f"ERROR: bundlebuilder failed with exit code {result.returncode}")
        return False

    print(f"  Bundle ready: {bundle_dir}")
    return True


def main():
    parser = argparse.ArgumentParser(
        description="Deploy sample assets to the Debug build output"
    )
    parser.add_argument(
        "--workspace", required=True,
        help="Workspace root path (e.g., D:\\private\\Tiny3D)"
    )
    parser.add_argument(
        "--clean", action="store_true",
        help="Remove destination directory before copying"
    )
    parser.add_argument(
        "--dry-run", action="store_true",
        help="Preview BundleBuilder command and copy list without executing"
    )
    parser.add_argument(
        "--exclude", default="",
        help="Comma-separated glob patterns to exclude (e.g., '*.fbx,*.FBX')"
    )
    parser.add_argument(
        "--skip-bundle", action="store_true",
        help="Skip BundleBuilder step (use existing assets/samples/bundle as-is)"
    )
    parser.add_argument(
        "--bundlebuilder", default=DEFAULT_BUNDLEBUILDER,
        help="Path to bundlebuilder.exe (relative to workspace or absolute)"
    )
    parser.add_argument(
        "--meshes-dir", default=DEFAULT_MESHES_DIR,
        help="Input meshes directory for BundleBuilder (relative to workspace)"
    )
    parser.add_argument(
        "--bundle-dir", default=DEFAULT_BUNDLE_DIR,
        help="Output bundle directory for BundleBuilder (relative to workspace)"
    )
    parser.add_argument(
        "--keep-languages", default="",
        help="Pass-through to bundlebuilder --keep-languages (e.g. hlsl,glsl,spirv)"
    )
    args = parser.parse_args()

    workspace = os.path.abspath(args.workspace)
    if not os.path.isdir(workspace):
        print(f"ERROR: Workspace directory not found: {workspace}")
        sys.exit(1)

    exclude_patterns = [p.strip() for p in args.exclude.split(",") if p.strip()]

    # Rebuild bundle before copy (matches ResourceApp POST_BUILD)
    if not args.skip_bundle:
        keep = args.keep_languages.strip() or None
        ok = build_samples_bundle(
            workspace,
            args.bundlebuilder,
            args.meshes_dir,
            args.bundle_dir,
            keep_languages=keep,
            dry_run=args.dry_run,
        )
        if not ok:
            sys.exit(1)
    else:
        print("\nSkipping BundleBuilder (--skip-bundle); using existing samples/bundle.")

    src_dir = os.path.join(workspace, SAMPLES_SRC)
    dst_dir = os.path.join(workspace, SAMPLES_DST)

    if not os.path.isdir(src_dir):
        print(f"ERROR: Samples source directory not found: {src_dir}")
        sys.exit(1)

    print(f"\n{'[DRY-RUN] ' if args.dry_run else ''}Deploying: {SAMPLES_SRC} -> {SAMPLES_DST}")

    if args.clean and not args.dry_run:
        if os.path.exists(dst_dir):
            print(f"  Cleaning: {dst_dir}")
            shutil.rmtree(dst_dir)

    copied, skipped, failed = copy_tree(src_dir, dst_dir, exclude_patterns, args.dry_run)

    print(f"  Result: {copied} copied, {skipped} skipped, {failed} failed")
    print(f"\nDone. Total: {copied} copied, {skipped} skipped, {failed} failed.")

    if failed > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
