"""
Deploy Assets to Debug Build Script

Copies asset files from the project's assets/ source directories to the
Debug build output directory.

Mapping:
    assets/editor/       -> source/bin/Windows/Debug/Editor/
    assets/samples/      -> source/bin/Windows/Debug/Assets/samples/

Usage:
    python deploy_assets.py --workspace D:/private/Tiny3D
    python deploy_assets.py --workspace D:/private/Tiny3D --target editor
    python deploy_assets.py --workspace D:/private/Tiny3D --clean --exclude "*.fbx,*.FBX"
"""

import argparse
import fnmatch
import os
import shutil
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
                rel_src = os.path.relpath(src_file, os.path.dirname(src))
                rel_dst = os.path.relpath(dst_file, os.path.dirname(dst))
                print(f"  [DRY-RUN] {rel_src} -> {rel_dst}")
                copied += 1
                continue

            try:
                shutil.copy2(src_file, dst_file)
                copied += 1
            except Exception as e:
                print(f"  [FAIL] {src_file}: {e}")
                failed += 1

    return copied, skipped, failed


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
    args = parser.parse_args()

    workspace = os.path.abspath(args.workspace)
    if not os.path.isdir(workspace):
        print(f"ERROR: Workspace directory not found: {workspace}")
        sys.exit(1)

    exclude_patterns = [p.strip() for p in args.exclude.split(",") if p.strip()]
    indices = TARGET_KEYS[args.target]

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

        label = src_rel
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
