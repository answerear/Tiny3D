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

#ifndef __T3D_RESOURCE_MANAGER_H__
#define __T3D_RESOURCE_MANAGER_H__


#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 资源管理器，统一管理资源
     */
    class T3D_ENGINE_API ResourceManager : public Object
    {
    public:
        /**
         * \brief 析构函数
         */
        ~ResourceManager() override = default;

        /**
         * \brief 卸载对应资源
         * \param [in] res : 要卸载的资源对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult unload(ResourcePtr res);
        
        /**
         * \brief 卸载所有资源
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult unloadAllResources();

        /**
         * \brief 卸载所有没用到的资源
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult unloadUnused();

        /**
         * \brief 克隆新资源对象
         * \param [in] src : 源资源对象 
         * \return 返回新资源对象
         */
        ResourcePtr clone(ResourcePtr src);

        /**
         * \brief 根据名称和 UUID 获取资源对象
         * \param [in] name : 给定资源名称 
         * \param [in] uuid : 给定 UUID
         * \return 返回查找到的资源对象
         */
        ResourcePtr getResource(const String &name, UUID uuid = UUID::INVALID) const;

        /**
         * \brief 根据 UUID 获取资源对象
         * \param [in] uuid : 给定的 UUID 
         * \return 返回查找到的资源对象
         */
        ResourcePtr getResource(UUID uuid) const;

        /**
         * \brief 添加一个资源对象到资源管理器里，主要用于在内存创建资源对象
         * \param [in] res : 资源对象 
         * \return 调用成功返回 T3D_OK
         */
        TResult addResource(ResourcePtr res);

    protected:
        /**
         * @brief 构造函数
         */
        ResourceManager() = default;

        ResourcePtr createResource(const String &name, int32_t argc, ...);

        /**
         * \brief 新生成一个资源对象，具体子类资源实现
         * \param [in] name : 资源名称
         * \param [in] argc : 生成资源参数数量 
         * \param [in] args : 资源参数列表
         * \return 返回一个新建资源对象
         */
        virtual ResourcePtr newResource(const String &name, int32_t argc, va_list args) = 0;
        
        /**
         * \brief 同步加载资源
         * \param [in] archive : 档案系统对象，资源文件从该档案系统读取
         * \param [in] name : 资源文件名
         * \return 返回加载出来的资源对象
         */
        ResourcePtr load(Archive *archive, const String &name);

        virtual ResourcePtr loadResource(Archive *archive, const String &name);

        /**
         * \brief 从数据流加载资源对象
         * \param [in] name : 资源名称
         * \param [in,out] stream : 数据流对象
         * \return 返回资源对象
         */
        virtual ResourcePtr loadResource(const String &name, DataStream &stream) = 0;

        /**
         * \brief 同步保存资源
         * \param [in] archive : 档案系统对象，资源文件保存到该档案系统里
         * \param [in] res : 要保存的资源对象
         * \return 调用成功返回 T3D_OK
         */
        TResult save(Archive *archive, Resource *res);

        /**
         * \brief 保存资源对象到数据流中
         * \param [in,out] stream : 数据流对象
         * \param [in] res : 要保存的资源对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult saveResource(DataStream &stream, Resource *res) = 0;

        /**
         * \brief 根据资源名称查找资源对象
         * \param [in] name : 给定的资源名称 
         * \return 查找到返回对应的资源对象，否则返回 nullptr
         */
        Resource *lookup(const String &name);

        /**
         * \brief 把资源对象加到缓存池中
         * \param [in] resource : 要加入缓存池的资源对象
         * \return 调用成功返回 true
         */
        bool insertCache(const ResourcePtr &resource);

        /**
         * \brief 从缓存池中移除资源对象
         * \param [in] resource : 要从缓存池移除的资源对象 
         */
        void removeCache(const ResourcePtr &resource);

    protected:
        using Resources = TList<Resource*> ;
        using ResourcesItr = Resources::iterator;
        using ResourcesConstItr = Resources::const_iterator;
        using ResourcesValue = Resources::value_type;

        using ResourcesLookup = TMap<String, Resources>;
        using ResourcesLookupItr = ResourcesLookup::iterator;
        using ResourcesLookupConstItr = ResourcesLookup::const_iterator;
        using ResourcesLookupValue = ResourcesLookup::value_type;

        using ResourcesCache = TMap<UUID, ResourcePtr>;
        using ResourcesCacheItr = ResourcesCache::iterator;
        using ResourcesCacheConstItr = ResourcesCache::const_iterator;
        using ResourcesCacheValue = ResourcesCache::value_type;

        /// 资源查找表
        ResourcesLookup mResourcesLookup;
        /// 资源缓存池
        ResourcesCache mResourcesCache;
    };
}


#endif  /*__T3D_RESOURCE_MANAGER_H__*/
