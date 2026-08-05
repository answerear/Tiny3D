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

#ifndef __T3D_PREFAB_MANAGER_H__
#define __T3D_PREFAB_MANAGER_H__


#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    /**
     * \brief 预制体资源单例管理器，负责 Prefab 的加载与保存
     */
    class T3D_ENGINE_API PrefabManager
        : public Singleton<PrefabManager>
        , public ResourceManager
    {
    public:
        /**
         * \brief 创建 PrefabManager 实例
         * \return 新创建的 PrefabManager 智能指针
         */
        static PrefabManagerPtr create();

        /**
         * \brief 新建预制体资源并纳入管理器缓存
         * \param [in] name : 资源名称，一般用预制体文件名
         * \param [in] root : 预制体根 GameObject，可为 nullptr 后续再设置
         * \return 成功返回 Prefab 智能指针，失败返回 nullptr
         * \remarks 相比直接 Prefab::create()，本接口会把资源登记到 UUID 缓存，
         *          之后才能通过 UUID 反查到该 Prefab
         */
        PrefabPtr createPrefab(const String &name, GameObjectPtr root = nullptr);

        /**
         * \brief 按名称从 Archive 加载预制体
         * \param [in] archive : 资源档案
         * \param [in] name : 预制体名称或路径
         * \return 成功返回 Prefab 智能指针，失败返回 nullptr
         */
        PrefabPtr loadPrefab(Archive *archive, const String &name);

        /**
         * \brief 将预制体保存到 Archive（使用资源自身文件名/名称）
         * \param [in] archive : 目标档案
         * \param [in] prefab : 待保存预制体，不可为 nullptr
         * \return 成功返回 T3D_OK
         */
        TResult savePrefab(Archive *archive, Prefab *prefab);

        /**
         * \brief 将预制体保存到指定文件名
         * \param [in] archive : 目标档案
         * \param [in] filename : 目标文件名
         * \param [in] prefab : 待保存预制体，不可为 nullptr
         * \return 成功返回 T3D_OK
         */
        TResult savePrefab(Archive *archive, const String &filename, Prefab *prefab);

        /**
         * \brief 按 UUID 加载预制体
         * \param [in] archive : 资源档案
         * \param [in] uuid : 预制体 UUID
         * \return 成功返回 Prefab 智能指针，失败返回 nullptr
         */
        PrefabPtr loadPrefab(Archive *archive, const UUID &uuid);

    protected:
        /**
         * \brief 创建 Prefab 资源实例
         * \param [in] name : 资源名称
         * \param [in] argc : 可变参数个数（未使用）
         * \param [in] args : 可变参数列表（未使用）
         * \return 新 Prefab 智能指针
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * \brief 从数据流反序列化预制体（忽略 name，委托 loadResource(stream)）
         * \param [in] name : 资源名称（未使用）
         * \param [in,out] stream : 输入数据流
         * \return 反序列化得到的 Prefab 智能指针
         */
        ResourcePtr loadResource(const String &name, DataStream &stream) override;

        /**
         * \brief 从数据流反序列化预制体
         * \param [in,out] stream : 输入数据流
         * \return 反序列化得到的 Prefab 智能指针
         */
        ResourcePtr loadResource(DataStream &stream) override;
        
        /**
         * \brief 将 Prefab 序列化写入数据流
         * \param [in,out] stream : 输出数据流
         * \param [in] res : 待保存的 Prefab 资源
         * \return 成功返回 T3D_OK
         */
        TResult saveResource(DataStream &stream, Resource *res) override;

        /**
         * \brief 保证预制体在 UUID 缓存中，使其可被 UUID 反查
         * \param [in] prefab : 预制体资源；为 nullptr 或已缓存时不做处理
         * \remarks 兼容外部直接 Prefab::create() 后再保存的路径
         */
        void ensureCached(Prefab *prefab);
    };

    #define T3D_PREFAB_MGR    (PrefabManager::getInstance()) 

    T3D_EXTERN_SINGLETON(PrefabManager)
}


#endif    /*__T3D_PREFAB_MANAGER_H__*/
