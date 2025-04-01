/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include <Tiny3D.h>


#if defined (T3D_OS_WINDOWS)

const char *SAMPLE_VERTEX_SHADER =
    "cbuffer type_Tiny3DPerDraw : register(b0)\n"
    "{\n"
    "    row_major float4x4 Tiny3DPerDraw_tiny3d_ObjectToWorld : packoffset(c0);\n"
    "    row_major float4x4 Tiny3DPerDraw_tiny3d_WorldToObject : packoffset(c4);\n"
    "};\n"
    "\n"
    "cbuffer type_Tiny3DPerFrame : register(b1)\n"
    "{\n"
    "    row_major float4x4 Tiny3DPerFrame_tiny3d_MatrixV : packoffset(c0);\n"
    "    row_major float4x4 Tiny3DPerFrame_tiny3d_MatrixP : packoffset(c4);\n"
    "    row_major float4x4 Tiny3DPerFrame_tiny3d_MatrixVP : packoffset(c8);\n"
    "};\n"
    "\n"
    "\n"
    "static float4 gl_Position;\n"
    "static float3 in_var_POSITION;\n"
    "static float2 in_var_TEXCOORD0;\n"
    "static float2 out_var_TEXCOORD0;\n"
    "\n"
    "struct SPIRV_Cross_Input\n"
    "{\n"
    "    float3 in_var_POSITION : POSITION;\n"
    "    float2 in_var_TEXCOORD0 : TEXCOORD0;\n"
    "};\n"
    "\n"
    "struct SPIRV_Cross_Output\n"
    "{\n"
    "    float2 out_var_TEXCOORD0 : TEXCOORD0;\n"
    "    float4 gl_Position : SV_Position;\n"
    "};\n"
    "\n"
    "void vert_main()\n"
    "{\n"
    "    gl_Position = mul(mul(Tiny3DPerFrame_tiny3d_MatrixVP, Tiny3DPerDraw_tiny3d_ObjectToWorld), float4(in_var_POSITION, 1.0f));\n"
    "    out_var_TEXCOORD0 = in_var_TEXCOORD0;\n"
    "}\n"
    "\n"
    "SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)\n"
    "{\n"
    "    in_var_POSITION = stage_input.in_var_POSITION;\n"
    "    in_var_TEXCOORD0 = stage_input.in_var_TEXCOORD0;\n"
    "    vert_main();\n"
    "    SPIRV_Cross_Output stage_output;\n"
    "    stage_output.gl_Position = gl_Position;\n"
    "    stage_output.out_var_TEXCOORD0 = out_var_TEXCOORD0;\n"
    "    return stage_output;\n"
    "}\n";

const char *SAMPLE_PIXEL_SHADER =
    "Texture2D<float4> texCube : register(t0);\n"
    "SamplerState samplertexCube : register(s1);\n"
    "\n"
    "static float2 in_var_TEXCOORD0;\n"
    "static float4 out_var_SV_Target;\n"
    "\n"
    "struct SPIRV_Cross_Input\n"
    "{\n"
    "    float2 in_var_TEXCOORD0 : TEXCOORD0;\n"
    "};\n"
    "\n"
    "struct SPIRV_Cross_Output\n"
    "{\n"
    "    float4 out_var_SV_Target : SV_Target0;\n"
    "};\n"
    "\n"
    "void frag_main()\n"
    "{\n"
    "    out_var_SV_Target = texCube.Sample(samplertexCube, in_var_TEXCOORD0);\n"
    "}\n"
    "\n"
    "SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)\n"
    "{\n"
    "    in_var_TEXCOORD0 = stage_input.in_var_TEXCOORD0;\n"
    "    frag_main();\n"
    "    SPIRV_Cross_Output stage_output;\n"
    "    stage_output.out_var_SV_Target = out_var_SV_Target;\n"
    "    return stage_output;\n"
    "}\n";

