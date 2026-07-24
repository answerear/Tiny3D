---
name: deploy-samples-to-debug
description: This skill deploys ONLY the sample assets to the Debug build output. It first runs bundlebuilder to rebuild assets/samples/bundle from assets/samples/meshes (same as ResourceApp POST_BUILD), then copies assets/samples (including the fresh bundle) to source/bin/Windows/Debug/Assets/samples. Use this when the user wants to deploy just the sample resources without touching editor assets. Trigger phrases include "deploy samples", "deploy sample assets", "copy samples to debug", "update debug samples", "sync samples to build", "build samples bundle", "run bundlebuilder then deploy samples", "deploy only samples", or any reference to copying sample assets into the Debug build directory without editor.
---

# Deploy Sample Assets to Debug Build Skill

## Purpose

Deploy **only** the sample assets to the Debug build output so the running application loads the latest sample resources.

The flow matches ResourceApp's POST_BUILD:

1. Run `bundlebuilder` to rebuild `assets/samples/bundle` from `assets/samples/meshes`
2. Copy `assets/samples/` (including the fresh bundle) to the Debug output

For deploying editor assets, use the `deploy-editor-to-debug` skill. To deploy both at once, use `deploy-assets-to-debug`.

## Copy Mapping

| Source | Destination |
|---|---|
| `assets/samples/` | `source/bin/Windows/Debug/Assets/samples/` |

## Bundle Step

| Item | Path / Value |
|---|---|
| Tool | `source/bin/Windows/Debug/bundlebuilder.exe` |
| Input (`--assets`) | `assets/samples/meshes` |
| Output (`--out`) | `assets/samples/bundle` |
| Working directory | Directory of `bundlebuilder.exe` (required so editor plugins load via `pluginPath="."`) |

Equivalent command:

```powershell
cd source\bin\Windows\Debug
.\bundlebuilder.exe --assets D:\private\Tiny3D\assets\samples\meshes --out D:\private\Tiny3D\assets\samples\bundle
```

## Usage

### Run via Python Script

```powershell
python .codebuddy/skills/deploy-samples-to-debug/scripts/deploy_samples.py --workspace D:\private\Tiny3D
```

### Parameters

| Parameter | Required | Default | Description |
|---|---|---|---|
| `--workspace` | Yes | — | Workspace root path (e.g., `D:\private\Tiny3D`) |
| `--clean` | No | Off | Remove destination directory before copying |
| `--dry-run` | No | Off | Preview BundleBuilder command and copy list without executing |
| `--exclude` | No | — | Comma-separated glob patterns to exclude (e.g., `*.fbx,*.FBX`) |
| `--skip-bundle` | No | Off | Skip BundleBuilder; copy existing `assets/samples/bundle` as-is |
| `--bundlebuilder` | No | `source/bin/Windows/Debug/bundlebuilder.exe` | Path to bundlebuilder (workspace-relative or absolute) |
| `--meshes-dir` | No | `assets/samples/meshes` | BundleBuilder `--assets` input (relative to workspace) |
| `--bundle-dir` | No | `assets/samples/bundle` | BundleBuilder `--out` output (relative to workspace) |
| `--keep-languages` | No | — | Pass-through to `bundlebuilder --keep-languages` (e.g. `hlsl,glsl,spirv`) |

### Examples

Deploy sample assets (rebuild bundle, then copy):
```powershell
python .codebuddy/skills/deploy-samples-to-debug/scripts/deploy_samples.py --workspace D:\private\Tiny3D
```

Copy samples without rebuilding the bundle:
```powershell
python .codebuddy/skills/deploy-samples-to-debug/scripts/deploy_samples.py --workspace D:\private\Tiny3D --skip-bundle
```

Rebuild bundle with language pruning, then deploy:
```powershell
python .codebuddy/skills/deploy-samples-to-debug/scripts/deploy_samples.py --workspace D:\private\Tiny3D --keep-languages hlsl,glsl,spirv
```

Clean deploy (remove old Debug samples first):
```powershell
python .codebuddy/skills/deploy-samples-to-debug/scripts/deploy_samples.py --workspace D:\private\Tiny3D --clean
```

Preview without running BundleBuilder or copying:
```powershell
python .codebuddy/skills/deploy-samples-to-debug/scripts/deploy_samples.py --workspace D:\private\Tiny3D --dry-run
```

## Workflow

1. **Validate** — Check workspace and (unless `--skip-bundle`) that `bundlebuilder.exe` and `assets/samples/meshes` exist
2. **Build bundle** — Unless `--skip-bundle` is set, run BundleBuilder with cwd = exe directory
3. **Prepare** — Optionally clean the destination directory if `--clean` is specified
4. **Copy** — Recursively copy files from `assets/samples` to the Debug samples directory, preserving structure
5. **Report** — Print summary of files copied, skipped, or failed

## When to Run

- After running `builtin-generator` / `sync-builtin-to-samples` / `mesh-converter` so meshes are up to date before bundling
- After modifying sample meshes, materials, or shaders that end up in the bundle
- After running `shader-compile-embed` or `compile-builtin-shaders` when those changes feed samples
- Before launching the application from the Debug build to ensure it has a fresh sample bundle

## Prerequisites

- `bundlebuilder.exe` must be built (`source/bin/Windows/Debug/bundlebuilder.exe`) unless `--skip-bundle` is used
- Editor plugins next to the exe must be present (FileSystemArchiveEditor / MetaFSArchiveEditor / NullRendererEditor), same as ResourceApp POST_BUILD
- `assets/samples/meshes/` must exist and contain the assets to pack

## Notes

- This skill uses recursive copy with merge semantics (files not in the source are NOT deleted unless `--clean` is used).
- The `--exclude` option uses `fnmatch` patterns to skip files matching the given patterns.
- BundleBuilder failure aborts deploy before any copy (fail-fast).
- Exit code 0 on success, non-zero on failure.
