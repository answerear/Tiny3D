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


#ifndef __T3D_TYPEDEF_H__
#define __T3D_TYPEDEF_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /// MemoryType 前向声明（定义见 T3DRenderConstant.h）
    enum class MemoryType : uint32_t;

    /** \brief Core 对象 SmartPtr 类型别名（ClassNamePtr / ClassNameSafePtr） */

    T3D_DECLARE_SMART_PTR(Object);

    // Kernel
    T3D_DECLARE_SMART_PTR(GameObject);
    T3D_DECLARE_SMART_PTR(SceneObject);
    T3D_DECLARE_SMART_PTR(Plugin);
    T3D_DECLARE_SMART_PTR(Archive);
    T3D_DECLARE_SMART_PTR(ArchiveManager);
    T3D_DECLARE_SMART_PTR(CompositeArchive);

    // Serializer
    T3D_DECLARE_SMART_PTR(Serializer);
    T3D_DECLARE_SMART_PTR(BinSerializer);
    T3D_DECLARE_SMART_PTR(JsonSerializer);
    T3D_DECLARE_SMART_PTR(SerializerManager);
    
    // Image
    T3D_DECLARE_SMART_PTR(ImageCodec);
    T3D_DECLARE_SMART_PTR(ImageCodecBase);

    // Resource
    T3D_DECLARE_SMART_PTR(Resource);
    T3D_DECLARE_SMART_PTR(ResourceManager);
    T3D_DECLARE_SMART_PTR(AssetManager);
    T3D_DECLARE_SMART_PTR(Prefab);
    T3D_DECLARE_SMART_PTR(PrefabManager);
    T3D_DECLARE_SMART_PTR(Dylib);
    T3D_DECLARE_SMART_PTR(DylibManager);
    T3D_DECLARE_SMART_PTR(Material);
    T3D_DECLARE_SMART_PTR(MaterialManager);
    T3D_DECLARE_SMART_PTR(Texture);
    T3D_DECLARE_SMART_PTR(Texture1D);
    T3D_DECLARE_SMART_PTR(Texture1DArray);
    T3D_DECLARE_SMART_PTR(Texture2D);
    T3D_DECLARE_SMART_PTR(Texture2DArray);
    T3D_DECLARE_SMART_PTR(Texture3D);
    T3D_DECLARE_SMART_PTR(Cubemap);
    T3D_DECLARE_SMART_PTR(CubemapArray);
    T3D_DECLARE_SMART_PTR(TextureManager);
    T3D_DECLARE_SMART_PTR(Image);
    T3D_DECLARE_SMART_PTR(ImageManager);
    T3D_DECLARE_SMART_PTR(Shader);
    T3D_DECLARE_SMART_PTR(ShaderManager);
    T3D_DECLARE_SMART_PTR(Mesh);
    T3D_DECLARE_SMART_PTR(MeshManager);
    T3D_DECLARE_SMART_PTR(SubMesh);
    T3D_DECLARE_SMART_PTR(SkinnedMesh);
    T3D_DECLARE_SMART_PTR(Skeleton);
    T3D_DECLARE_SMART_PTR(SkeletonManager);
    T3D_DECLARE_SMART_PTR(SkeletalAnimation);
    T3D_DECLARE_SMART_PTR(AnimationManager);

    // Skeletal Animation
    T3D_DECLARE_SMART_PTR(Keyframe);
    T3D_DECLARE_SMART_PTR(KfTranslation);
    T3D_DECLARE_SMART_PTR(KfOrientation);
    T3D_DECLARE_SMART_PTR(KfScaling);
    T3D_DECLARE_SMART_PTR(AnimationTrack);
    T3D_DECLARE_SMART_PTR(AnimationClip);
    T3D_DECLARE_SMART_PTR(AnimationPlayer);
    T3D_DECLARE_SMART_PTR(AnimationPlayerMgr);

    // Material
    T3D_DECLARE_SMART_PTR(ShaderConstantParam);
    T3D_DECLARE_SMART_PTR(ShaderConstantValue);
    T3D_DECLARE_SMART_PTR(ShaderSamplerParam);
    T3D_DECLARE_SMART_PTR(ShaderSamplerValue);
    T3D_DECLARE_SMART_PTR(ShaderResourceParam);
    T3D_DECLARE_SMART_PTR(Technique);
    T3D_DECLARE_SMART_PTR(Pass);
    T3D_DECLARE_SMART_PTR(ShaderVariant);
    T3D_DECLARE_SMART_PTR(ShaderVariantSet);
    T3D_DECLARE_SMART_PTR(ShaderVariantInstance);
    T3D_DECLARE_SMART_PTR(PassInstance);
    T3D_DECLARE_SMART_PTR(TechniqueInstance);

    // Scene
    T3D_DECLARE_SMART_PTR(Scene);
    T3D_DECLARE_SMART_PTR(SceneManager);
    T3D_DECLARE_SMART_PTR(SceneManagerImpl);
    T3D_DECLARE_SMART_PTR(BuiltinSceneManager);

    // Component
    T3D_DECLARE_SMART_PTR(Component);
    T3D_DECLARE_SMART_PTR(Behaviour);
    T3D_DECLARE_SMART_PTR(PrefabInstance);
    T3D_DECLARE_SMART_PTR(Camera);
    T3D_DECLARE_SMART_PTR(TransformNode);
    T3D_DECLARE_SMART_PTR(Transform3D);
    T3D_DECLARE_SMART_PTR(Renderable);
    T3D_DECLARE_SMART_PTR(Geometry);
    T3D_DECLARE_SMART_PTR(SkinnedGeometry);
    T3D_DECLARE_SMART_PTR(Bone);
    T3D_DECLARE_SMART_PTR(Skybox);

    // Bound
    T3D_DECLARE_SMART_PTR(Bound);
    T3D_DECLARE_SMART_PTR(SphereBound);
    T3D_DECLARE_SMART_PTR(AabbBound);
    T3D_DECLARE_SMART_PTR(ObbBound);
    T3D_DECLARE_SMART_PTR(CapsuleBound);
    T3D_DECLARE_SMART_PTR(FrustumBound);

    // Light
    T3D_DECLARE_SMART_PTR(Light);
    T3D_DECLARE_SMART_PTR(IndirectLight);
    T3D_DECLARE_SMART_PTR(AmbientLight);
    T3D_DECLARE_SMART_PTR(LocalLight);
    T3D_DECLARE_SMART_PTR(PointLight);
    T3D_DECLARE_SMART_PTR(SpotLight);
    T3D_DECLARE_SMART_PTR(DirectionalLight);

    // Render
    T3D_DECLARE_SMART_PTR(RenderContext);
    T3D_DECLARE_SMART_PTR(RenderPipeline);
    T3D_DECLARE_SMART_PTR(ForwardRenderPipeline);

    T3D_DECLARE_SMART_PTR(RenderResource);
    
    T3D_DECLARE_SMART_PTR(BlendState);
    T3D_DECLARE_SMART_PTR(DepthStencilState);
    T3D_DECLARE_SMART_PTR(RasterizerState);
    T3D_DECLARE_SMART_PTR(SamplerState);
    T3D_DECLARE_SMART_PTR(RenderState);
    T3D_DECLARE_SMART_PTR(VertexDeclaration);
    T3D_DECLARE_SMART_PTR(RenderBuffer);
    T3D_DECLARE_SMART_PTR(VertexBuffer);
    T3D_DECLARE_SMART_PTR(IndexBuffer);
    T3D_DECLARE_SMART_PTR(PixelBuffer);
    T3D_DECLARE_SMART_PTR(PixelBuffer1D);
    T3D_DECLARE_SMART_PTR(PixelBuffer2D);
    T3D_DECLARE_SMART_PTR(PixelBuffer3D);
    T3D_DECLARE_SMART_PTR(PixelBufferCubemap);
    T3D_DECLARE_SMART_PTR(ConstantBuffer);
    T3D_DECLARE_SMART_PTR(StructuredBuffer);
    
    T3D_DECLARE_SMART_PTR(RenderStateManager);
    T3D_DECLARE_SMART_PTR(RenderBufferManager);
    T3D_DECLARE_SMART_PTR(RenderTargetManager);

    T3D_DECLARE_SMART_PTR(RenderTarget);
    T3D_DECLARE_SMART_PTR(RenderTexture);
    T3D_DECLARE_SMART_PTR(RenderWindow);
    T3D_DECLARE_SMART_PTR(NullRenderWindow);

    // RHI
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
    T3D_DECLARE_SMART_PTR(RHIPixelBuffer1D);
    T3D_DECLARE_SMART_PTR(RHIPixelBuffer2D);
    T3D_DECLARE_SMART_PTR(RHIPixelBuffer3D);
    T3D_DECLARE_SMART_PTR(RHIPixelBufferCubemap);
    T3D_DECLARE_SMART_PTR(RHIConstantBuffer);
    T3D_DECLARE_SMART_PTR(RHIStructuredBuffer);
    T3D_DECLARE_SMART_PTR(RHIVertexDeclaration);
    T3D_DECLARE_SMART_PTR(RHIRenderTarget);
    T3D_DECLARE_SMART_PTR(RHIRenderWindow);
    T3D_DECLARE_SMART_PTR(RHIRenderTexture);
    T3D_DECLARE_SMART_PTR(RHIShader);
    T3D_DECLARE_SMART_PTR(RHIThread);
    T3D_DECLARE_SMART_PTR(RHICommand);

