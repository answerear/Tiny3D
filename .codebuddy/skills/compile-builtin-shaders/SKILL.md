---
name: compile-builtin-shaders
description: This skill compiles all .shader files under assets/editor/builtin/shaders into .tshader files and places them in assets/editor/builtin/TempShaders. It uses scc.exe (ShaderCrossCompiler) to perform the compilation. Trigger phrases include "compile builtin shaders", "build tshader", "generate tshader from shader", "compile shader to tshader", "run scc on builtin shaders", or any reference to compiling .shader to .tshader in the builtin directory.
---

# Compile Builtin Shaders Skill

## Purpose

Compile all `.shader` source files under the builtin shaders directory into `.tshader` binary format using `scc.exe` (ShaderCrossCompiler), and output the results to the `TempShaders` directory.

## Key Paths

| Resource | Path |
|---|---|
| Shader compiler (scc.exe) | `source/bin/Windows/Debug/scc.exe` |
| Shader source directory | `assets/editor/builtin/shaders/` |
| Output directory | `assets/editor/builtin/TempShaders/` |
| Shader source files | `assets/editor/builtin/shaders/*.shader` |
| Compiled output files | `assets/editor/builtin/TempShaders/*.tshader` |

## Usage

### Run via Python Script

```powershell
python .codebuddy/skills/compile-builtin-shaders/scripts/compile_builtin_shaders.py
```

### Run with Custom Paths

```powershell
python .codebuddy/skills/compile-builtin-shaders/scripts/compile_builtin_shaders.py --scc <scc_path> --input <shader_dir> --output <output_dir>
```

### Parameters

| Parameter | Required | Default | Description |
|---|---|---|---|
| `--scc` | No | `source/bin/Windows/Debug/scc.exe` | Path to scc.exe compiler |
| `--input` | No | `assets/editor/builtin/shaders` | Directory containing .shader source files |
| `--output` | No | `assets/editor/builtin/TempShaders` | Output directory for compiled .tshader files |
| `--clean` | No | Off | Remove output directory before compilation |
| `--target` | No | `hlsl` | Compilation target language (hlsl or glsl) |

## Compilation Details

For each `.shader` file found in the input directory, the script runs:

```
scc.exe <shader_file> -t hlsl -o <output_dir>
```

This produces a corresponding `.tshader` file in the output directory. The compilation target defaults to `hlsl`, matching the engine's BuiltinGenerator behavior.

## Workflow

1. **Verify** — Check that `scc.exe` exists at the expected path
2. **Prepare** — Create or clean the output directory (`TempShaders`)
3. **Compile** — For each `.shader` file in the input directory, invoke `scc.exe` to compile it
4. **Report** — Print compilation results (success/failure count)

## When to Run

- After modifying `.shader` source files under `assets/editor/builtin/shaders/`
- After modifying `.cginc` include files referenced by the shaders
- When you need to verify shader compilation without running the full BuiltinGenerator
- When debugging shader compilation issues in isolation

## Notes

- `scc.exe` must be built first via CMake. The Debug build is at `source/bin/Windows/Debug/scc.exe`.
- The `.tshader` format is the Tiny3D engine's compiled shader binary format.
- This skill only compiles shaders; it does NOT generate materials, textures, or meshes (use `builtin-generator` for the full pipeline).
- A non-zero exit code from `scc.exe` indicates a shader compilation error.
