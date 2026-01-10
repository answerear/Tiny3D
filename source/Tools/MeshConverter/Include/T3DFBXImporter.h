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
    class FBXImporter;

    T3D_DECLARE_SMART_PTR(FBXImporter);
    
    class FBXImporter : public AssetImporter
    {
    public:
        static FBXImporterPtr create();

        ~FBXImporter() override;

    protected:
        FBXImporter();

        TResult run(const ConverterOptions &opts, Assets &resources) override;

        /// 初始化 FBX SDK 对象
        TResult initFbxObjects();

        /// 销毁 FBX SDK 对象
        TResult destroyFbxObjects();

        /// 导入场景
        TResult importScene(DataStream &stream, FbxScene *lFbxScene);

        /// 设置度量系统
        TResult setupMetricSystem(FbxScene *lFbxScene);

        TResult compileAllShaders(const String &tempPath, const String &assetsPath);

        TResult compileShaders(const String &inputPath, const String &outputPath);

        TResult compileShader(const String &inputPath, const String &outputPath);

        /// 加载默认材质
        TResult loadDefaultMaterial(const String &rootPath, const String &relativePath);
        
        TResult processFbxScene(FbxScene *lFbxScene);

        TResult processFbxNode(FbxNode *lFbxNode, GameObject *goParent);

        TResult processFbxMesh(FbxNode *lFbxNode);

        /// 处理静态网格
        TResult processStaticMesh(FbxScene *lFbxScene);

        /// 获取 FBX 网格节点
        TResult getFbxMeshNode(FbxNode *lNode, bool hasSkin);

        /// 处理 FBX 网格节点
        TResult processFbxMeshNode(FbxNode *lNode);

        void getFbxTriangleSmoothGroupIndices(FbxGeometryBase *lGeometry, int32_t triangleCount, TArray<int32_t> &triangleSmGroupIndices) const;

        void getFbxTriangleMaterialIndices(FbxGeometryBase *lGeometry, int32_t triangleCount, TArray<int32_t> &triangleMaterialIndices) const;

        void readVertex(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, Vector3 &vertex);

        void readNormal(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &normal);

        void readTangent(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &tangent);

        void readBinormal(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &binormal);

        void readColor(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, ColorRGBA &color);

        void readUV(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t texUVIndex, int32_t uvLayer, Vector2 &uv);

        TResult createMaterial(FbxSurfaceMaterial *lFbxMaterial, MaterialPtr &material);

        TResult createTexture(const FbxProperty &lFbxProperty, TexturePtr &texture);

        TResult createSubMesh(const String &name, Material *material, SubMeshPtr &submesh);

        TResult createMesh();

        /// 处理蒙皮网格
        TResult processSkinnedMesh(FbxScene *lFbxScene);

        /// 处理骨架
        TResult processSkeleton(FbxScene *lFbxScene);

        /// 递归获取骨架结构
        TResult getSkeleton(FbxScene *lFbxScene, FbxNode *lFbxNode, GameObjectPtr parent);

        /// 提取并计算 offset matrix
        TResult getOffsetMatrix(FbxMesh *lFbxMesh);

        /// 提取蒙皮网格的混合权重和索引
        TResult extractBlendWeightsAndIndices(FbxScene *lFbxScene, FbxNode *lFbxNode, const TMap<String, uint8_t> &boneNameToIndexMap);

        /// 处理单个网格的混合数据
        TResult processMeshBlendData(FbxMesh *lFbxMesh, const TMap<String, uint8_t> &boneNameToIndexMap);

        /// 根据骨架层次结构，生成骨骼信息索引表
        TResult generateBones();

        /// 创建骨架
        TResult createSkeleton();

        /// 处理动画
        TResult processAnimation(FbxScene *lFbxScene);
        
        /// 提取平移关键帧（内部辅助函数）
        void extractTranslationKeyframes(FbxAnimCurveNode *lCurveNode, 
            const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
            TranslationTrack &track);

        /// 提取旋转关键帧（内部辅助函数）
        void extractRotationKeyframes(FbxAnimCurveNode *lCurveNode, 
            const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
            OrientationTrack &track);

        /// 提取缩放关键帧（内部辅助函数）
        void extractScalingKeyframes(FbxAnimCurveNode *lCurveNode, 
            const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
            ScalingTrack &track);

        Vector3 FbxPointToTinyVector3Remap(const FbxVector4 &lFbxPos) const;

        Quaternion FbxEulerToTinyQuaternion(const FbxVector4 &lFbxEuler) const;

        Vector3 FbxPointToTinyVector3(const FbxVector4 &lFbxPos) const;
        
    protected:
        FbxManager *mFbxManager {nullptr};

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
        
        using BoneInfoMap = TMap<String, BoneInfo>;

        using FbxNodeLUT = TUnorderedMap<FbxNode*, GameObjectPtr>;
        using FbxMeshLUT = TUnorderedMap<FbxMesh*, MeshData*>;

        struct MeshData
        {
            
        };

        /// 顶点数据
        Vector3Array mVertices {};
        /// 颜色数据
        ColorArray mColors {};
        /// 平滑组数据
        SmoothingGroupArray mSmoothingGroups {};
        /// 纹理坐标数据
        Vector2Array mTexCoords[T3D_MAX_TEXTURE_LEVEL] {};
        /// 法线数据
        Vector3Array mNormals {};
        /// 切线数据
        Vector3Array mTangents {};
        /// 副法线数据
        Vector3Array mBinormals {};
        /// 索引数据
        IndexArray mIndices {};

        /// 每个顶点对应的控制点索引（用于映射 blend weights）
        TArray<int32_t> mVertexToControlPointMap {};

        /// 混合权重数据（用于蒙皮网格）
        TArray<TArray<float32_t>> mBlendWeights {};
        /// 混合索引数据（用于蒙皮网格）
        TArray<TArray<uint8_t>> mBlendIndices {};

        /// 子网格
        SubMeshes mSubMeshes {};

        /// 蒙皮网格的平移、旋转、缩放
        Vector3 mMeshT {Vector3::ZERO};
        Quaternion mMeshQ {Quaternion::IDENTITY};
        Vector3 mMeshS {Vector3::UNIT_SCALE};

        /// 骨架信息
        Bones mBones {};

        /// 骨架对应的 Game Object 映射表，方便后续生成索引表
        BoneInfoMap mBoneMap {};

        /// 动画片段集合
        AnimationClips mAnimationClips {};

        GameObjectPtr mRoot {nullptr};

        SkeletonPtr mSkeleton {nullptr};

        SkeletalAnimationPtr mAnimation {nullptr};
        
        /// FBXNode 查找 Game Object 映射表
        FbxNodeLUT mFbxNodesToGameObjects {};

        /// FBXMesh 查找 MeshData 映射表
        FbxMeshLUT mFbxMeshes {};

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

