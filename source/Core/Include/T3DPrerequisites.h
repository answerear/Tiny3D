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


/** \brief Core 符号导出/导入宏；T3D_CORE_STATIC 时为空，T3DCORE_EXPORT 时导出否则导入 */
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
    /** \brief 各子系统日志标签字符串，供 T3D_LOG_* 使用 */

    #define LOG_TAG_CONFIG_FILE         "ConfigFile"
    #define LOG_TAG_ENGINE              "Agent"
    #define LOG_TAG_INPUT               "Input"
    #define LOG_TAG_IMAGE               "Image"
    #define LOG_TAG_RENDER              "Render"
    #define LOG_TAG_PLUGIN              "Plugin"
    #define LOG_TAG_IMAGE_CODEC         "ImageCodec"
    #define LOG_TAG_SCENE               "Scene"
    #define LOG_TAG_RESOURCE            "Resource"
    #define LOG_TAG_COMPONENT           "Component"
    #define LOG_TAG_SERIALIZE           "Serialize"
    #define LOG_TAG_ANIMATION           "Animation"

    #define TXT_DESCRIPTION             "Description"

    /** \brief 后端声明某个 RHI 接口不受支持时的统一 stub 宏 */

    /**
     * \brief 声明某个返回 TResult 的 RHI 接口在当前后端不受支持
     * \remarks 三件事一次做完：
     *          1) Debug 下断言对应能力位为 false，防止「能力位说支持但接口是 stub」
     *          2) 打警告日志，让误用在日志里立刻可见
     *          3) 返回 T3D_ERR_NOT_IMPLEMENT，而不是谎报 T3D_OK
     *          只能在 RHIContext 派生类的成员函数内使用（依赖 getCapabilities()）
     * \param capField : RHICapabilities 中对应的能力位字段名
     */
    #define T3D_RHI_UNSUPPORTED(capField)                                       \
        do {                                                                    \
            T3D_ASSERT(!getCapabilities().capField);                            \
            T3D_LOG_WARNING(LOG_TAG_RENDER,                                     \
                "%s is not supported by this RHI backend", __FUNCTION__);        \
            return T3D_ERR_NOT_IMPLEMENT;                                       \
        } while (false)

    /**
     * \brief T3D_RHI_UNSUPPORTED 的指针返回版本，用于 createXXX 系列接口
     * \param capField : RHICapabilities 中对应的能力位字段名
     */
    #define T3D_RHI_UNSUPPORTED_PTR(capField)                                   \
        do {                                                                    \
            T3D_ASSERT(!getCapabilities().capField);                            \
            T3D_LOG_WARNING(LOG_TAG_RENDER,                                     \
                "%s is not supported by this RHI backend", __FUNCTION__);        \
            return nullptr;                                                     \
        } while (false)

    /** \brief Core 类型前向声明，供头文件间解耦引用 */

    class Object;
    class ObjectTracer;

    // Kernel
    class Agent;
    class Time;
    class Input;
    class GameObject;
    class SceneObject;
    class Plugin;
    class Archive;
    class ArchiveManager;
    class CompositeArchive;
    
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
    class AssetManager;
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
    class PrefabInstance;
    struct PropertyModification;
    class Shader;
    class ShaderManager;
    class Mesh;
    class MeshManager;
    class SubMesh;
    class SkinnedMesh;
    class Skeleton;
    class SkeletonManager;
    class SkeletalAnimation;
    class AnimationManager;

    // Skeletal Animation
    class Keyframe;
    class KfTranslation;
    class KfOrientation;
    class KfScaling;
    class AnimationTrack;
    class AnimationClip;
    class AnimationPlayer;
    class AnimationPlayerMgr;
    
    // Material
    class ShaderConstantParam;
    class ShaderConstantValue;
    class ShaderSamplerParam;
    class ShaderSamplerValue;
    class ShaderResourceParam;
    class ShaderVariant;
    class ShaderVariantSet;
    enum class SHADER_LANGUAGE : uint32_t;
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
    class Behaviour;
    class Camera;
    class TransformNode;
    class Transform3D;
    class Renderable;
    class Geometry;
    class SkinnedGeometry;
    class Bone;
    class Skybox;

    // Bound
    class Bound;
    class SphereBound;
    class AabbBound;
    class ObbBound;
    class CapsuleBound;
    class FrustumBound;

    // Light
    class Light;
    class IndirectLight;
    class AmbientLight;
    class LocalLight;
    class DirectionalLight;
    class PointLight;
    class SpotLight;

    // Performance
    class Performance;
    
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
    struct StructuredBufferDesc;

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
    class StructuredBuffer;

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
    class RHIPixelBufferCubemap;
    class RHIConstantBuffer;
    class RHIStructuredBuffer;
    class RHIVertexDeclaration;
    class RHIRenderTarget;
    class RHIRenderWindow;
    class RHIRenderTexture;
    class RHIShader;
    class RHIThread;
    class RHICommand;

#if defined(T3D_OS_DESKTOP)
    /** \brief 桌面端 .meta 资源描述类型前向声明 */
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
    class MetaAnimation;
    class MetaSkeleton;
#endif
} // namespace Tiny3D

#endif    /*__T3D_PREREQUISITES_H__*/