const char *SAMPLE_LIT_VERTEX_SHADER =
    "cbuffer type_Tiny3DPerDraw : register(b0)\n"
    "{\n"
    "    row_major float4x4 Tiny3DPerDraw_tiny3d_ObjectToWorld : packoffset(c0);\n"
    "    row_major float4x4 Tiny3DPerDraw_tiny3d_WorldToObject : packoffset(c4);\n"
    "};\n"
    "\n"
    "cbuffer type_Tiny3DPerFrame : register(b1)\n"
    "{\n"
    "    row_major float4x4 Tiny3DPerFrame_tiny3d_MatrixV : packoffset(c0);\n"
    "    row_major float4x4 Tiny3DPerFrame_tiny3d_MatrixP : packoffset(c4);\n"
    "    row_major float4x4 Tiny3DPerFrame_tiny3d_MatrixVP : packoffset(c8);\n"
    "};\n"
    "\n"
    "\n"
    "static float4 gl_Position;\n"
    "static float3 in_var_POSITION;\n"
    "static float3 in_var_NORMAL;\n"
    "static float2 in_var_TEXCOORD0;\n"
    "static float2 out_var_TEXCOORD0;\n"
    "\n"
    "struct SPIRV_Cross_Input\n"
    "{\n"
    "    float3 in_var_POSITION : POSITION;\n"
    "    float3 in_var_NORMAL : NORMAL;\n"
    "    float2 in_var_TEXCOORD0 : TEXCOORD0;\n"
    "};\n"
    "\n"
    "struct SPIRV_Cross_Output\n"
    "{\n"
    "    float2 out_var_TEXCOORD0 : TEXCOORD0;\n"
    "    float4 gl_Position : SV_Position;\n"
    "};\n"
    "\n"
    "void vert_main()\n"
    "{\n"
    "    gl_Position = mul(mul(Tiny3DPerFrame_tiny3d_MatrixVP, Tiny3DPerDraw_tiny3d_ObjectToWorld), float4(in_var_POSITION, 1.0f));\n"
    "    out_var_TEXCOORD0 = in_var_TEXCOORD0;\n"
    "}\n"
    "\n"
    "SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)\n"
    "{\n"
    "    in_var_POSITION = stage_input.in_var_POSITION;\n"
    "    in_var_NORMAL = stage_input.in_var_NORMAL;\n"
    "    in_var_TEXCOORD0 = stage_input.in_var_TEXCOORD0;\n"
    "    vert_main();\n"
    "    SPIRV_Cross_Output stage_output;\n"
    "    stage_output.gl_Position = gl_Position;\n"
    "    stage_output.out_var_TEXCOORD0 = out_var_TEXCOORD0;\n"
    "    return stage_output;\n"
    "}\n";

const char *SAMPLE_LIT_PIXEL_SHADER =
    "cbuffer type_Globals : register(b2)\n"
    "{\n"
    "    float4 _Globals_tiny3d_AmbientLight : packoffset(c0);\n"
    "};\n"
    "\n"
    "Texture2D<float4> texCube : register(t0);\n"
    "SamplerState samplertexCube : register(s1);\n"
    "\n"
    "static float2 in_var_TEXCOORD0;\n"
    "static float4 out_var_SV_Target;\n"
    "\n"
    "struct SPIRV_Cross_Input\n"
    "{\n"
    "    float2 in_var_TEXCOORD0 : TEXCOORD0;\n"
    "};\n"
    "\n"
    "struct SPIRV_Cross_Output\n"
    "{\n"
    "    float4 out_var_SV_Target : SV_Target0;\n"
    "};\n"
    "\n"
    "void frag_main()\n"
    "{\n"
    "    out_var_SV_Target = texCube.Sample(samplertexCube, in_var_TEXCOORD0) * float4(_Globals_tiny3d_AmbientLight.xyz * _Globals_tiny3d_AmbientLight.w, 1.0f);\n"
    "}\n"
    "\n"
    "SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)\n"
    "{\n"
    "    in_var_TEXCOORD0 = stage_input.in_var_TEXCOORD0;\n"
    "    frag_main();\n"
    "    SPIRV_Cross_Output stage_output;\n"
    "    stage_output.out_var_SV_Target = out_var_SV_Target;\n"
    "    return stage_output;\n"
    "}\n";

#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_IOS)
#elif defined (T3D_OS_ANDROID)
#elif defined (T3D_OS_LINUX)
#endif
