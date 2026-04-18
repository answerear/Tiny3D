#!/usr/bin/env python3
"""
Sync built-in asset files from assets/editor/builtin/ to assets/samples/meshes/.

Copies .shader, .tmat, and .tshader files (with their .meta companions)
from the appropriate builtin subdirectory to the samples/meshes target.
"""

import argparse
import os
import shutil
import sys

# Mapping: file extension -> builtin subdirectory name
EXTENSION_TO_SUBDIR = {
    ".shader": "shaders",
    ".tmat": "materials",
    ".tshader": "TempShaders",
}

SUPPORTED_EXTENSIONS = list(EXTENSION_TO_SUBDIR.keys())


def find_target_assets(target_dir, names=None):
    """
    Scan the target directory for asset files (non-.meta) that we can sync.
    Returns a list of (basename_without_ext, extension) tuples.
    """
    assets = []
    if not os.path.isdir(target_dir):
        return assets

    for fname in os.listdir(target_dir):
        if fname.endswith(".meta"):
            continue
        for ext in SUPPORTED_EXTENSIONS:
            if fname.endswith(ext):
                base = fname[: -len(ext)]
                if names is None or base in names:
                    assets.append((base, ext))
                break
    return assets


def sync_assets(workspace, dry_run=False, names=None):
    builtin_dir = os.path.join(workspace, "assets", "editor", "builtin")
    target_dir = os.path.join(workspace, "assets", "samples", "meshes")

    if not os.path.isdir(builtin_dir):
        print(f"ERROR: Builtin directory not found: {builtin_dir}", file=sys.stderr)
        return False

    if not os.path.isdir(target_dir):
        print(f"ERROR: Target directory not found: {target_dir}", file=sys.stderr)
        return False

    # Determine which assets to sync
    if names:
        name_set = set(names)
        # For explicit names, check all supported extensions
        assets = []
        for name in name_set:
            for ext in SUPPORTED_EXTENSIONS:
                subdir = EXTENSION_TO_SUBDIR[ext]
                src = os.path.join(builtin_dir, subdir, name + ext)
                if os.path.isfile(src):
                    assets.append((name, ext))
    else:
        assets = find_target_assets(target_dir)

    if not assets:
        print("No matching assets found to sync.")
        return True

    success = True
    copied = 0

    for base, ext in assets:
        subdir = EXTENSION_TO_SUBDIR[ext]
        src_file = os.path.join(builtin_dir, subdir, base + ext)
        src_meta = src_file + ".meta"
        dst_file = os.path.join(target_dir, base + ext)
        dst_meta = dst_file + ".meta"

        for src, dst in [(src_file, dst_file), (src_meta, dst_meta)]:
            if not os.path.isfile(src):
                print(f"WARNING: Source not found, skipping: {src}")
                continue

            rel_src = os.path.relpath(src, workspace)
            rel_dst = os.path.relpath(dst, workspace)

            if dry_run:
                print(f"[DRY-RUN] {rel_src} -> {rel_dst}")
            else:
                try:
                    shutil.copy2(src, dst)
                    print(f"Copied: {rel_src} -> {rel_dst}")
                    copied += 1
                except Exception as e:
                    print(f"ERROR copying {rel_src}: {e}", file=sys.stderr)
                    success = False

    if not dry_run:
        print(f"\nDone. {copied} file(s) copied.")
    else:
        print(f"\n[DRY-RUN] {len(assets) * 2} file(s) would be processed.")

    return success


def main():
    parser = argparse.ArgumentParser(
        description="Sync builtin assets to samples/meshes directory."
    )
    parser.add_argument(
        "--workspace",
        required=True,
        help="Workspace root path (e.g., D:\\private\\Tiny3D)",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Preview changes without copying files",
    )
    parser.add_argument(
        "--names",
        type=str,
        default=None,
        help="Comma-separated asset names to sync (e.g., Tiny3DStandard,Test-Material)",
    )

    args = parser.parse_args()

    names = None
    if args.names:
        names = [n.strip() for n in args.names.split(",") if n.strip()]

    ok = sync_assets(args.workspace, dry_run=args.dry_run, names=names)
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
