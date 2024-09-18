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
    class T3D_ENGINE_API MeshManager
        : public Singleton<MeshManager>
        , public ResourceManager
    {
    public:
        /**
         * @brief 创建 mesh 资源管理器对象
         */
        static MeshManagerPtr create();

        /**
         * @brief 创建 mesh 对象
         * @param [in] name : mesh 名称
         * @param [in] attributes : 顶点属性
         * @param [in] vertices : 顶点数据数组
         * @param [in] strides : 顶点跨度数组，也就是一个顶点数据的大小
         * @param [in] offsets : 顶点数据的偏移
         * @param [in] submeshes : 子 mesh 对象
         * @return 调用成功返回 mesh 对象，否则返回 nullptr
         */
        MeshPtr createMesh(const String &name, VertexAttributes &&attributes, Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes);

        /**
         * @brief 根据文件名，加载 mesh 对象
         * @param [in] archive : 从该档案对象中加载 mesh 对象
         * @param [in] filename : mesh 文件名
         * @return 调用成功返回 mesh 对象，否则返回 nullptr
         */
        MeshPtr loadMesh(Archive *archive, const String &filename);

        /**
         * @brief 根据 UUID 加载 mesh 对象
         * @param [in] archive : 从该档案对象中加载 mesh 对象
         * @param [in] uuid : 资源的 UUID
         * @return 调用成功返回 mesh 对象，否则返回 nullptr
         */
        MeshPtr loadMesh(Archive *archive, const UUID &uuid);

        /**
         * @brief 把 mesh 对象写到对应文件名的 mesh 文件中
         * @param [in] archive : 保存 mesh 文件到该档案对象中
         * @param [in] filename : mesh 文件名
         * @param [in] mesh : 要保存的 mesh 对象
         * @return 调用成功返回 T3D_OK
         */
        TResult saveMesh(Archive *archive, const String &filename, Mesh *mesh);

        /**
         * @brief 把 mesh 对象写到 UUID 对应的 mesh 文件中
         * @param [in] archive : 保存 mesh 文件到该档案对象中
         * @param [in] mesh : 要保存的 mesh 对象
         * @return 调用成功返回 T3D_OK
         */
        TResult saveMesh(Archive *archive, Mesh *mesh);

    protected:
        /**
         * 重写基类接口
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * 重写基类接口
         */
        ResourcePtr loadResource(const String &name, DataStream &stream) override;

        /**
         * 重写基类接口
         */
        ResourcePtr loadResource(DataStream &stream) override;

        /**
         * 重写基类接口
         */
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_MESH_MGR    (MeshManager::getInstance())
}


#endif    /*__T3D_MESH_MANAGER_H__*/
