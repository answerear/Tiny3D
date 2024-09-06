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


#include "T3DBuiltinMesh.h"
#include "T3DBuiltinMaterials.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

#if !defined (GENERATE_TEST_MESH)
    const char *MATERIAL_NAME = "Default-Material.tmat";
#else
    const char *MATERIAL_NAME = "Test-Material.tmat";
#endif
    
    //--------------------------------------------------------------------------

    MaterialPtr BuiltinMesh::buildMaterial()
    {
#if 0
        TResult ret;

        // vertex & pixel shader keyword
        ShaderKeyword vkeyword;
        vkeyword.addKeyword("-");
        vkeyword.generate();
        ShaderKeyword pkeyword(vkeyword);
        
        // vertex shader
        const String vs =
            "//cbuffer ConstantBuffer : register(b0)\n"
            "//{\n"
            "//   float4x4 modelMatrix;\n"
            "//   float4x4 viewMatrix;\n"
            "//   float4x4 projectionMatrix;\n"
            "//}\n"
            "cbuffer Tiny3DPerDraw : register(b0)\n"
            "{\n"
            "   row_major float4x4 tiny3d_ObjectToWorld;\n"
            "   row_major float4x4 tiny3d_WorldToObject;\n"
            "}\n"
            "\n"
            "cbuffer Tiny3DPerFrame : register(b1)\n"
            "{\n"
            "   row_major float4x4 tiny3d_MatrixV;\n"
            "   row_major float4x4 tiny3d_MatrixP;\n"
            "   row_major float4x4 tiny3d_MatrixVP;\n"
            "}\n"
            "\n"
            "static float4x4 tiny3d_MatrixMVP = mul(tiny3d_MatrixVP, tiny3d_ObjectToWorld);\n"
            "static float4x4 tiny3d_MatrixMV = mul(tiny3d_MatrixV, tiny3d_ObjectToWorld);\n"
            "\n"
            "#define TINY3D_MATRIX_M    tiny3d_ObjectToWorld\n"
            "#define TINY3D_MATRIX_V    tiny3d_MatrixV\n"
            "#define TINY3D_MATRIX_P    tiny3d_MatrixP\n"
            "#define TINY3D_MATRIX_VP   tiny3d_MatrixVP\n"
            "#define TINY3D_MATRIX_MV   tiny3d_MatrixMV\n"
            "#define TINY3D_MATRIX_MVP  tiny3d_MatrixMVP\n"
            "\n"
            "struct VertexInput\n"
            "{\n"
            "   float3 position : POSITION;\n"
            "   float2 uv : TEXCOORD0;\n"
            "};\n"
            "struct VertexOutput\n"
            "{\n"
            "   float4 position : SV_POSITION;\n"
            "   float2 uv : TEXCOORD0;\n"
            "};\n"
            "VertexOutput main(VertexInput input)\n"
            "{\n"
            "   VertexOutput output;\n"
            "   //float4 worldPosition = mul(float4(input.position, 1.0), modelMatrix);\n"
            "   //float4 viewPosition = mul(viewMatrix, worldPosition);\n"
            "   //float4 clipPosition = mul(projectionMatrix, viewPosition);\n"
            "   //output.position = clipPosition;\n"
            "   //row_major float4x4 MVP = mul(tiny3d_MatrixVP, tiny3d_ObjectToWorld);\n"
            "   output.position = mul(TINY3D_MATRIX_MVP, float4(input.position, 1.0f));\n"
            "   //output.position = float4(input.position, 1.0);\n"
            "   output.uv = input.uv;\n"
            "   return output;\n"
            "}\n";
        
        ShaderVariantPtr vshader = ShaderVariant::create(std::move(vkeyword), vs);
        vshader->setShaderStage(SHADER_STAGE::kVertex);

        // pixel shader
        const String ps =
            "#define CONCATENATE(a, b) a##b\n"
            "#define TEX2D(name) Texture2D name; SamplerState CONCATENATE(sampler, name);\n"
            "#define TEX2D_R(name, r) Texture2D name : register(CONCATENATE(t, r)); SamplerState CONCATENATE(sampler, name) : register(CONCATENATE(s, r));\n"
            "#define SAMPLE(tex, uv) tex.Sample(CONCATENATE(sampler, tex), uv);\n"
            "//Texture2D texCube : register(t0);\n"
            "//SamplerState samplertexCube : register(s0);\n"
            "TEX2D(texCube);\n"
            "struct PS_INPUT\n"
            "{\n"
            "    float4 position : SV_POSITION;\n"
            "    float2 uv : TEXCOORD0;\n"
            "};\n"
            "float4 main(PS_INPUT input) : SV_Target\n"
            "{\n"
            "    //float4 color = texCube.Sample(sampler_texCube, input.uv);\n"
            "    float4 color = SAMPLE(texCube, input.uv);\n"
            "    //float4 color = float4(0.15f, 0.5f, 1.0f, 1.0f);\n"
            "    return color;\n"
            "}";
        
        ShaderVariantPtr pshader = ShaderVariant::create(std::move(pkeyword), ps);
        pshader->setShaderStage(SHADER_STAGE::kPixel);

        // render state
        RenderStatePtr renderState = RenderState::create();

        // blend state
        BlendDesc blendDesc;
        renderState->setBlendDesc(blendDesc);

        // depth & stencil state
        DepthStencilDesc depthStencilDesc;
        renderState->setDepthStencilDesc(depthStencilDesc);

        // rasterizer state
        RasterizerDesc rasterizeDesc;
        renderState->setRasterizerDesc(rasterizeDesc);
        
        // pass
        PassPtr pass = Pass::create("0");
        ret = pass->addShaderVariant(vshader->getShaderKeyword(), vshader);
        T3D_ASSERT(T3D_SUCCEEDED(ret));
        ret = pass->addShaderVariant(pshader->getShaderKeyword(), pshader);
        pass->setRenderState(renderState);
        T3D_ASSERT(T3D_SUCCEEDED(ret));

        // technique
        TechniquePtr tech = Technique::create("Default-Technique");
        bool rval = tech->addPass(pass);
        T3D_ASSERT(rval);

        // shader
        ShaderPtr shader = T3D_SHADER_MGR.createShader("Default-Shader");
        rval = shader->addTechnique(tech);
        T3D_ASSERT(rval);
        
        // samplers
        // ShaderSamplerParams samplers;
        // const String texSamplerName = "texCube";
        // Texture2DPtr texture = buildTexture();
        // // sampler state
        // SamplerDesc samplerDesc;
        // texture->setSamplerDesc(samplerDesc);
        // ShaderSamplerParamPtr sampler = ShaderSamplerParam::create(texSamplerName, TEXTURE_TYPE::TT_2D, texture);
        // shader->addSamplerParam(sampler);
        
        // material
        MaterialPtr material = T3D_MATERIAL_MGR.createMaterial("Default-Material", shader);
        StringArray enableKeywrods;
        enableKeywrods.push_back("-");
        StringArray disableKeywords;
        material->switchKeywords(enableKeywrods, disableKeywords);
        
        return material;
#else
        String name = MATERIAL_NAME;
        MaterialPtr material = T3D_BUILTIN_MATERIALS.getMaterial(name);
        StringArray enableKeywrods;
        enableKeywrods.push_back("");
        StringArray disableKeywords;
        material->switchKeywords(enableKeywrods, disableKeywords);
        return material;
#endif
    }

    //--------------------------------------------------------------------------
}