#if defined(T3D_OS_DESKTOP)
    T3D_DECLARE_SMART_PTR(Meta);
    T3D_DECLARE_SMART_PTR(MetaFolder);
    T3D_DECLARE_SMART_PTR(MetaFile);
    T3D_DECLARE_SMART_PTR(MetaTxt);
    T3D_DECLARE_SMART_PTR(MetaBin);
    T3D_DECLARE_SMART_PTR(MetaDylib);
    T3D_DECLARE_SMART_PTR(MetaMaterial);
    T3D_DECLARE_SMART_PTR(MetaTexture);
    T3D_DECLARE_SMART_PTR(MetaShader);
    T3D_DECLARE_SMART_PTR(MetaMesh);
    T3D_DECLARE_SMART_PTR(MetaPrefab);
    T3D_DECLARE_SMART_PTR(MetaScene);
    T3D_DECLARE_SMART_PTR(MetaShader);
    T3D_DECLARE_SMART_PTR(MetaShaderLab);
    T3D_DECLARE_SMART_PTR(MetaAnimation);
    T3D_DECLARE_SMART_PTR(MetaSkeleton);
#endif
    

    /** \brief RHI 渲染器注册表类型别名 */
    using RHIRenderers = TMap<String, RHIRendererPtr>;
    using RHIRenderersItr = RHIRenderers::iterator;
    using RHIRenderersConstItr = RHIRenderers::const_iterator;
    using RHIRenderersValue = RHIRenderers::value_type;

    /** \brief 基础数值与颜色数组别名 */
    using BoolArray = TArray<bool>;
    using IntArray = TArray<int32_t>;
    using FloatArray = TArray<float32_t>;
    using ColorArray = TArray<ColorRGBA>;
    using Vector4Array = TArray<Vector4f>;
    using Matrix4Array = TArray<Matrix4>;
    
    /** \brief 材质 Pass 与实例容器别名 */
    using Passes = TList<PassPtr>;
    using PassInstances = TUnorderedMap<uint32_t, PassInstancePtr>;

    /** \brief 着色器关键字、变体与 ShaderLab 标签容器别名 */
    using ShaderKeywords = TList<ShaderKeyword>;
    using ShaderVariants = TMap<ShaderKeyword, ShaderVariantPtr>;
    using ShaderVariantsValue = ShaderVariants::value_type;
    /// 按 keyword 索引的多语言变体集合
    using ShaderVariantSets = TMap<ShaderKeyword, ShaderVariantSetPtr>;
    using ShaderVariantSetsValue = ShaderVariantSets::value_type;
    
    using ShaderLabTags = TMap<String, String>;
    using ShaderLabTagsValue = ShaderLabTags::value_type;

    using ShaderConstantParams = TUnorderedMap<String, ShaderConstantParamPtr>;
    using ShaderConstantValues = TUnorderedMap<String, ShaderConstantValuePtr>;
    using ShaderSamplerParams = TUnorderedMap<String, ShaderSamplerParamPtr>;
    using ShaderSamplerValues = TUnorderedMap<String, ShaderSamplerValuePtr>;
    using ShaderResourceParams = TUnorderedMap<String, ShaderResourceParamPtr>;

    /** \brief 渲染绑定与场景对象容器别名 */
    using ConstBuffers = TArray<Buffer>;
    using Textures = TArray<TexturePtr>;

    using GameObjectList = TList<GameObjectPtr>;
    using CameraList = TMultimap<uint32_t, Camera *>;

    /** \brief 网格顶点与缓冲描述容器别名 */
    using VertexAttributes = TArray<VertexAttribute>;
    using VertexBuffers = TArray<VertexBufferPtr>;

    using SubMeshes = TMap<String, SubMeshPtr>;
    using Vertices = TArray<Buffer>;
    using VertexStrides = TArray<uint32_t>;
    using VertexOffsets = TArray<uint32_t>;

    using PixelBuffers = TArray<PixelBufferPtr>;
    using ConstantBuffers = TArray<ConstantBufferPtr>;
    using Samplers = TArray<SamplerStatePtr>;

    /// 只读结构化缓冲绑定数组（SRV 路径）
    using StructuredBuffers = TArray<StructuredBufferPtr>;
    /// UAV 绑定数组；元素可以是 PixelBufferXD、StructuredBuffer 或带 UAV 的顶点缓冲，
    /// 因此元素类型取三者唯一的共同基类 RenderBuffer
    using UnorderedAccessBuffers = TArray<RenderBufferPtr>;
    /// Append/Consume 与 Counter 缓冲的初始计数；kKeepUAVCounter 表示保持当前值
    using UAVInitialCounts = TArray<uint32_t>;

    using RenderTextures = TArray<RenderTexture*>;

    /** \brief 骨骼动画轨道与播放器容器别名 */
    using TranslationTrack = TArray<KfTranslationPtr>;
    using OrientationTrack = TArray<KfOrientationPtr>;
    using ScalingTrack = TArray<KfScalingPtr>;
    using AnimationTracks = TMap<String, AnimationTrackPtr>;
    using AnimationClips = TMap<String, AnimationClipPtr>;
    using BoneGameObjectsMap = TUnorderedMap<String, GameObject*>;
    using BoneGameObjects = TArray<GameObject*>;

    using GameObjects = TUnorderedMap<UUID, GameObjectPtr, UUIDHash, UUIDEqual>;
    using AnimationPlayers = TUnorderedMap<AnimationPlayer*, AnimationPlayerPtr>;
}


#endif  /*__T3D_TYPEDEF_H__*/
