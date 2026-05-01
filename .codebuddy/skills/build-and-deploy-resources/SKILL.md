---
name: build-and-deploy-resources
description: This skill runs the complete Tiny3D resource generation pipeline AND deploys the results to the Debug build directory. It sequentially executes builtin-generator, compile-builtin-shaders, sync-builtin-to-samples, mesh-converter, shader-compile-embed, and deploy-assets-to-debug to regenerate all engine resources and make them available for the running application. Trigger phrases include "build and deploy resources", "rebuild and deploy", "full pipeline with deploy", "generate resources and copy to debug", "build all and deploy", "regenerate and deploy assets", or any request to regenerate all resources and deploy them to the Debug directory.
---

# Build and Deploy Resources Skill

## Purpose

Run the complete Tiny3D resource generation pipeline to regenerate all engine resources from source, then deploy the results to the Debug build directory so the running application can load the latest resources.

## Pipeline Steps

| # | Step | Description | Output |
|---|------|-------------|--------|
| 1 | builtin-generator | Run `BuiltinGenerator.exe` to generate builtin textures, shaders, materials, meshes | `assets/editor/builtin/` |
| 2 | compile-builtin-shaders | Compile `.shader` files to `.tshader` binary format via `scc.exe` | `assets/editor/builtin/TempShaders/` |
| 3 | sync-builtin-to-samples | Sync builtin shader/material/tshader files to `samples/meshes/` | `assets/samples/meshes/` |
| 4 | mesh-converter | Batch convert FBX files from `samples/raw/` to engine format via `mconv.exe` | `assets/samples/meshes/*.tmesh/.tskel/.tani` |
| 5 | shader-compile-embed | Compile `.vshader`/`.pshader` to HLSL/GLSL and embed into `SampleShaders.cpp` | `assets/samples/shaders/output/` + `SampleShaders.cpp` |
| 6 | deploy-assets-to-debug | Copy `assets/editor/` and `assets/samples/` to Debug build output | `source/bin/Windows/Debug/` |

## Data Flow

```
BuiltinGenerator.exe
  → assets/editor/builtin/shaders/*.shader
    → scc.exe → assets/editor/builtin/TempShaders/*.tshader
      → sync → assets/samples/meshes/ (shaders, materials, tshaders)
        → mconv.exe → assets/samples/meshes/*.tmesh/.tskel/.tani (uses builtin materials)
          → scc.exe → HLSL/GLSL → embed → SampleShaders.cpp
            → deploy → source/bin/Windows/Debug/Editor/
                      → source/bin/Windows/Debug/Assets/samples/
```

## Usage

### Run Full Pipeline with Deploy

```powershell
python .codebuddy/skills/build-pipeline/scripts/build_pipeline.py --workspace D:\private\Tiny3D --deploy
```

### Skip Specific Steps

```powershell
python .codebuddy/skills/build-pipeline/scripts/build_pipeline.py --workspace D:\private\Tiny3D --deploy --skip builtin-generator
```

Skip multiple steps:
```powershell
python .codebuddy/skills/build-pipeline/scripts/build_pipeline.py --workspace D:\private\Tiny3D --deploy --skip builtin-generator,compile-builtin-shaders
```

### Parameters

| Parameter | Required | Default | Description |
|---|---|---|---|
| `--workspace` | Yes | — | Workspace root path (e.g., `D:\private\Tiny3D`) |
| `--deploy` | Yes | — | Enable the deploy step (Step 6) |
| `--skip` | No | — | Comma-separated step names to skip |

### Valid Step Names for --skip

- `builtin-generator`
- `compile-builtin-shaders`
- `sync-builtin-to-samples`
- `mesh-converter`
- `shader-compile-embed`
- `deploy-assets-to-debug`

## Deploy Mapping

| Source | Destination |
|---|---|
| `assets/editor/` | `source/bin/Windows/Debug/Editor/` |
| `assets/samples/` | `source/bin/Windows/Debug/Assets/samples/` |

## When to Run

- After modifying any builtin or sample resources and wanting to immediately test in the application
- Before launching the application from the Debug build to ensure it has the latest resources
- After modifying shader source code and wanting to see changes in the running application
- After adding or modifying FBX source models in `assets/samples/raw/`
- When setting up a fresh workspace and need all resources regenerated and ready to run

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
- The deploy step copies ALL files from source directories, preserving directory structure
- If no FBX files exist in `assets/samples/raw/`, the mesh-converter step is gracefully skipped

## Notes

- If you only need to regenerate resources without deploying, use `build-all-resources` instead.
- Individual steps can be run independently using their respective skills.
- The `--skip` option is useful when only some resources have changed (e.g., skip `builtin-generator` if only sample shaders changed).
- The mesh-converter step automatically batch-converts all `.fbx` files in `assets/samples/raw/`.
- After deploy, the Debug directory will have the latest resources and the application can be launched directly.
