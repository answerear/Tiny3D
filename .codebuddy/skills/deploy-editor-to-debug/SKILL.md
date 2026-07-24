---
name: deploy-editor-to-debug
description: This skill deploys ONLY the editor assets to the Debug build output. It copies assets/editor to source/bin/Windows/Debug/Editor as-is (no bundlebuilder step). Use this when the user wants to deploy just the editor resources without touching samples. Trigger phrases include "deploy editor", "deploy editor assets", "copy editor to debug", "update debug editor assets", "sync editor to build", "deploy only editor", or any reference to copying editor assets into the Debug build directory without samples.
---

# Deploy Editor Assets to Debug Build Skill

## Purpose

Deploy **only** the editor assets to the Debug build output so the running application loads the latest editor resources. Editor assets are copied as-is; there is no bundle step (that only applies to samples).

For deploying samples, use the `deploy-samples-to-debug` skill. To deploy both at once, use `deploy-assets-to-debug`.

## Copy Mapping

| Source | Destination |
|---|---|
| `assets/editor/` | `source/bin/Windows/Debug/Editor/` |

## Usage

### Run via Python Script

```powershell
python .codebuddy/skills/deploy-editor-to-debug/scripts/deploy_editor.py --workspace D:\private\Tiny3D
```

### Parameters

| Parameter | Required | Default | Description |
|---|---|---|---|
| `--workspace` | Yes | — | Workspace root path (e.g., `D:\private\Tiny3D`) |
| `--clean` | No | Off | Remove destination directory before copying |
| `--dry-run` | No | Off | Preview the copy list without executing |
| `--exclude` | No | — | Comma-separated glob patterns to exclude (e.g., `*.fbx,*.FBX`) |

### Examples

Deploy editor assets:
```powershell
python .codebuddy/skills/deploy-editor-to-debug/scripts/deploy_editor.py --workspace D:\private\Tiny3D
```

Clean deploy (remove old Debug editor assets first):
```powershell
python .codebuddy/skills/deploy-editor-to-debug/scripts/deploy_editor.py --workspace D:\private\Tiny3D --clean
```

Preview without copying:
```powershell
python .codebuddy/skills/deploy-editor-to-debug/scripts/deploy_editor.py --workspace D:\private\Tiny3D --dry-run
```

## Workflow

1. **Validate** — Check workspace and that `assets/editor` exists
2. **Prepare** — Optionally clean the destination directory if `--clean` is specified
3. **Copy** — Recursively copy files from `assets/editor` to the Debug `Editor` directory, preserving structure
4. **Report** — Print summary of files copied, skipped, or failed

## When to Run

- After modifying editor resources (icons, editor shaders, editor configs, etc.)
- Before launching the application from the Debug build to ensure it has the latest editor resources

## Notes

- This skill uses recursive copy with merge semantics (files not in the source are NOT deleted unless `--clean` is used).
- The `--exclude` option uses `fnmatch` patterns to skip files matching the given patterns.
- Exit code 0 on success, non-zero on failure.
