#!/usr/bin/env python3
"""
Sync built-in asset files from assets/editor/builtin/ to assets/samples/meshes/.

- Copies ALL files from builtin/shaders/ (including .shader, .cginc, .meta, etc.)
- Copies .tmat (+ .meta) from builtin/materials/
- Copies .tshader (+ .meta) from builtin/TempShaders/
"""

import argparse
import os
import shutil
import sys

# Mapping: file extension -> builtin subdirectory name (for materials & TempShaders)
EXTENSION_TO_SUBDIR = {
    ".tmat": "materials",
    ".tshader": "TempShaders",
}

SUPPORTED_EXTENSIONS = list(EXTENSION_TO_SUBDIR.keys())


def sync_all_shaders(builtin_dir, target_dir, workspace, dry_run=False, names=None):
    """
    Copy ALL files from builtin/shaders/ to the target directory.
    When names is specified, only copy files whose stem (filename without all
    extensions, e.g. 'Tiny3D' from 'Tiny3D.cginc.meta') matches one of the names.
    """
    shaders_dir = os.path.join(builtin_dir, "shaders")
    if not os.path.isdir(shaders_dir):
        print(f"WARNING: Shaders directory not found: {shaders_dir}")
        return 0, True

    copied = 0
    success = True

    for fname in sorted(os.listdir(shaders_dir)):
        src = os.path.join(shaders_dir, fname)
        if not os.path.isfile(src):
            continue

        # Filter by --names if specified: match stem (name before first '.')
        if names is not None:
            stem = fname.split(".")[0]
            if stem not in names:
                continue

        dst = os.path.join(target_dir, fname)
        rel_src = os.path.relpath(src, workspace)
        rel_dst = os.path.relpath(dst, workspace)

        if dry_run:
            print(f"[DRY-RUN] {rel_src} -> {rel_dst}")
            copied += 1
        else:
            try:
                shutil.copy2(src, dst)
                print(f"Copied: {rel_src} -> {rel_dst}")
                copied += 1
            except Exception as e:
                print(f"ERROR copying {rel_src}: {e}", file=sys.stderr)
                success = False

    return copied, success


def find_target_assets(target_dir, names=None):
    """
    Scan the target directory for .tmat / .tshader files (non-.meta) to sync.
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
        os.makedirs(target_dir, exist_ok=True)
        print(f"Created target directory: {target_dir}")

    success = True
    copied = 0

    # --- Phase 1: Copy ALL files from builtin/shaders/ ---
    print("=== Syncing shaders (all files) ===")
    name_set = set(names) if names else None
    shader_copied, shader_ok = sync_all_shaders(
        builtin_dir, target_dir, workspace, dry_run=dry_run, names=name_set
    )
    copied += shader_copied
    if not shader_ok:
        success = False

    # --- Phase 2: Sync .tmat and .tshader by extension matching ---
    print("\n=== Syncing materials & compiled shaders ===")
    if names:
        name_set = set(names)
        assets = []
        for name in name_set:
            for ext in SUPPORTED_EXTENSIONS:
                subdir = EXTENSION_TO_SUBDIR[ext]
                src = os.path.join(builtin_dir, subdir, name + ext)
                if os.path.isfile(src):
                    assets.append((name, ext))
    else:
        assets = find_target_assets(target_dir, names=None)

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
                copied += 1
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
        print(f"\n[DRY-RUN] {copied} file(s) would be processed.")

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
