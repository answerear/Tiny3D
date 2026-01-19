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


#pragma once


#include "T3DAssetImporter.h"


namespace Tiny3D
{
    class FBXImporterNew;

    T3D_DECLARE_SMART_PTR(FBXImporterNew);
    
    class FBXImporterNew : public AssetImporter
    {
        using Vector3Array = TArray<Vector3>;
        using ColorArray = TArray<ColorRGBA>;
        using SmoothingGroupArray = TArray<int32_t>;
        using Vector2Array = TArray<Vector2>;
        using IndexArray = TArray<uint32_t>;

        using Materials = TUnorderedMap<String, MaterialPtr>;
        using Textures = TUnorderedMap<String, TexturePtr>;

        struct BoneInfo
        {
            GameObject *gameObject {nullptr};
            Matrix4 offsetMatrix {Matrix4::IDENTITY};
        };
        
        using BoneInfoMap = TUnorderedMap<FbxNode*, uint16_t>;

        struct AnimationClipData : public Allocator
        {
            uint32_t duration {0};
            AnimationTracks tracks {};
        };

        using AnimationClipsData = TUnorderedMap<String, AnimationClipData*>;
        
        struct SubMeshData : public Allocator
        {
            FbxSurfaceMaterial *fbxMaterial {nullptr};
            IndexArray indices {};
        };
        
        using MaterialData = TUnorderedMap<String, SubMeshData*>;

        struct MeshData : public Allocator
        {
            ~MeshData()
            {
                for (auto& material : materials)
                {
                    T3D_POD_SAFE_DELETE(material.second);
                }
            }
            
            /// 顶点数据
            Vector3Array vertices {};
            /// 颜色数据
            ColorArray colors {};
            /// 平滑组数据
            SmoothingGroupArray smoothingGroups {};
            /// 纹理坐标数据
            Vector2Array texCoords[T3D_MAX_TEXTURE_LEVEL] {};
            /// 法线数据
            Vector3Array normals {};
            /// 切线数据
            Vector3Array tangents {};
            /// 副法线数据
            Vector3Array binormals {};

            /// 每个顶点对应的控制点索引（用于映射 blend weights）
            TArray<int32_t> vertexToControlPointMap {};

            /// 混合权重数据（用于蒙皮网格）
            TArray<TArray<float32_t>> blendWeights {};
            /// 混合索引数据（用于蒙皮网格）
            TArray<TArray<uint8_t>> blendIndices {};
            
            /// 材质数据
            MaterialData materials {};

            /// 子网格
            SubMeshes subMeshes {};

            /// 蒙皮网格的平移、旋转、缩放
            Vector3 skinnedMeshT {Vector3::ZERO};
            Quaternion skinnedMeshQ {Quaternion::IDENTITY};
            Vector3 skinnedMeshS {Vector3::UNIT_SCALE};
        };

        struct SkeletalAnimationData : public Allocator
        {
            /// 骨架信息
            Bones bones {};

            /// 骨骼对应的索引
            BoneInfoMap boneMap {};

            /// 所有动画剪辑信息
            AnimationClipsData animationClips {};

            /// 骨架资源
            SkeletonPtr skeleton {nullptr};

            /// 动画资源
            SkeletalAnimationPtr animation {nullptr};
        };
        
        struct AnimationTimeRange
        {
            float timeRangeStart {0.0f};
            float timeRangeEnd {0.0f};
            float timeRangeOffset {0.0f};
        };

        using FbxNodeLUT = TUnorderedMap<FbxNode*, FbxNode*>;
        using FbxMeshNodeLUT = TUnorderedMap<FbxNode*, FbxMesh*>;
        using FbxMeshRootLUT = TUnorderedMap<FbxMesh*, FbxNode*>;
        using FbxMeshLUT = TUnorderedMap<FbxMesh*, MeshData*>;
        using FbxSkeletonLUT = TUnorderedMap<FbxNode*, SkeletalAnimationData*>;
        
    public:
        static FBXImporterNewPtr create();

        ~FBXImporterNew() override;

    protected:
        FBXImporterNew();

        /// 继承自 AssetImporter，重写 AssetImporter::run
        TResult run(const ConverterOptions &opts, Assets &resources) override;

        /// 初始化 FBX SDK 对象
        TResult initFbxObjects();

