---
name: deploy-assets-to-debug
description: This skill deploys editor and sample assets to the Debug build output. For samples it first runs bundlebuilder to rebuild assets/samples/bundle from assets/samples/meshes (same as ResourceApp POST_BUILD), then copies assets/editor to source/bin/Windows/Debug/Editor and assets/samples to source/bin/Windows/Debug/Assets/samples. Trigger phrases include "deploy assets", "copy assets to debug", "sync assets to build", "update debug assets", "deploy editor assets", "deploy sample assets", "build samples bundle", "run bundlebuilder then deploy", "copy resources to debug output", or any reference to copying assets into the Debug build directory.
---

# Deploy Assets to Debug Build Skill

## Purpose

Deploy project assets to the Debug build output so the running application loads the latest resources.

For **samples**, the flow matches ResourceApp's POST_BUILD:

1. Run `bundlebuilder` to rebuild `assets/samples/bundle` from `assets/samples/meshes`
2. Copy `assets/samples/` (including the fresh bundle) to the Debug output

Editor assets are copied as-is (no bundle step).

## Copy Mapping

| Source | Destination |
|---|---|
| `assets/editor/` | `source/bin/Windows/Debug/Editor/` |
| `assets/samples/` | `source/bin/Windows/Debug/Assets/samples/` |

## Bundle Step (samples only)

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
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D
```

### Parameters

| Parameter | Required | Default | Description |
|---|---|---|---|
| `--workspace` | Yes | — | Workspace root path (e.g., `D:\private\Tiny3D`) |
| `--target` | No | `all` | What to deploy: `all`, `editor`, `samples` |
| `--clean` | No | Off | Remove destination directories before copying |
| `--dry-run` | No | Off | Preview BundleBuilder command and copy list without executing |
| `--exclude` | No | — | Comma-separated glob patterns to exclude (e.g., `*.fbx,*.FBX`) |
| `--skip-bundle` | No | Off | Skip BundleBuilder; copy existing `assets/samples/bundle` as-is |
| `--bundlebuilder` | No | `source/bin/Windows/Debug/bundlebuilder.exe` | Path to bundlebuilder (workspace-relative or absolute) |
| `--meshes-dir` | No | `assets/samples/meshes` | BundleBuilder `--assets` input (relative to workspace) |
| `--bundle-dir` | No | `assets/samples/bundle` | BundleBuilder `--out` output (relative to workspace) |
| `--keep-languages` | No | — | Pass-through to `bundlebuilder --keep-languages` (e.g. `hlsl,glsl,spirv`) |

### Examples

Deploy all assets (rebuild samples bundle, then copy editor + samples):
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D
```

Deploy only editor assets (no BundleBuilder):
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D --target editor
```

Deploy only sample assets (rebuild bundle, then copy samples):
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D --target samples
```

Copy samples without rebuilding the bundle:
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D --target samples --skip-bundle
```

Rebuild bundle with language pruning, then deploy:
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D --target samples --keep-languages hlsl,glsl,spirv
```

Clean deploy (remove old Debug assets first):
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D --clean
```

Preview without running BundleBuilder or copying:
```powershell
python .codebuddy/skills/deploy-assets-to-debug/scripts/deploy_assets.py --workspace D:\private\Tiny3D --dry-run
```

## Workflow

1. **Validate** — Check workspace and (when deploying samples) that `bundlebuilder.exe` and `assets/samples/meshes` exist
2. **Build bundle** — If target includes `samples` and `--skip-bundle` is not set, run BundleBuilder with cwd = exe directory
3. **Prepare** — Optionally clean destination directories if `--clean` is specified
4. **Copy** — Recursively copy files from source to destination, preserving directory structure
5. **Report** — Print summary of files copied, skipped, or failed

## When to Run

- After running `builtin-generator` / `sync-builtin-to-samples` / `mesh-converter` so meshes are up to date before bundling
- After modifying sample meshes, materials, or shaders that end up in the bundle
- After running `shader-compile-embed` or `compile-builtin-shaders` when those changes feed samples
- Before launching the application from the Debug build to ensure it has the latest resources (including a fresh bundle)

## Prerequisites

- `bundlebuilder.exe` must be built (`source/bin/Windows/Debug/bundlebuilder.exe`) when deploying samples without `--skip-bundle`
- Editor plugins next to the exe must be present (FileSystemArchiveEditor / MetaFSArchiveEditor / NullRendererEditor), same as ResourceApp POST_BUILD
- `assets/samples/meshes/` must exist and contain the assets to pack

## Notes

- This skill uses recursive copy with `dirs_exist_ok` semantics to merge into existing directories.
- Files in the destination that are not in the source are NOT deleted unless `--clean` is used.
- The `--exclude` option uses `fnmatch` patterns to skip files matching the given patterns.
- BundleBuilder failure aborts deploy before any samples copy (fail-fast).
- Exit code 0 on success, non-zero on failure.
