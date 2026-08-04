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

#ifndef __T3D_MESH_MANAGER_H__
#define __T3D_MESH_MANAGER_H__


#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    /**
     * \brief Mesh / SkinnedMesh 资源管理器，负责创建、加载与保存网格资源
     */
    class T3D_ENGINE_API MeshManager
        : public Singleton<MeshManager>
        , public ResourceManager
    {
    public:
        /**
         * \brief 创建 MeshManager 单例对象
         * \return 新建的 MeshManager 智能指针
         */
        static MeshManagerPtr create();

        /**
         * \brief 用完整数据创建 Mesh 资源
         * \param [in] name : 网格名称
         * \param [in] attributes : 顶点属性
         * \param [in] vertices : 顶点数据
         * \param [in] strides : 顶点跨度
         * \param [in] offsets : 顶点偏移
         * \param [in] submeshes : 子网格
         * \param [in] position : 网格节点位置
         * \param [in] orientation : 网格节点旋转
         * \param [in] scaling : 网格节点缩放
         * \param [in] meshNodeName : 场景节点名称
         * \param [in] uuid : 可选 UUID；编辑器下非 INVALID 时保留指定 guid
         * \return 成功返回 Mesh 智能指针；createResource 失败时返回 nullptr
         */
        MeshPtr createMesh(const String &name, VertexAttributes &&attributes, Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes,
            const Vector3 &position = Vector3::ZERO, const Quaternion &orientation = Quaternion::IDENTITY, const Vector3 &scaling = Vector3::UNIT_SCALE,
            const String &meshNodeName = "", const UUID &uuid = UUID::INVALID);
        
        /**
         * \brief 按文件名从档案加载 Mesh
         * \param [in] archive : 来源档案
         * \param [in] filename : 网格文件名
         * \return 成功返回 Mesh 智能指针；加载失败时返回 nullptr
         */
        MeshPtr loadMesh(Archive *archive, const String &filename);

        /**
         * \brief 按 UUID 从档案加载 Mesh
         * \param [in] archive : 来源档案
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Mesh 智能指针；加载失败时返回 nullptr
         */
        MeshPtr loadMesh(Archive *archive, const UUID &uuid);

        /**
         * \brief 将 Mesh 保存到指定文件名
         * \param [in] archive : 目标档案
         * \param [in] filename : 保存文件名
         * \param [in] mesh : 要保存的 Mesh 对象
         * \return 转调 ResourceManager::save 的返回值
         */
        TResult saveMesh(Archive *archive, const String &filename, Mesh *mesh);

        /**
         * \brief 将 Mesh 保存到以其 UUID 为键的路径
         * \param [in] archive : 目标档案
         * \param [in] mesh : 要保存的 Mesh 对象
         * \return 转调 ResourceManager::save 的返回值
         */
        TResult saveMesh(Archive *archive, Mesh *mesh);

        /**
         * \brief 创建 SkinnedMesh 资源
         * \param [in] name : 网格名称
         * \param [in] attributes : 顶点属性
         * \param [in] vertices : 顶点数据
         * \param [in] strides : 顶点跨度
         * \param [in] offsets : 顶点偏移
         * \param [in] submeshes : 子网格
         * \param [in] skeleton : 骨架资源
         * \param [in] skeletalAni : 骨骼动画资源
         * \param [in] position : 网格节点位置
         * \param [in] orientation : 网格节点旋转
         * \param [in] scaling : 网格节点缩放
         * \param [in] meshNodeName : 场景节点名称
         * \param [in] uuid : 可选 UUID
         * \return 成功返回 SkinnedMesh 智能指针；createResource 失败时返回 nullptr
         */
        SkinnedMeshPtr createSkinnedMesh(const String &name,
            VertexAttributes &&attributes, Vertices &&vertices,
            VertexStrides &&strides, VertexOffsets &&offsets,
            SubMeshes &&submeshes, Skeleton *skeleton,
            SkeletalAnimation *skeletalAni, 
            const Vector3 &position = Vector3::ZERO, 
            const Quaternion &orientation = Quaternion::IDENTITY,
            const Vector3 &scaling = Vector3::UNIT_SCALE,
            const String &meshNodeName = "", const UUID &uuid = UUID::INVALID);

    protected:
        /**
         * \brief 创建 Mesh 或 SkinnedMesh 实例（argc 为 9/10 或 11/12）
         * \param [in] name : 资源名称
         * \param [in] argc : 可变参数个数
         * \param [in] args : 可变参数列表
         * \return 对应 Mesh/SkinnedMesh::create 的结果
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * \brief 从数据流反序列化 Mesh
         * \param [in] name : 资源名称（未使用）
         * \param [in,out] stream : 输入数据流
         * \return Serializer 反序列化结果
         */
        ResourcePtr loadResource(const String &name, DataStream &stream) override;

        /**
         * \brief 从数据流反序列化 Mesh
         * \param [in,out] stream : 输入数据流
         * \return Serializer 反序列化结果
         */
        ResourcePtr loadResource(DataStream &stream) override;

        /**
         * \brief 将 Mesh 或 SkinnedMesh 序列化到数据流
         * \param [in,out] stream : 输出数据流
         * \param [in] res : 资源对象（须为 kMesh 或 kSkinnedMesh）
         * \return Serializer 序列化结果
         */
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_MESH_MGR    (MeshManager::getInstance())

    T3D_EXTERN_SINGLETON(MeshManager)
}


#endif    /*__T3D_MESH_MANAGER_H__*/
