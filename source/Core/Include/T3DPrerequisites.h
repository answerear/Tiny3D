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

#ifndef __T3D_PREREQUISITES_H__
#define __T3D_PREREQUISITES_H__


#include "T3DMacro.h"


#if defined(T3D_CORE_STATIC)
    #define T3D_ENGINE_API
#else
    #if defined T3DCORE_EXPORT
        #define T3D_ENGINE_API T3D_EXPORT_API
    #else
        #define T3D_ENGINE_API T3D_IMPORT_API
    #endif 
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

    // Kernel
    class Agent;
    class GameObject;
    class SceneObject;
    class Plugin;
    class Archive;
    class ArchiveManager;
    
    struct Transform;
    struct Settings;
    struct MeshData;

    // Serializer
    class Serializer;
    class BinSerializer;
    class JsonSerializer;
    class SerializerManager;

    // Image
    class ImageCodec;
    class ImageCodecBase;

    // Resource
    class Resource;
    class ResourceManager;
    class Dylib;
    class DylibManager;
    class Material;
    class MaterialManager;
    class Texture;
    class Texture1D;
    class Texture1DArray;
    class Texture2D;
    class Texture2DArray;
    class Texture3D;
    class Cubemap;
    class CubemapArray;
    class TextureManager;
    class Image;
    class ImageManager;
    class Prefab;
    class PrefabManager;
    class Shader;
    class ShaderManager;
    class Mesh;
    class MeshManager;
    class SubMesh;
    class SkinnedMesh;
    class SkeletalAnimation;
    class AnimationManager;

    // Skeletal Animation
    class BoneNode;
    class Keyframe;
    class KfTranslation;
    class KfOrientation;
    class KfScaling;
    class AnimationTrack;
    class AnimationClip;
    
    // Material
    class ShaderConstantParam;
    class ShaderConstantValue;
    class ShaderSamplerParam;
    class ShaderSamplerValue;
    class ShaderVariant;
    class Technique;
    class Pass;
    class ShaderVariantInstance;
    class PassInstance;
    class TechniqueInstance;
    struct ShaderKeyword;

    // Scene
    class Scene;
    class SceneManager;
    class SceneManagerImpl;
    class BuiltinSceneManager;

    // Component
    class Component;
    class Camera;
    class TransformNode;
    class Transform3D;
    class Renderable;
    class Geometry;
    class SkinnedGeometry;

    // Bound
    class Bound;
    class SphereBound;
    class AabbBound;
    class ObbBound;
    class FrustumBound;

    // Light
    class Light;
    class IndirectLight;
    class AmbientLight;
    class LocalLight;
    class DirectionalLight;
    class PointLight;
    class SpotLight;
    
    // Render
    class RenderContext;
    class RenderPipeline;
    class ForwardRenderPipeline;

    class RenderResource;
    
    struct BlendDesc;
    struct DepthStencilDesc;
    struct RasterizerDesc;
    struct SamplerDesc;

    struct VertexBufferDesc;
    struct IndexBufferDesc;
    struct PixelBufferDesc;
    struct PixelBuffer1DDesc;
    struct PixelBuffer2DDesc;
    struct PixelBuffer3DDesc;

    class BlendState;
    class DepthStencilState;
    class RasterizerState;
    class SamplerState;
    class RenderState;
    class VertexDeclaration;
    class RenderBuffer;
    class VertexBuffer;
    class IndexBuffer;
    class PixelBuffer;
    class PixelBuffer1D;
    class PixelBuffer2D;
    class PixelBuffer3D;
    class PixelBufferCubemap;
    class ConstantBuffer;

    struct VertexAttribute;

    class RenderStateManager;
    class RenderBufferManager;
    class RenderTargetManager;
    
    class RenderTarget;
    class RenderTexture;
    class RenderWindow;
    class NullRenderWindow;
    
    struct Viewport;

    // RHI
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
    class RHIPixelBuffer1D;
    class RHIPixelBuffer2D;
    class RHIPixelBuffer3D;
    class RHIConstantBuffer;
    class RHIVertexDeclaration;
    class RHIRenderTarget;
    class RHIRenderWindow;
    class RHIRenderTexture;
    class RHIShader;
    class RHIThread;
    class RHICommand;

#if defined(T3D_EDITOR)
    class Meta;
    class MetaFolder;
    class MetaFile;
    class MetaTxt;
    class MetaBin;
    class MetaDylib;
    class MetaMaterial;
    class MetaTexture;
    class MetaShader;
    class MetaMesh;
    class MetaPrefab;
    class MetaScene;
    class MetaShader;
    class MetaShaderLab;
#endif
} // namespace Tiny3D

#endif    /*__T3D_PREREQUISITES_H__*/
