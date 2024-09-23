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
     * @brief 资源管理器，统一管理资源
     */
    class T3D_ENGINE_API ResourceManager : public Object
    {
    public:
        /**
         * @brief 析构函数
         */
        ~ResourceManager() override = default;

        /**
         * @brief 卸载对应资源
         * @param [in] res : 要卸载的资源对象
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult unload(ResourcePtr res);
        
        /**
         * @brief 卸载所有资源
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult unloadAllResources();

        /**
         * @brief 卸载所有没用到的资源
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult unloadUnused();

        /**
         * @brief 克隆新资源对象
         * @param [in] src : 源资源对象 
         * @return 返回新资源对象
         */
        ResourcePtr clone(ResourcePtr src);

        /**
         * @brief 根据名称获取资源对象
         * @param [in] filename : 给定资源名称 
         * @return 返回查找到的资源对象
         */
        Resource *getResource(const String &filename) const;

        /**
         * @brief 根据 UUID 获取资源对象
         * @param [in] uuid : 给定的 UUID 
         * @return 返回查找到的资源对象
         */
        Resource *getResource(const UUID &uuid) const;

        /**
         * @brief 添加一个资源对象到资源管理器里，主要用于在内存创建资源对象
         * @param [in] res : 资源对象 
         * @return 调用成功返回 T3D_OK
         */
        // TResult addResource(ResourcePtr res);

    protected:
        /**
         * @brief 构造函数
         */
        ResourceManager() = default;

        /**
         * @brief 创建新资源
         * @param [in] name : 资源名称
         * @param [in] argc : 附带参数数量 
         * @param [in] ... : 参数列表
         * @return 调用成功返回新建资源对象，否则返回 nullptr
         */
        ResourcePtr createResource(const String &name, int32_t argc, ...);

        /**
         * @brief 新生成一个资源对象，具体子类资源实现
         * @param [in] name : 资源名称
         * @param [in] argc : 生成资源参数数量 
         * @param [in] args : 资源参数列表
         * @return 调用成功返回新建资源对象，否则返回 nullptr
         */
        virtual ResourcePtr newResource(const String &name, int32_t argc, va_list args) = 0;
        
        /**
         * @brief 根据文件名，同步加载资源
         * @param [in] archive : 档案系统对象，资源文件从该档案系统读取
         * @param [in] filename : 资源文件名
         * @return 调用成功返回加载的资源对象，否则返回 nullptr
         */
        ResourcePtr load(Archive *archive, const String &filename);

        /**
         * @brief 根据文件名，同步加载资源. 子类可以定制化加载对应类型资源的逻辑，也可以用默认逻辑
         * @param [in] archive : 档案系统对象，资源文件从该档案系统读取
         * @param [in] filename : 资源文件名
         * @return 调用成功返回加载的资源对象，否则返回 nullptr
         */
        virtual ResourcePtr loadResource(Archive *archive, const String &filename);

        /**
         * @brief 根据 UUID，同步加载资源
         * @param [in] archive : 档案系统对象，资源文件从该档案系统读取 
         * @param [in] uuid : 资源 UUID
         * @return 调用成功返回加载的资源对象，否则返回 nullptr
         */
        ResourcePtr load(Archive *archive, const UUID &uuid);

        /**
         * @brief 根据 UUID，同步加载资源
         * @param [in] archive : 档案系统对象，资源文件从该档案系统读取
         * @param [in] uuid : 资源 UUID
         * @return 调用成功返回加载的资源对象，否则返回 nullptr
         */
        virtual ResourcePtr loadResource(Archive *archive, const UUID &uuid);
        
        /**
         * @brief 从数据流加载资源对象
         * @param [in] filename : 文件名
         * @param [in,out] stream : 数据流对象
         * @return 调用成功返回加载的资源对象，否则返回 nullptr
         */
        virtual ResourcePtr loadResource(const String &filename, DataStream &stream) = 0;

        /**
         * @brief 从数据流加载资源对象
         * @param [in,out] stream : 数据流对象
         * @return 调用成功返回加载的资源对象，否则返回 nullptr
         */
        virtual ResourcePtr loadResource(DataStream &stream);

        /**
         * @brief 同步保存资源到指定文件名文件
         * @param [in] archive : 档案系统对象，资源文件保存到该档案系统里
         * @param [in] filename : 文件名
         * @param [in] res : 要保存的资源对象
         * @return 调用成功返回 T3D_OK
         */
        TResult save(Archive *archive, const String &filename, Resource *res);

        /**
         * @brief 同步保存资源，文件按照 UUID 来指定
         * @param [in] archive : 档案系统对象，资源文件保存到该档案系统里
         * @param [in] res : 要保存的资源对象
         * @return 调用成功返回 T3D_OK
         */
        TResult save(Archive *archive, Resource *res);

        /**
         * @brief 保存资源对象到数据流中
         * @param [in,out] stream : 数据流对象
         * @param [in] res : 要保存的资源对象
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult saveResource(DataStream &stream, Resource *res) = 0;

        /**
         * @brief 根据资源文件名查找资源对象
         * @param [in] filename : 给定的资源文件名
         * @return 查找到返回对应的资源对象，否则返回 nullptr
         */
        Resource *lookup(const String &filename) const;
        
        /**
         * @brief 根据 UUID 查找资源对象
         * @param [in] uuid : 给定的 UUID
         * @return 查找到返回对应的资源对象，否则返回 nullptr
         */
        Resource *lookup(const UUID &uuid) const;
        
        /**
         * @brief 把資源對象加到緩存池中
         * @param [in] resource : 要加入緩存池的資源對象
         * @return 調用成功返回 true 
         */
        bool insertCache(const ResourcePtr &resource);

        /**
         * @brief 从缓存池中移除资源对象
         * @param [in] resource : 要从缓存池移除的资源对象 
         */
        void removeCache(const ResourcePtr &resource);

        /**
         * @brief 把資源對象加入 Lookup Table 中
         * @param [in] resource : 要加入 LUT 的資源對象
         * @return 調用成功返回 true
         */
        bool insertLUT(const ResourcePtr &resource);

        /**
         * @brief 从 LUT 中移除资源对象
         * @param resource : 要从 LUT 移除的资源对象
         */
        void removeLUT(const String &name);

    protected:        
        /// 资源查找表容器类
        /// String : 文件路径
        /// Resources : 资源列表
        using ResourcesLookup = TUnorderedMap<String, Resource*>;
        
        /// 资源缓存池类
        using ResourcesCache = TUnorderedMap<UUID, ResourcePtr, UUIDHash, UUIDEqual>;

        /// 资源查找表，从文件加载过的，不再加载，避免重复读取文件加载
        ResourcesLookup mResourcesLookup;
        /// 资源缓存池
        ResourcesCache mResourcesCache;
    };
}


#endif  /*__T3D_RESOURCE_MANAGER_H__*/
