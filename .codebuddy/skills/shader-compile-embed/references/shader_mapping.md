# Tiny3D Shader Mapping Reference

## Shader Source Files

All shader source files are located in `assets/samples/shaders/`:

| Source File | Type | Description |
|---|---|---|
| `GeometryApp.vshader` | Vertex | Basic geometry vertex shader |
| `GeometryApp.pshader` | Pixel | Basic geometry pixel shader |
| `LitGeometryApp.vshader` | Vertex | Lit geometry vertex shader |
| `LitGeometryApp.pshader` | Pixel | Lit geometry pixel shader |
| `ShadowPass.vshader` | Vertex | Shadow pass vertex shader |
| `ForwardPass.vshader` | Vertex | Forward pass vertex shader |
| `ForwardPass.pshader` | Pixel | Forward pass pixel shader |
| `SkinShadowPass.vshader` | Vertex | Skinned mesh shadow pass |
| `SkinForwardPass.vshader` | Vertex | Skinned mesh forward pass |
| `GPUSkinShadowPass.vshader` | Vertex | GPU skinning shadow pass |
| `GPUSkinForwardPass.vshader` | Vertex | GPU skinning forward pass |

## Compiled Output Files

After compilation with `scc.exe`, each shader produces two files:

- **HLSL** output: `output/HLSL/<ShaderName>_vertex.hlsl` or `<ShaderName>_fragment.hlsl`
- **GLSL** output: `output/OpenGL4/<ShaderName>_vertex.glsl` or `<ShaderName>_fragment.glsl`

## C++ Variable Mapping in SampleShaders.cpp

The file `source/Samples/Common/SampleShaders.cpp` contains all shader code as C++ string constants, wrapped inside `#if defined (T3D_OS_WINDOWS)`.

### HLSL Variables (no suffix)

| Compiled File | C++ Variable |
|---|---|
| `GeometryApp_vertex.hlsl` | `SAMPLE_VERTEX_SHADER` |
| `GeometryApp_fragment.hlsl` | `SAMPLE_PIXEL_SHADER` |
| `LitGeometryApp_vertex.hlsl` | `SAMPLE_LIT_VERTEX_SHADER` |
| `LitGeometryApp_fragment.hlsl` | `SAMPLE_LIT_PIXEL_SHADER` |
| `ShadowPass_vertex.hlsl` | `SHADOW_VERTEX_SHADER` |
| `ForwardPass_vertex.hlsl` | `FORWARD_VERTEX_SHADER` |
| `ForwardPass_fragment.hlsl` | `FORWARD_PIXEL_SHADER` |
| `SkinShadowPass_vertex.hlsl` | `SKIN_SHADOW_VERTEX_SHADER` |
| `SkinForwardPass_vertex.hlsl` | `SKIN_FORWARD_VERTEX_SHADER` |
| `GPUSkinShadowPass_vertex.hlsl` | `GPU_SKIN_SHADOW_VERTEX_SHADER` |
| `GPUSkinForwardPass_vertex.hlsl` | `GPU_SKIN_FORWARD_VERTEX_SHADER` |

### GLSL Variables (_GL suffix)

| Compiled File | C++ Variable |
|---|---|
| `GeometryApp_vertex.glsl` | `SAMPLE_VERTEX_SHADER_GL` |
| `GeometryApp_fragment.glsl` | `SAMPLE_PIXEL_SHADER_GL` |
| `LitGeometryApp_vertex.glsl` | `SAMPLE_LIT_VERTEX_SHADER_GL` |
| `LitGeometryApp_fragment.glsl` | `SAMPLE_LIT_PIXEL_SHADER_GL` |
| `ShadowPass_vertex.glsl` | `SHADOW_VERTEX_SHADER_GL` |
| `ForwardPass_vertex.glsl` | `FORWARD_VERTEX_SHADER_GL` |
| `ForwardPass_fragment.glsl` | `FORWARD_PIXEL_SHADER_GL` |
| `SkinShadowPass_vertex.glsl` | `SKIN_SHADOW_VERTEX_SHADER_GL` |
| `SkinForwardPass_vertex.glsl` | `SKIN_FORWARD_VERTEX_SHADER_GL` |
| `GPUSkinShadowPass_vertex.glsl` | `GPU_SKIN_SHADOW_VERTEX_SHADER_GL` |
| `GPUSkinForwardPass_vertex.glsl` | `GPU_SKIN_FORWARD_VERTEX_SHADER_GL` |

## scc.exe Parameters

| Parameter | Description |
|---|---|
| `-t hlsl` | Target HLSL output |
| `-t glsl` | Target GLSL output |
| `-o <dir>` | Output directory |
| `-N` | No optimization names |
| `-O0` | Optimization level 0 |
| `-D TINY3D_DIRECTX` | Define macro for DirectX/HLSL path |
| `-D TINY3D_OPENGL` | Define macro for OpenGL/GLSL path |

Default compile commands:
```
scc.exe <input> -t hlsl -o <output_dir> -N -O0 -D TINY3D_DIRECTX
scc.exe <input> -t glsl -o <output_dir> -N -O0 -D TINY3D_OPENGL
```