        /// 销毁 FBX SDK 对象
        TResult destroyFbxObjects();

        /// 释放中间数据
        TResult releaseAllObjects();

        /// 导入场景
        TResult importScene(DataStream &stream, FbxScene *lFbxScene);

        /// 设置度量系统
        TResult setupMetricSystem(FbxScene *lFbxScene);

        /// 编译所有依赖的 shader
        TResult compileAllShaders(const String &tempPath, const String &assetsPath);

        /// 搜索文件夹中的 shader 并编译
        TResult compileShaders(const String &inputPath, const String &outputPath);

        /// 编译单个 shader
        TResult compileShader(const String &inputPath, const String &outputPath);

        /// 加载默认材质
        TResult loadDefaultMaterial(const String &rootPath, const String &relativePath);

        /// 预处理 FBX 场景，主要收集 FbxMesh 对象
        TResult processFbxScene(FbxScene *lFbxScene);

        /// 收集 FBX 网格信息
        TResult preprocessFbxScene(FbxScene *lFbxScene);

        /// 递归收集 FBX 网格信息
        TResult recursiveCollectFbxMesh(FbxNode *lFbxRoot, FbxNode *lFbxNode);

        /// 收集 FBX 网格信息
        TResult collectFbxMesh(FbxNode *lFbxRoot, FbxNode *lFbxNode);

        /// 处理 FBX 骨架和动画信息
        TResult generateSkeletonsAndAnimations(FbxScene *lFbxScene);

        /// 递归处理 FBX 骨架和动画信息
        TResult recursiveGenerateSkeletonsAndAnimations(FbxScene *lFbxScene, FbxNode *lFbxRoot, FbxNode *lFbxNode, uint16_t parentIdx);

        /// 处理 FBX 骨架和动画信息
        TResult generateSkeletonAndAnimation(FbxScene *lFbxScene, FbxNode *lFbxRoot, FbxNode *lFbxNode, uint16_t parentIdx, uint16_t &boneIdx);

        /// 处理 FBX 骨骼
        TResult generateBone(FbxNode *lFbxNode, SkeletalAnimationData *skelAniData, uint16_t parentIdx, uint16_t &boneIdx);

        /// 处理 FBX 骨骼动画
        TResult generateBoneAnimationTrack(FbxScene *lFbxScene, FbxNode *lFbxNode, SkeletalAnimationData *skelAniData);

        // /// 提取平移关键帧（内部辅助函数）
        // void extractTranslationKeyframes(FbxAnimCurveNode *lCurveNode, 
        //     const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
        //     TranslationTrack &track);
        //
        // /// 提取旋转关键帧（内部辅助函数）
        // void extractRotationKeyframes(FbxAnimCurveNode *lCurveNode, 
        //     const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
        //     OrientationTrack &track);
        //
        // /// 提取缩放关键帧（内部辅助函数）
        // void extractScalingKeyframes(FbxAnimCurveNode *lCurveNode, 
        //     const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
        //     ScalingTrack &track);
        
        void setupTimeRange(FbxScene& fbxScene, FbxAnimStack& animStack, AnimationTimeRange& output) const;
        
        void evaluateTimeRange(FbxAnimCurve *lFbxCurve, const AnimationTimeRange &range, float &minimum, float &maximum) const;

        /// 生成骨架链接数据，主要一个是生成 binding pose 数据，一个是生成 skeleton 对应 mesh 的关系
        TResult generateMeshesLinkData();

        /// 生成网格链接数据
        TResult generateMeshLinkData(FbxNode *lFbxMeshRoot, FbxMesh *lFbxMesh);

        /// 生成网格对象
        TResult generateMeshes(bool hasSkin);

        TResult generateMesh(FbxMesh *lFbxMesh, MeshData *meshData);

        TResult generateMeshSkinData(FbxMesh *lFbxMesh, MeshData *meshData);
        
        void getFbxTriangleSmoothGroupIndices(FbxGeometryBase *lFbxGeometry, int32_t triangleCount, TArray<int32_t> &triangleSmGroupIndices) const;

        void getFbxTriangleMaterialIndices(FbxGeometryBase *lFbxGeometry, int32_t triangleCount, TArray<int32_t> &triangleMaterialIndices) const;

        void readVertex(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, Vector3 &vertex);

