---
name: sync-builtin-to-samples
description: This skill synchronizes built-in asset files from the builtin directory to the samples/meshes directory. It copies shader (.shader), material (.tmat), and compiled shader (.tshader) files along with their .meta files from assets/editor/builtin subdirectories to assets/samples/meshes. This skill should be used when the user wants to sync builtin files to samples, update sample assets from builtin, copy builtin shaders/materials to samples, or refresh sample meshes directory with builtin resources. Trigger phrases include "sync builtin to samples", "copy builtin to samples", "update samples from builtin", "refresh sample assets", "sync samples meshes", or any mention of copying files from builtin to samples/meshes.
---

# Sync Builtin to Samples Skill

## Purpose

Copy built-in asset files from `assets/editor/builtin/` subdirectories to `assets/samples/meshes/`, overwriting existing files. This ensures the sample project always has the latest built-in resources.

## Key Paths

| Resource | Path |
|---|---|
| Builtin shaders | `assets/editor/builtin/shaders/` |
| Builtin materials | `assets/editor/builtin/materials/` |
| Builtin TempShaders | `assets/editor/builtin/TempShaders/` |
| Target directory | `assets/samples/meshes/` |

## File Mapping

The sync copies files based on name matching across these builtin subdirectories:

| File Extension | Source Subdirectory | Example |
|---|---|---|
| `.shader` + `.shader.meta` | `builtin/shaders/` | `Tiny3DStandard.shader` |
| `.tmat` + `.tmat.meta` | `builtin/materials/` | `Tiny3DStandard.tmat` |
| `.tshader` + `.tshader.meta` | `builtin/TempShaders/` | `Tiny3DStandard.tshader` |

## Workflow

### Automatic Sync (Recommended)

Run the sync script to automatically detect and copy all matching files:

```bash
python <skill_dir>/scripts/sync_builtin.py --workspace <workspace_root>
```

The script scans `assets/samples/meshes/` for existing files, finds matching source files in the builtin subdirectories, and copies them over.

### Parameters

| Parameter | Required | Description |
|---|---|---|
| `--workspace` | Yes | Workspace root path (e.g., `D:\private\Tiny3D`) |
| `--dry-run` | No | Preview which files would be copied without actually copying |
| `--names` | No | Comma-separated list of asset names to sync (e.g., `Tiny3DStandard,Test-Material`). If omitted, syncs all matching files found in samples/meshes. |

### Examples

Sync all matching files:
```powershell
python <skill_dir>/scripts/sync_builtin.py --workspace D:\private\Tiny3D
```

Preview changes without copying:
```powershell
python <skill_dir>/scripts/sync_builtin.py --workspace D:\private\Tiny3D --dry-run
```

Sync specific assets only:
```powershell
python <skill_dir>/scripts/sync_builtin.py --workspace D:\private\Tiny3D --names Tiny3DStandard
```

## When to Run

- After running the `builtin-generator` skill to regenerate built-in resources
- After manually modifying any built-in shader, material, or compiled shader files
- When the sample project needs to be updated with the latest built-in assets
- When setting up a fresh samples directory

## Notes

- Only files that already exist in the target directory (`samples/meshes/`) are synced by default, unless `--names` is specified to explicitly request specific assets.
- Each file type (`.shader`, `.tmat`, `.tshader`) is sourced from its specific builtin subdirectory.
- Both the asset file and its corresponding `.meta` file are always copied together.
- The script exits with code 0 on success and non-zero on failure.
