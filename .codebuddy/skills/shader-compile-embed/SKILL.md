---
name: shader-compile-embed
description: This skill handles the complete workflow of compiling Tiny3D shader source files (.vshader and .pshader) into HLSL and GLSL using scc.exe, and embedding the compiled shader code into C++ string constants in SampleShaders.cpp. This skill should be used when the user wants to compile shaders, update shader strings in C++ code, regenerate HLSL or GLSL output, or synchronize shader changes into the engine source. Trigger phrases include "compile shaders", "update SampleShaders", "regenerate HLSL/GLSL", "embed shaders into C++", "sync shader code", or any mention of scc.exe with shader files.
---

# Shader Compile & Embed Skill

## Purpose

Automate the two-step workflow for Tiny3D shader development:
1. **Compile** `.vshader` and `.pshader` source files into HLSL and GLSL using `scc.exe`
2. **Embed** the compiled shader text into C++ string constants in `SampleShaders.cpp`

## Key Paths

| Resource | Path |
|---|---|
| Shader compiler | `source/bin/Windows/Debug/scc.exe` |
| Shader sources | `assets/samples/shaders/*.vshader`, `*.pshader` |
| HLSL output | `assets/samples/shaders/output/HLSL/` |
| GLSL output | `assets/samples/shaders/output/OpenGL4/` |
| C++ target | `source/Samples/Common/SampleShaders.cpp` |

## Workflow

### Step 1: Compile Shaders

Run `scripts/compile_shaders.py` to batch-compile all shader source files:

```bash
python <skill_dir>/scripts/compile_shaders.py \
    --scc source/bin/Windows/Debug/scc.exe \
    --src assets/samples/shaders \
    --hlsl-out assets/samples/shaders/output/HLSL \
    --glsl-out assets/samples/shaders/output/OpenGL4 \
    --extra-args "-N -O0"
```

This finds all `.vshader` and `.pshader` files and compiles each with both `-t hlsl` and `-t glsl` targets. The script automatically adds platform-specific defines: `-D TINY3D_DIRECTX` for HLSL and `-D TINY3D_OPENGL` for GLSL.

### Step 2: Embed into C++

Run `scripts/embed_shaders.py` to replace the C++ string constants with the newly compiled shader content:

```bash
python <skill_dir>/scripts/embed_shaders.py \
    --cpp source/Samples/Common/SampleShaders.cpp \
    --hlsl-dir assets/samples/shaders/output/HLSL \
    --glsl-dir assets/samples/shaders/output/OpenGL4 \
    --map "GeometryApp_vertex.hlsl:SAMPLE_VERTEX_SHADER" \
    --map "GeometryApp_fragment.hlsl:SAMPLE_PIXEL_SHADER" \
    --map "LitGeometryApp_vertex.hlsl:SAMPLE_LIT_VERTEX_SHADER" \
    --map "LitGeometryApp_fragment.hlsl:SAMPLE_LIT_PIXEL_SHADER" \
    --map "ShadowPass_vertex.hlsl:SHADOW_VERTEX_SHADER" \
    --map "ForwardPass_vertex.hlsl:FORWARD_VERTEX_SHADER" \
    --map "ForwardPass_fragment.hlsl:FORWARD_PIXEL_SHADER" \
    --map "SkinShadowPass_vertex.hlsl:SKIN_SHADOW_VERTEX_SHADER" \
    --map "SkinForwardPass_vertex.hlsl:SKIN_FORWARD_VERTEX_SHADER" \
    --map "GPUSkinShadowPass_vertex.hlsl:GPU_SKIN_SHADOW_VERTEX_SHADER" \
    --map "GPUSkinForwardPass_vertex.hlsl:GPU_SKIN_FORWARD_VERTEX_SHADER"
```

The `--map` parameter uses the format `<hlsl_filename>:<CPP_VARIABLE>`. The script automatically:
- Replaces the HLSL variable (no suffix) with content from `--hlsl-dir`
- Replaces the GLSL variable (appends `_GL` suffix) with content from `--glsl-dir`

### Mapping Convention

- **HLSL variables**: No suffix (e.g., `SAMPLE_VERTEX_SHADER`)
- **GLSL variables**: `_GL` suffix (e.g., `SAMPLE_VERTEX_SHADER_GL`)
- All variables are defined inside `#if defined (T3D_OS_WINDOWS)` block in `SampleShaders.cpp`

### C++ String Format

Each shader line is converted to a C++ string literal:
```cpp
const char *VARIABLE_NAME =
    "first line of shader\n"
    "second line of shader\n"
    ...
    "last line\n";
```

## Reference

For the complete mapping table between compiled files and C++ variables, see `references/shader_mapping.md`.

## Adding New Shaders

To add a new shader to the pipeline:
1. Create the `.vshader` or `.pshader` source file in `assets/samples/shaders/`
2. Add the corresponding `const char *` variable declarations in `SampleShaders.cpp` (both HLSL and GLSL versions)
3. Add a new `--map` entry to the embed command
4. Update `references/shader_mapping.md` with the new mapping
