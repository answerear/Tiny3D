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

        /// 加载默认材质
        TResult loadDefaultMaterial(const String &path);

        /// 处理静态网格
        TResult processStaticMesh(FbxScene *lFbxScene);

        /// 获取 FBX 网格节点
        TResult getFbxMeshNode(FbxNode *lNode);

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

        TResult createSubMesh(const String &name, Material *material, SubMeshPtr &submesh);

        /// 处理蒙皮网格
        TResult processSkinnedMesh(FbxScene *lFbxScene);

        /// 处理骨架
        TResult processSkeleton(FbxScene *lFbxScene);

        /// 处理动画
        TResult processAnimation(FbxScene *lFbxScene);
        
    protected:
        FbxManager *mFbxManager {nullptr};

        using Vector3Array = TArray<Vector3>;
        using ColorArray = TArray<ColorRGBA>;
        using SmoothingGroupArray = TArray<int32_t>;
        using Vector2Array = TArray<Vector2>;
        using IndexArray = TArray<uint32_t>;

        using SubMeshes = TUnorderedMap<String, SubMeshPtr>;

        using Materials = TUnorderedMap<String, MaterialPtr>;

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

        /// 子网格
        SubMeshes mSubMeshes {};
        /// 材质
        Materials mMaterials {};

        /// 默认材质
        /// 1、FBX 材质是自定义的时候，直接用默认材质
        /// 2、FBX 材质的参数可以通过默认材质组合而来
        MaterialPtr mDefaultMaterial {nullptr};

        Assets mResources {};
    };
}

