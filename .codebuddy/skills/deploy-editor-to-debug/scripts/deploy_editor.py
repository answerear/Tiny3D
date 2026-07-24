"""
Deploy Editor Assets to Debug Build Script

Copies editor asset files from the project's assets/editor source directory to
the Debug build output directory. Editor assets are copied as-is (no bundle
step is involved).

Mapping:
    assets/editor/       -> source/bin/Windows/Debug/Editor/

Usage:
    python deploy_editor.py --workspace D:/private/Tiny3D
    python deploy_editor.py --workspace D:/private/Tiny3D --clean
    python deploy_editor.py --workspace D:/private/Tiny3D --exclude "*.fbx,*.FBX"
    python deploy_editor.py --workspace D:/private/Tiny3D --dry-run
"""

import argparse
import fnmatch
import os
import shutil
import sys


# Copy mapping for editor assets: (source_relative, dest_relative)
EDITOR_SRC = os.path.join("assets", "editor")
EDITOR_DST = os.path.join("source", "bin", "Windows", "Debug", "Editor")


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


def main():
    parser = argparse.ArgumentParser(
        description="Deploy editor assets to the Debug build output"
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

    src_dir = os.path.join(workspace, EDITOR_SRC)
    dst_dir = os.path.join(workspace, EDITOR_DST)

    if not os.path.isdir(src_dir):
        print(f"ERROR: Editor source directory not found: {src_dir}")
        sys.exit(1)

    print(f"\n{'[DRY-RUN] ' if args.dry_run else ''}Deploying: {EDITOR_SRC} -> {EDITOR_DST}")

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
