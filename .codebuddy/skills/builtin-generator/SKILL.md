---
name: builtin-generator
description: This skill runs the Tiny3D BuiltinGenerator tool to regenerate all built-in engine resources (textures, shaders, materials, and meshes) into the specified output directory. This skill should be used when the user wants to regenerate builtin resources, rebuild builtin assets, update builtin textures/shaders/materials/meshes, or mentions BuiltinGenerator.exe. Trigger phrases include "generate builtin", "regenerate builtin resources", "run BuiltinGenerator", "rebuild builtin assets", "update builtin shaders/materials/textures/meshes", or any reference to BuiltinGenerator.exe.
---

# Builtin Resource Generator Skill

## Purpose

Run the Tiny3D `BuiltinGenerator.exe` tool to batch-generate all built-in engine resources: textures, shaders, materials, and basic geometry meshes.

## Key Paths

| Resource | Path |
|---|---|
| Generator executable | `source/bin/Windows/Debug/BuiltinGenerator.exe` |
| Default output directory | `assets/editor/builtin` |
| Generated textures | `assets/editor/builtin/textures/` |
| Generated shaders | `assets/editor/builtin/shaders/` |
| Generated materials | `assets/editor/builtin/materials/` |
| Generated meshes | `assets/editor/builtin/meshes/` |
| Generator source code | `source/Tools/BuiltinGenerator/` |

## Usage

### Basic Command

```bash
source/bin/Windows/Debug/BuiltinGenerator.exe <output_path> [-t]
```

### Parameters

| Parameter | Required | Description |
|---|---|---|
| `<output_path>` | Yes | Output directory for generated builtin resources |
| `-t` | No | Reserve temporary folders after generation (useful for debugging) |
| `-?` | No | Display help information |

### Standard Run (with temporary files preserved)

```powershell
D:\private\Tiny3D\source\bin\Windows\Debug\BuiltinGenerator.exe -t D:\private\Tiny3D\assets\editor\builtin
```

### Standard Run (clean, no temporary files)

```powershell
D:\private\Tiny3D\source\bin\Windows\Debug\BuiltinGenerator.exe D:\private\Tiny3D\assets\editor\builtin
```

## Workflow

The generator executes four phases in sequence:

1. **Generate Textures** — Creates built-in texture resources (e.g., `blocks.png`) in `<output>/textures/`
2. **Generate Shaders** — Compiles built-in shader files (e.g., `Default-Material.shader`, `Test-Material.shader`) with all shader variants into `<output>/shaders/`
3. **Generate Materials** — Creates built-in material files (`.tmat`) in `<output>/materials/`
4. **Generate Meshes** — Creates built-in geometry meshes (Cube, Cylinder) in `<output>/meshes/`

## When to Run

- After modifying builtin shader source code (`.cginc` files or shader definitions in `T3DBuiltinShaders.cpp`)
- After modifying builtin material definitions in `T3DBuiltinMaterials.cpp`
- After modifying builtin texture generation logic in `T3DBuiltinTextures.cpp`
- After modifying builtin mesh generation logic (e.g., `T3DBuiltinCube.cpp`)
- When setting up a fresh workspace and builtin assets are missing

## Notes

- The executable must be built first via CMake before running. The Debug build is located at `source/bin/Windows/Debug/BuiltinGenerator.exe`.
- The `-t` flag is recommended during development to keep temporary files for inspection.
- A successful run exits with code 0 and logs "Completed" messages for each generation phase.
- If any phase fails, the tool logs an error but continues to the next phase.
