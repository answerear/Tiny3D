---
name: shader-compile-embed
description: This skill handles the complete workflow of compiling Tiny3D shader source files (.vshader and .pshader) into HLSL, GLSL, SPIR-V, and ESSL (GLES3) using scc.exe, and embedding the compiled shader code into C++ constants in SampleShaders.cpp. This skill should be used when the user wants to compile shaders, update shader strings in C++ code, regenerate HLSL/GLSL/ESSL/SPIR-V output, or synchronize shader changes into the engine source. Trigger phrases include "compile shaders", "update SampleShaders", "regenerate HLSL/GLSL/ESSL", "compile SPIR-V", "compile GLES3 shaders", "embed shaders into C++", "sync shader code", or any mention of scc.exe with shader files.
---

# Shader Compile & Embed Skill

## Purpose

Automate the workflow for Tiny3D shader development:
1. **Compile** `.vshader` and `.pshader` source files into HLSL, GLSL, SPIR-V, and ESSL (GLES3) using `scc.exe`
2. **Embed HLSL/GLSL/ESSL** compiled shader text into C++ string constants in per-platform `.h` files
3. **Embed SPIR-V** compiled binary bytecode into C++ byte array constants in `SampleShaders_vk.h`

## Key Paths

| Resource | Path |
|---|---|
| Shader compiler | `source/bin/Windows/Debug/scc.exe` |
| Shader sources | `assets/samples/shaders/*.vshader`, `*.pshader` |
| HLSL output | `assets/samples/shaders/output/HLSL/` |
| GLSL output | `assets/samples/shaders/output/OpenGL4/` |
| ESSL output | `assets/samples/shaders/output/OpenGLES3/` |
| SPIR-V output | `assets/samples/shaders/output/Vulkan/` |
| C++ HLSL target | `source/Samples/Common/SampleShaders_hlsl.h` |
| C++ GL4 target | `source/Samples/Common/SampleShaders_gl4.h` |
| C++ GLES3 target | `source/Samples/Common/SampleShaders_gles3.h` |
| C++ VK target | `source/Samples/Common/SampleShaders_vk.h` |
| C++ entry point | `source/Samples/Common/SampleShaders.cpp` (conditionally includes the above per platform) |

## Workflow

### Step 1: Compile Shaders

Run `scripts/compile_shaders.py` to batch-compile all shader source files:

```bash
python <skill_dir>/scripts/compile_shaders.py \
    --scc source/bin/Windows/Debug/scc.exe \
    --src assets/samples/shaders \
    --hlsl-out assets/samples/shaders/output/HLSL \
    --glsl-out assets/samples/shaders/output/OpenGL4 \
    --spirv-out assets/samples/shaders/output/Vulkan \
    --essl-out assets/samples/shaders/output/OpenGLES3 \
    --extra-args "-N -O0"
```

This finds all `.vshader` and `.pshader` files and compiles each with `-t hlsl`, `-t glsl`, `-t spirv`, and `-t essl` targets. The script automatically adds platform-specific defines:
- `-D TINY3D_DIRECTX` for HLSL
- `-D TINY3D_OPENGL` for GLSL
- `-D TINY3D_VULKAN` for SPIR-V
- `-D TINY3D_OPENGLES` for ESSL (GLES3)

### Step 2: Embed HLSL/GLSL/ESSL into C++

Run `scripts/embed_shaders.py` to replace the C++ string constants with the newly compiled shader content. Each platform's shaders are now stored in separate `.h` files:

