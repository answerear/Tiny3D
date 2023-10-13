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

#ifndef __T3D_PREREQUISITES_H__
#define __T3D_PREREQUISITES_H__


#include "T3DMacro.h"

#if defined T3DCORE_EXPORT
    #define T3D_ENGINE_API        T3D_EXPORT_API
#else
    #define T3D_ENGINE_API        T3D_IMPORT_API
#endif


#include <T3DPlatformLib.h>
#include <T3DSystemLib.h>
#include <T3DMathLib.h>
#include <T3DLog.h>
#include <T3DUtils.h>
#include <T3DFramework.h>
#include <rttr/registration>
#include <rttr/registration_friend>


#if defined (T3D_OS_IOS) || defined (T3D_OS_OSX)
#include <SDL_main.h>
#else
#endif

#include <algorithm>
#undef min

namespace Tiny3D
{
    #define LOG_TAG_CONFIG_FILE         "ConfigFile"
    #define LOG_TAG_ENGINE              "Agent"
    #define LOG_TAG_IMAGE               "Image"
    #define LOG_TAG_RENDER              "Render"
    #define LOG_TAG_PLUGIN              "Plugin"
    #define LOG_TAG_IMAGE_CODEC         "ImageCodec"
    #define LOG_TAG_SCENE               "Scene"
    #define LOG_TAG_RESOURCE            "Resource"
    #define LOG_TAG_COMPONENT           "Component"
    #define LOG_TAG_SERIALIZE           "Serialize"

    #define TXT_DESCRIPTION             "Description"

    class Object;
    class ObjectTracer;

    class Agent;
    class Plugin;
    class Archive;
    class ArchiveManager;

    struct Settings;

    class Serializer;
    class BinSerializer;
    class JsonSerializer;
    class SerializerManager;

    class Image;
    class ImageCodec;
    class ImageCodecBase;

    class Meta;
    class Resource;
    class ResourceManager;
    class Dylib;
    class DylibManager;
    class Material;
    class MaterialManager;
    class Texture;
    class TextureManager;
    class Prefab;
    class PrefabManager;
    class ShaderConstantParam;
    class ShaderSamplerParam;
    class Shader;
    class ShaderVariant;
    class ShaderManager;

    class Technique;
    class Pass;

    class RenderQueue;
    class RenderPipeline;
    class ForwardRenderPipeline;

    class RenderResource;
    
    class BlendState;
    class DepthStencilState;
    class RasterizerState;
    class SamplerState;
    class RenderState;
    class TextureState;
    class VertexDeclaration;
    class RenderBuffer;
    class VertexBuffer;
    class IndexBuffer;
    class PixelBuffer;
    class ConstantBuffer;

    class RenderStateManager;
    class RenderBufferManager;
    
    class RenderTarget;
    class RenderTexture;
    class RenderWindow;
    class NullRenderWindow;
    class Viewport;
    
    class RHIRenderer;
    class RHIContext;
    class RHIResource;
    class RHIBlendState;
    class RHIDepthStencilState;
    class RHIRasterizerState;
    class RHISamplerState;
    class RHIBuffer;
    class RHIVertexBuffer;
    class RHIIndexBuffer;
    class RHIPixelBuffer;
    class RHIConstantBuffer;
    class RHIVertexDeclaration;
    class RHIRenderTarget;
    class RHIRenderWindow;
    class RHIRenderTexture;
    class RHIShader;
    class RHIThread;
    class RHICommand;
} // namespace Tiny3D

#endif    /*__T3D_PREREQUISITES_H__*/
