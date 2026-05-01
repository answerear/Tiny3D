---
name: build-all-resources
description: This skill runs the complete Tiny3D resource generation pipeline WITHOUT deploying to the Debug build directory. It sequentially executes builtin-generator, compile-builtin-shaders, sync-builtin-to-samples, mesh-converter, and shader-compile-embed to regenerate all engine resources from source. Trigger phrases include "build all resources", "regenerate all resources", "full resource build", "rebuild all assets", "run full pipeline", "generate and compile all", or any request to regenerate all resources without deployment.
---

# Build All Resources Skill

## Purpose

Run the complete Tiny3D resource generation pipeline to regenerate all engine resources from source. This skill orchestrates 5 sub-skills in the correct dependency order, **without** deploying the results to the Debug build directory.

## Pipeline Steps

| # | Step | Description | Output |
|---|------|-------------|--------|
| 1 | builtin-generator | Run `BuiltinGenerator.exe` to generate builtin textures, shaders, materials, meshes | `assets/editor/builtin/` |
| 2 | compile-builtin-shaders | Compile `.shader` files to `.tshader` binary format via `scc.exe` | `assets/editor/builtin/TempShaders/` |
| 3 | sync-builtin-to-samples | Sync builtin shader/material/tshader files to `samples/meshes/` | `assets/samples/meshes/` |
| 4 | mesh-converter | Batch convert FBX files from `samples/raw/` to engine format via `mconv.exe` | `assets/samples/meshes/*.tmesh/.tskel/.tani` |
| 5 | shader-compile-embed | Compile `.vshader`/`.pshader` to HLSL/GLSL and embed into `SampleShaders.cpp` | `assets/samples/shaders/output/` + `SampleShaders.cpp` |

## Data Flow

```
BuiltinGenerator.exe
  → assets/editor/builtin/shaders/*.shader
    → scc.exe → assets/editor/builtin/TempShaders/*.tshader
      → sync → assets/samples/meshes/ (shaders, materials, tshaders)
        → mconv.exe → assets/samples/meshes/*.tmesh/.tskel/.tani (uses builtin materials)
          → scc.exe → HLSL/GLSL → embed → SampleShaders.cpp
```

## Usage

### Run Full Pipeline

```powershell
python .codebuddy/skills/build-pipeline/scripts/build_pipeline.py --workspace D:\private\Tiny3D
```

### Skip Specific Steps

```powershell
python .codebuddy/skills/build-pipeline/scripts/build_pipeline.py --workspace D:\private\Tiny3D --skip builtin-generator
```

Skip multiple steps:
```powershell
python .codebuddy/skills/build-pipeline/scripts/build_pipeline.py --workspace D:\private\Tiny3D --skip builtin-generator,compile-builtin-shaders
```

### Parameters

| Parameter | Required | Default | Description |
|---|---|---|---|
| `--workspace` | Yes | — | Workspace root path (e.g., `D:\private\Tiny3D`) |
| `--skip` | No | — | Comma-separated step names to skip |

### Valid Step Names for --skip

- `builtin-generator`
- `compile-builtin-shaders`
- `sync-builtin-to-samples`
- `mesh-converter`
- `shader-compile-embed`

## When to Run

- After modifying builtin shader source code (`.cginc` or shader definitions in C++)
- After modifying builtin material/texture/mesh generation logic
- After modifying `.vshader` or `.pshader` sample shader source files
- After adding or modifying FBX source models in `assets/samples/raw/`
- When setting up a fresh workspace and need all resources regenerated
- When you want to regenerate resources but are NOT ready to test in the running application

## Prerequisites

- `BuiltinGenerator.exe` must be built (`source/bin/Windows/Debug/BuiltinGenerator.exe`)
- `scc.exe` must be built (`source/bin/Windows/Debug/scc.exe`)
- `mconv.exe` must be built (`source/bin/Windows/Debug/mconv.exe`)
- Python 3.8+

## Behavior

- Steps execute sequentially in dependency order
- **Fail-fast**: If any step fails, the pipeline stops immediately and reports the error
- Each step prints its status and execution time
- A summary is printed at the end showing all step results
- If no FBX files exist in `assets/samples/raw/`, the mesh-converter step is gracefully skipped

## Notes

- This skill does NOT deploy assets to the Debug build directory. Use `build-and-deploy-resources` if you also need deployment.
- Individual steps can be run independently using their respective skills.
- The `--skip` option is useful when only some resources have changed (e.g., skip `builtin-generator` if only sample shaders changed).
- The mesh-converter step automatically batch-converts all `.fbx` files in `assets/samples/raw/`.
