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

#ifndef __T3D_MATERIAL_MANAGER_H__
#define __T3D_MATERIAL_MANAGER_H__


#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    /**
     * \brief Material 资源管理器，负责创建、加载与保存 Material
     */
    class T3D_ENGINE_API MaterialManager
        : public Singleton<MaterialManager>
        , public ResourceManager
    {
    public:
        /**
         * \brief 创建 MaterialManager 单例对象
         * \return 新建的 MaterialManager 智能指针
         */
        static MaterialManagerPtr create();

        /**
         * \brief 创建绑定 Shader 的 Material 资源
         * \param [in] name : 材质名称
         * \param [in] shader : 关联 Shader 对象
         * \param [in] uuid : 可选 UUID；编辑器下非 INVALID 时在创建时保留指定 guid
         * \return 成功返回 Material 智能指针；createResource 失败时返回 nullptr
         */
        MaterialPtr createMaterial(const String &name, Shader *shader, const UUID &uuid = UUID::INVALID);

        /**
         * \brief 按文件名从档案加载 Material
         * \param [in] archive : 来源档案
         * \param [in] filename : 材质文件名
         * \return 成功返回 Material 智能指针；加载失败时返回 nullptr
         */
        MaterialPtr loadMaterial(Archive *archive, const String &filename);

        /**
         * \brief 按 UUID 从档案加载 Material
         * \param [in] archive : 来源档案
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Material 智能指针；加载失败时返回 nullptr
         */
        MaterialPtr loadMaterial(Archive *archive, const UUID &uuid);

        /**
         * \brief 将 Material 保存到指定文件名
         * \param [in] archive : 目标档案
         * \param [in] filename : 保存文件名
         * \param [in] material : 要保存的 Material 对象
         * \return 转调 ResourceManager::save 的返回值
         */
        TResult saveMaterial(Archive *archive, const String &filename, Material *material);

        /**
         * \brief 将 Material 保存到以其 UUID 为键的路径
         * \param [in] archive : 目标档案
         * \param [in] material : 要保存的 Material 对象
         * \return 转调 ResourceManager::save 的返回值
         */
        TResult saveMaterial(Archive *archive, Material *material);

    protected:
        /**
         * \brief 创建 Material 实例（argc 为 1 或 2：shader 及可选 uuid）
         * \param [in] name : 资源名称
         * \param [in] argc : 可变参数个数
         * \param [in] args : 可变参数列表
         * \return Material::create 的结果
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * \brief 从数据流反序列化 Material（忽略 filename 参数）
         * \param [in] name : 资源名称（未使用）
         * \param [in,out] stream : 输入数据流
         * \return 转调 loadResource(stream)
         */
        ResourcePtr loadResource(const String &name, DataStream &stream) override;

        /**
         * \brief 从数据流反序列化 Material
         * \param [in,out] stream : 输入数据流
         * \return Serializer 反序列化结果
         */
        ResourcePtr loadResource(DataStream &stream) override;

        /**
         * \brief 将 Material 序列化到数据流
         * \param [in,out] stream : 输出数据流
         * \param [in] res : 资源对象
         * \return Serializer 序列化结果
         */
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_MATERIAL_MGR    (MaterialManager::getInstance()) 

    T3D_EXTERN_SINGLETON(MaterialManager)
}


#endif    /*__T3D_MATERIAL_MANAGER_H__*/
