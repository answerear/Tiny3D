/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#ifndef __T3D_TYPEDEF_H__
#define __T3D_TYPEDEF_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    T3D_DECLARE_SMART_PTR(Object);

    T3D_DECLARE_SMART_PTR(Settings);
    
    T3D_DECLARE_SMART_PTR(Image);
    T3D_DECLARE_SMART_PTR(ImageCodec);
    T3D_DECLARE_SMART_PTR(ImageCodecBase);

    T3D_DECLARE_SMART_PTR(Meta);
    T3D_DECLARE_SMART_PTR(Resource);
    T3D_DECLARE_SMART_PTR(ResourceManager);
    T3D_DECLARE_SMART_PTR(Prefab);
    T3D_DECLARE_SMART_PTR(PrefabManager);
    T3D_DECLARE_SMART_PTR(Dylib);
    T3D_DECLARE_SMART_PTR(DylibManager);
    T3D_DECLARE_SMART_PTR(Material);
    T3D_DECLARE_SMART_PTR(MaterialManager);
    T3D_DECLARE_SMART_PTR(Texture);
    T3D_DECLARE_SMART_PTR(TextureManager);
    T3D_DECLARE_SMART_PTR(ShaderConstantParam);
    T3D_DECLARE_SMART_PTR(ShaderSamplerParam);
    T3D_DECLARE_SMART_PTR(Shader);
    T3D_DECLARE_SMART_PTR(ShaderVariant);
    T3D_DECLARE_SMART_PTR(ShaderManager);

    T3D_DECLARE_SMART_PTR(Technique);
    T3D_DECLARE_SMART_PTR(Pass);
    
    T3D_DECLARE_SMART_PTR(RenderQueue);
    T3D_DECLARE_SMART_PTR(RenderPipeline);
    T3D_DECLARE_SMART_PTR(ForwardRenderPipeline);

    T3D_DECLARE_SMART_PTR(RenderResource);
    
    T3D_DECLARE_SMART_PTR(BlendState);
    T3D_DECLARE_SMART_PTR(DepthStencilState);
    T3D_DECLARE_SMART_PTR(RasterizerState);
    T3D_DECLARE_SMART_PTR(SamplerState);
    T3D_DECLARE_SMART_PTR(RenderState);
    T3D_DECLARE_SMART_PTR(TextureState);
    T3D_DECLARE_SMART_PTR(VertexDeclaration);
    T3D_DECLARE_SMART_PTR(RenderBuffer);
    T3D_DECLARE_SMART_PTR(VertexBuffer);
    T3D_DECLARE_SMART_PTR(IndexBuffer);
    T3D_DECLARE_SMART_PTR(PixelBuffer);
    T3D_DECLARE_SMART_PTR(ConstantBuffer);
    
    T3D_DECLARE_SMART_PTR(RenderStateManager);
    T3D_DECLARE_SMART_PTR(RenderBufferManager);

    T3D_DECLARE_SMART_PTR(RenderTarget);
    T3D_DECLARE_SMART_PTR(RenderTexture);
    T3D_DECLARE_SMART_PTR(RenderWindow);
    T3D_DECLARE_SMART_PTR(NullRenderWindow);

    T3D_DECLARE_SMART_PTR(Viewport);
    
    T3D_DECLARE_SMART_PTR(Archive);
    T3D_DECLARE_SMART_PTR(ArchiveManager);

    T3D_DECLARE_SMART_PTR(Serializer);
    T3D_DECLARE_SMART_PTR(BinSerializer);
    T3D_DECLARE_SMART_PTR(JsonSerializer);
    T3D_DECLARE_SMART_PTR(SerializerManager);
    
    T3D_DECLARE_SMART_PTR(RHIRenderer);
    T3D_DECLARE_SMART_PTR(RHIContext);
    T3D_DECLARE_SMART_PTR(RHIResource);
    T3D_DECLARE_SMART_PTR(RHIBlendState);
    T3D_DECLARE_SMART_PTR(RHIDepthStencilState);
    T3D_DECLARE_SMART_PTR(RHIRasterizerState);
    T3D_DECLARE_SMART_PTR(RHISamplerState);
    T3D_DECLARE_SMART_PTR(RHIBuffer);
    T3D_DECLARE_SMART_PTR(RHIVertexBuffer);
    T3D_DECLARE_SMART_PTR(RHIIndexBuffer);
    T3D_DECLARE_SMART_PTR(RHIPixelBuffer);
    T3D_DECLARE_SMART_PTR(RHIConstantBuffer);
    T3D_DECLARE_SMART_PTR(RHIVertexDeclaration);
    T3D_DECLARE_SMART_PTR(RHIRenderTarget);
    T3D_DECLARE_SMART_PTR(RHIRenderWindow);
    T3D_DECLARE_SMART_PTR(RHIRenderTexture);
    T3D_DECLARE_SMART_PTR(RHIShader);
    T3D_DECLARE_SMART_PTR(RHIThread);
    T3D_DECLARE_SMART_PTR(RHICommand);
    

    using RHIRenderers = TMap<String, RHIRendererPtr>;
    using RHIRenderersItr = RHIRenderers::iterator;
    using RHIRenderersConstItr = RHIRenderers::const_iterator;
    using RHIRenderersValue = RHIRenderers::value_type;

    using ShaderLabTags = TMap<String, String>;
    using ShaderLabTagsValue = ShaderLabTags::value_type;
}


#endif  /*__T3D_TYPEDEF_H__*/