```bash
python <skill_dir>/scripts/embed_shaders.py \
    --hlsl-cpp source/Samples/Common/SampleShaders_hlsl.h \
    --glsl-cpp source/Samples/Common/SampleShaders_gl4.h \
    --essl-cpp source/Samples/Common/SampleShaders_gles3.h \
    --hlsl-dir assets/samples/shaders/output/HLSL \
    --glsl-dir assets/samples/shaders/output/OpenGL4 \
    --essl-dir assets/samples/shaders/output/OpenGLES3 \
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
- Replaces the HLSL variable (no suffix) in `--hlsl-cpp` with content from `--hlsl-dir`
- Replaces the GLSL variable (appends `_GL` suffix) in `--glsl-cpp` with content from `--glsl-dir`
- Replaces the ESSL variable (appends `_GLES` suffix) in `--essl-cpp` with content from `--essl-dir`

### Step 3: Embed SPIR-V into C++

Run `scripts/embed_spirv.py` to replace the C++ byte array constants with the newly compiled SPIR-V bytecode:

```bash
python <skill_dir>/scripts/embed_spirv.py \
    --cpp source/Samples/Common/SampleShaders_vk.h \
    --spirv-dir assets/samples/shaders/output/Vulkan \
    --map "GeometryApp_vertex.spirv:SAMPLE_VERTEX_SHADER_VK" \
    --map "GeometryApp_fragment.spirv:SAMPLE_PIXEL_SHADER_VK" \
    --map "LitGeometryApp_vertex.spirv:SAMPLE_LIT_VERTEX_SHADER_VK" \
    --map "LitGeometryApp_fragment.spirv:SAMPLE_LIT_PIXEL_SHADER_VK" \
    --map "ShadowPass_vertex.spirv:SHADOW_VERTEX_SHADER_VK" \
    --map "ForwardPass_vertex.spirv:FORWARD_VERTEX_SHADER_VK" \
    --map "ForwardPass_fragment.spirv:FORWARD_PIXEL_SHADER_VK" \
    --map "SkinShadowPass_vertex.spirv:SKIN_SHADOW_VERTEX_SHADER_VK" \
    --map "SkinForwardPass_vertex.spirv:SKIN_FORWARD_VERTEX_SHADER_VK" \
    --map "GPUSkinShadowPass_vertex.spirv:GPU_SKIN_SHADOW_VERTEX_SHADER_VK" \
    --map "GPUSkinForwardPass_vertex.spirv:GPU_SKIN_FORWARD_VERTEX_SHADER_VK"
```

The `--map` parameter uses the format `<spirv_filename>:<CPP_VARIABLE>`. For each mapping, the script generates:
- `alignas(4) const unsigned char <VAR>[] = { 0x03, 0x02, ... };`
- `const size_t <VAR>_SIZE = sizeof(<VAR>);`

### Mapping Convention

- **HLSL variables**: No suffix (e.g., `SAMPLE_VERTEX_SHADER`) — defined in `SampleShaders_hlsl.h`
- **GLSL variables**: `_GL` suffix (e.g., `SAMPLE_VERTEX_SHADER_GL`) — defined in `SampleShaders_gl4.h`
- **ESSL variables**: `_GLES` suffix (e.g., `SAMPLE_VERTEX_SHADER_GLES`) — defined in `SampleShaders_gles3.h`
- **SPIR-V variables**: `_VK` suffix (e.g., `SAMPLE_VERTEX_SHADER_VK`) + `_VK_SIZE` for byte count — defined in `SampleShaders_vk.h`

Platform inclusion is controlled by `SampleShaders.cpp` via conditional `#include`:
- Windows: includes `_hlsl.h` + `_gl4.h` + `_vk.h`
- Android: includes `_gles3.h` + `_vk.h`

### C++ String Format (HLSL/GLSL/ESSL)

Each shader line is converted to a C++ string literal:
```cpp
const char *VARIABLE_NAME =
    "first line of shader\n"
    "second line of shader\n"
    ...
    "last line\n";
```

### C++ Byte Array Format (SPIR-V)

SPIR-V binary is embedded as an aligned byte array:
```cpp
alignas(4) const unsigned char VARIABLE_NAME_VK[] = {
    0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00,
    ...
};
const size_t VARIABLE_NAME_VK_SIZE = sizeof(VARIABLE_NAME_VK);
```

The `alignas(4)` ensures the array satisfies Vulkan's 4-byte alignment requirement for `VkShaderModuleCreateInfo::pCode`.

## Reference

For the complete mapping table between compiled files and C++ variables, see `references/shader_mapping.md`.

## Adding New Shaders

To add a new shader to the pipeline:
1. Create the `.vshader` or `.pshader` source file in `assets/samples/shaders/`
2. Add the corresponding `const char *` variable in the appropriate platform header:
   - HLSL variable in `SampleShaders_hlsl.h`
   - GLSL variable (with `_GL` suffix) in `SampleShaders_gl4.h`
   - ESSL variable (with `_GLES` suffix) in `SampleShaders_gles3.h`
   - SPIR-V array (with `_VK` suffix) in `SampleShaders_vk.h`
3. Add a new `--map` entry to the embed commands (both `embed_shaders.py` and `embed_spirv.py`)
4. Update `references/shader_mapping.md` with the new mapping