        void readNormal(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &normal);

        void readTangent(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &tangent);

        void readBinormal(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &binormal);

        void readColor(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, ColorRGBA &color);

        void readUV(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t texUVIndex, int32_t uvLayer, Vector2 &uv);
        
        // TResult generateMeshIndices(MeshData *meshData);
        
        TResult createMaterialsAndSubMeshes(MeshData *meshData);

        TResult createMaterial(FbxSurfaceMaterial *lFbxMaterial, MaterialPtr &material);

        TResult createTexture(const FbxProperty &lFbxProperty, TexturePtr &texture);

        TResult createSubMesh(const String &name, SubMeshData *subMeshData, Material *material, SubMeshPtr &submesh);

        TResult createMesh(FbxNode *lFbxMeshRoot, MeshData *meshData);

        TResult createMeshes();

        TResult createSkeletonsAndAnimations();
        
        /// 生成骨架对象
        TResult createSkeleton(SkeletalAnimationData *skelAniData);

        /// 生成动画对象
        TResult createAnimation(SkeletalAnimationData *skelAniData);
        
        /// 将 FBX 点转换为 Tiny3D 向量
        Vector3 FbxPointToTinyVector3Remap(const FbxVector4 &lFbxPos) const;

        /// 将 FBX 欧拉角转换为 Tiny3D 四元数
        Quaternion FbxEulerToTinyQuaternion(const FbxVector4 &lFbxEuler) const;

        /// 将 FBX 点转换为 Tiny3D 向量
        Vector3 FbxPointToTinyVector3(const FbxVector4 &lFbxPos) const;

        Matrix4 FbxAMatrixToMatrix4(const FbxAMatrix lFbxM) const;

        // bool lookupFbxMeshNode(FbxNode *lFbxNode, FbxNode *&lFbxMeshNode) const;

        bool lookupFbxMeshRoot(FbxNode *lFbxBoneRoot, FbxNode *&lFbxMeshRoot) const;

        bool lookupFbxBoneRoot(FbxNode *lFbxMeshRoot, FbxNode *&lFbxBoneRoot) const;
        
        bool lookupMeshData(FbxMesh *lFbxMesh, MeshData *&meshData) const;

        bool lookupFbxMeshRoot(FbxMesh *lFbxMesh, FbxNode *&lFbxMeshRoot) const;

        FbxNode *getFbxSkeletalRoot(FbxNode *lFbxNode) const;

        SkeletalAnimationData *getSkeletalAnimationData(FbxNode *lFbxRoot);
        
    protected:
        FbxManager *mFbxManager {nullptr};

        FbxScene *mFbxScene {nullptr};
        
        double mFbxFrameRate {0.0};
        
        double mFbxSampleRate {0.0};

        /// Fbx 骨骼所在 FbxNode 根节点查找 Fbx mesh 所在 FbxNode 根节点映射表
        /// key : 骨架根节点, value : 网格根节点
        FbxNodeLUT mFbxBoneRootToMeshRoot {};

        FbxNodeLUT mFbxMeshRootToBoneRoot {};
        
        /// Fbx mesh 所在的 FbxNode 根节点查找 FbxMesh 映射表
        /// key : 网格根节点, value : 网格
        FbxMeshNodeLUT mFbxMeshes {};

        /// FbxMesh 所在的 FbxNode 根节点映射表
        /// key : 网格, value : 网格根节点
        FbxMeshRootLUT mFbxMeshRoots {};

        /// FbxMesh 查找 MeshData 映射表
        /// key : 网格, value : 网格数据
        FbxMeshLUT mFbxMeshData {};

        /// Fbx 骨骼所在的 FbxNode 根节点查找 SkeletalAnimationData 映射表
        /// key : 骨架根节点, value : 骨架数据
        FbxSkeletonLUT mFbxSkeletons {};

        /// 默认材质
        /// 1、FBX 材质是自定义的时候，直接用默认材质
        /// 2、FBX 材质的参数可以通过默认材质组合而来
        MaterialPtr mDefaultMaterial {nullptr};

        /// 输出名称
        String mOutputName {};

        /// 输入路径
        String mInputDir {};

        Assets mResources {};

        /// 是否 OpenGL 的制式
        bool mIsOpenGLMetric {false};
    };
}

