---
name: deploy-assets-to-debug
description: This skill copies editor and sample asset files from the source assets directory to the Debug build output directory. It copies assets/editor to source/bin/Windows/Debug/Editor, and assets/samples to source/bin/Windows/Debug/Assets/samples. Trigger phrases include "deploy assets", "copy assets to debug", "sync assets to build", "update debug assets", "deploy editor assets", "deploy sample assets", "copy resources to debug output", or any reference to copying assets into the Debug build directory.
---

# Deploy Assets to Debug Build Skill

## Purpose

Copy asset files from the project's `assets/` source directories to the Debug build output directory, so that the running application can load the latest resources.

## Copy Mapping

| Source | Destination |
|---|---|
| `assets/editor/` | `source/bin/Windows/Debug/Editor/` |
| `assets/samples/` | `source/bin/Windows/Debug/Assets/samples/` |

## Usage

### Run via Python Script

```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D
```

### Parameters

| Parameter | Required | Default | Description |
|---|---|---|---|
| `--workspace` | Yes | — | Workspace root path (e.g., `D:\private\Tiny3D`) |
| `--target` | No | `all` | What to deploy: `all`, `editor`, `samples` |
| `--clean` | No | Off | Remove destination directories before copying |
| `--dry-run` | No | Off | Preview which files would be copied without actually copying |
| `--exclude` | No | — | Comma-separated glob patterns to exclude (e.g., `*.fbx,*.FBX`) |

### Examples

Deploy all assets (editor + samples):
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D
```

Deploy only editor assets:
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D --target editor
```

Deploy only sample assets:
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D --target samples
```

Clean deploy (remove old files first):
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D --clean
```

Preview changes:
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D --dry-run
```

Exclude raw FBX files:
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D --exclude "*.fbx,*.FBX"
```

## Workflow

1. **Validate** — Check that source directories exist
2. **Prepare** — Optionally clean destination directories if `--clean` is specified
3. **Copy** — Recursively copy files from source to destination, preserving directory structure
4. **Report** — Print summary of files copied, skipped, or failed

## When to Run

- After running `builtin-generator` to regenerate built-in editor resources
- After running `sync-builtin-to-samples` to update sample assets
- After running `mesh-converter` to convert new FBX models
- After running `shader-compile-embed` or `compile-builtin-shaders` to recompile shaders
- After modifying any asset files (shaders, materials, textures, meshes) in the source directories
- Before launching the application from the Debug build to ensure it has the latest resources

## Notes

- This skill uses `shutil.copytree` with `dirs_exist_ok=True` (Python 3.8+) to merge into existing directories.
- Files in the destination that are not in the source are NOT deleted unless `--clean` is used.
- The `--exclude` option uses `fnmatch` patterns to skip files matching the given patterns.
- Exit code 0 on success, non-zero on failure.
