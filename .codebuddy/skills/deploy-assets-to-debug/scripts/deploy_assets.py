"""
Deploy Assets to Debug Build Script

Copies asset files from the project's assets/ source directories to the
Debug build output directory.

For samples: first runs bundlebuilder to rebuild assets/samples/bundle
from assets/samples/meshes (same as ResourceApp POST_BUILD), then copies.

Mapping:
    assets/editor/       -> source/bin/Windows/Debug/Editor/
    assets/samples/      -> source/bin/Windows/Debug/Assets/samples/

Usage:
    python deploy_assets.py --workspace D:/private/Tiny3D
    python deploy_assets.py --workspace D:/private/Tiny3D --target editor
    python deploy_assets.py --workspace D:/private/Tiny3D --clean --exclude "*.fbx,*.FBX"
    python deploy_assets.py --workspace D:/private/Tiny3D --skip-bundle
"""

import argparse
import fnmatch
import os
import shutil
import subprocess
import sys


# Copy mapping: (source_relative, dest_relative)
COPY_MAPPING = [
    ("assets/editor", "source/bin/Windows/Debug/Editor"),
    ("assets/samples", "source/bin/Windows/Debug/Assets/samples"),
]

TARGET_KEYS = {
    "editor": [0],
    "samples": [1],
    "all": [0, 1],
}

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
        # Compute relative path from src
        rel_dir = os.path.relpath(root, src)
        dest_dir = os.path.join(dst, rel_dir) if rel_dir != "." else dst

        # Create destination directory
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
        description="Deploy assets from source directories to Debug build output"
    )
    parser.add_argument(
        "--workspace", required=True,
        help="Workspace root path (e.g., D:\\private\\Tiny3D)"
    )
    parser.add_argument(
        "--target", default="all", choices=["all", "editor", "samples"],
        help="What to deploy: all, editor, or samples (default: all)"
    )
    parser.add_argument(
        "--clean", action="store_true",
        help="Remove destination directories before copying"
    )
    parser.add_argument(
        "--dry-run", action="store_true",
        help="Preview which files would be copied without actually copying"
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
    indices = TARGET_KEYS[args.target]
    deploy_samples = 1 in indices

    # Samples: rebuild bundle before copy (matches ResourceApp POST_BUILD)
    if deploy_samples and not args.skip_bundle:
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
    elif deploy_samples and args.skip_bundle:
        print("\nSkipping BundleBuilder (--skip-bundle); using existing samples/bundle.")

    total_copied = 0
    total_skipped = 0
    total_failed = 0

    for idx in indices:
        src_rel, dst_rel = COPY_MAPPING[idx]
        src_dir = os.path.join(workspace, src_rel)
        dst_dir = os.path.join(workspace, dst_rel)

        if not os.path.isdir(src_dir):
            print(f"WARNING: Source directory not found, skipping: {src_dir}")
            continue

        print(f"\n{'[DRY-RUN] ' if args.dry_run else ''}Deploying: {src_rel} -> {dst_rel}")

        if args.clean and not args.dry_run:
            if os.path.exists(dst_dir):
                print(f"  Cleaning: {dst_dir}")
                shutil.rmtree(dst_dir)

        copied, skipped, failed = copy_tree(src_dir, dst_dir, exclude_patterns, args.dry_run)
        total_copied += copied
        total_skipped += skipped
        total_failed += failed

        print(f"  Result: {copied} copied, {skipped} skipped, {failed} failed")

    # Summary
    print(f"\nDone. Total: {total_copied} copied, {total_skipped} skipped, {total_failed} failed.")

    if total_failed > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
