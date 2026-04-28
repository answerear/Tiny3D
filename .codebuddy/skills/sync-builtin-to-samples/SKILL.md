---
name: sync-builtin-to-samples
description: This skill synchronizes built-in asset files from the builtin directory to the samples/meshes directory. It copies ALL files from builtin/shaders/ (including .shader, .cginc, and their .meta files), plus .tmat and .tshader files from their respective subdirectories. This skill should be used when the user wants to sync builtin files to samples, update sample assets from builtin, copy builtin shaders/materials to samples, or refresh sample meshes directory with builtin resources. Trigger phrases include "sync builtin to samples", "copy builtin to samples", "update samples from builtin", "refresh sample assets", "sync samples meshes", or any mention of copying files from builtin to samples/meshes.
---

# Sync Builtin to Samples Skill

## Purpose

Copy built-in asset files from `assets/editor/builtin/` subdirectories to `assets/samples/meshes/`, overwriting existing files. This ensures the sample project always has the latest built-in resources.

- **Shaders directory**: ALL files are copied (full sync), including `.shader`, `.cginc`, `.meta`, etc.
- **Materials / TempShaders**: Only `.tmat` and `.tshader` files (with `.meta`) are synced by name matching.

## Key Paths

| Resource | Path |
|---|---|
| Builtin shaders (all files) | `assets/editor/builtin/shaders/` |
| Builtin materials | `assets/editor/builtin/materials/` |
| Builtin TempShaders | `assets/editor/builtin/TempShaders/` |
| Target directory | `assets/samples/meshes/` |

## File Mapping

| Source | Sync Mode | File Types | Example |
|---|---|---|---|
| `builtin/shaders/` | **Full directory copy** | `.shader`, `.cginc`, `.meta`, etc. | `Tiny3DStandard.shader`, `Tiny3D.cginc` |
| `builtin/materials/` | By name matching | `.tmat` + `.tmat.meta` | `Tiny3DStandard.tmat` |
| `builtin/TempShaders/` | By name matching | `.tshader` + `.tshader.meta` | `Tiny3DStandard.tshader` |

## Workflow

### Automatic Sync (Recommended)

Run the sync script to copy all builtin shader files and matching materials/compiled shaders:

```bash
python <skill_dir>/scripts/sync_builtin.py --workspace <workspace_root>
```

### Parameters

| Parameter | Required | Description |
|---|---|---|
| `--workspace` | Yes | Workspace root path (e.g., `D:/private/Tiny3D`) |
| `--dry-run` | No | Preview which files would be copied without actually copying |
| `--names` | No | Comma-separated asset names to sync (e.g., `Tiny3DStandard,Test-Material`). Filters shaders by stem name, and materials/TempShaders by base name. If omitted, syncs all. |

### Examples

Sync all files:
```powershell
python <skill_dir>/scripts/sync_builtin.py --workspace D:/private/Tiny3D
```

Preview changes without copying:
```powershell
python <skill_dir>/scripts/sync_builtin.py --workspace D:/private/Tiny3D --dry-run
```

Sync specific assets only:
```powershell
python <skill_dir>/scripts/sync_builtin.py --workspace D:/private/Tiny3D --names Tiny3DStandard
```

## When to Run

- After running the `builtin-generator` skill to regenerate built-in resources
- After manually modifying any built-in shader, material, or compiled shader files
- When the sample project needs to be updated with the latest built-in assets
- When setting up a fresh samples directory

## Notes

- The `builtin/shaders/` directory is fully synced — all files are copied regardless of extension.
- For materials (`.tmat`) and compiled shaders (`.tshader`), only files already present in the target directory are synced by default, unless `--names` is specified.
- When `--names` is used, shaders are filtered by filename stem (part before the first `.`).
- The script exits with code 0 on success and non-zero on failure.
