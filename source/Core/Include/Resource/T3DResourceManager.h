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
     * \brief 资源管理器基类，负责资源的创建、加载、保存、克隆、缓存与按文件名/UUID 查找
     */
    class T3D_ENGINE_API ResourceManager : public Object
    {
    public:
        /// 析构函数
        ~ResourceManager() override = default;

        /**
         * \brief 卸载指定资源
         * \param [in] res : 要卸载的资源对象
         * \return res 为 nullptr 时返回 T3D_ERR_RES_INVALID_OBJECT；onUnload 失败则返回对应错误码；否则返回 T3D_OK
         */
        virtual TResult unload(ResourcePtr res);
        
        /**
         * \brief 卸载缓存中所有已加载资源
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult unloadAllResources();

        /**
         * \brief 卸载引用计数仅剩管理器自身的未使用资源
         * \return 调用成功返回 T3D_OK
         * \remarks 仅处理状态为 kLoaded 且 referCount() == 1 的缓存项
         */
        virtual TResult unloadUnused();

        /**
         * \brief 克隆资源对象
         * \param [in] src : 源资源对象
         * \return 成功返回新克隆资源；src 为 nullptr 或入缓存失败时返回 nullptr
         */
        ResourcePtr clone(ResourcePtr src);

        /**
         * \brief 克隆资源并重命名
         * \param [in] newName : 克隆体的逻辑名称
         * \param [in] src : 源资源对象
         * \return 成功返回新克隆资源；src 为 nullptr 或入缓存失败时返回 nullptr
         */
        ResourcePtr clone(const String &newName, ResourcePtr src);

#if defined (T3D_EDITOR)
        /**
         * \brief 克隆资源、重命名并可选保留指定 UUID
         * \param [in] newName : 克隆体的逻辑名称
         * \param [in] src : 源资源对象
         * \param [in] uuid : 要使用的 UUID；为 UUID::INVALID 时保持克隆默认生成的新 UUID
         * \return 成功返回新克隆资源；src 为 nullptr 或入缓存失败时返回 nullptr
         * \note 须在 insertCache 之前设置 UUID，以保证缓存索引一致
         */
        ResourcePtr clone(const String &newName, ResourcePtr src, const UUID &uuid);
#endif

        /**
         * \brief 按文件名从 LUT 查找已缓存资源
         * \param [in] filename : 资源文件名或逻辑名称
         * \return 找到返回资源裸指针；未找到返回 nullptr
         * \remarks 克隆体不在 LUT 中，无法通过本接口按文件名查到
         */
        Resource *getResource(const String &filename) const;

        /**
         * \brief 按 UUID 从缓存池查找资源
         * \param [in] uuid : 资源 UUID
         * \return 找到返回资源裸指针；未找到返回 nullptr
         */
        Resource *getResource(const UUID &uuid) const;

        // TResult addResource(ResourcePtr res);

    protected:
        /// 默认构造
        ResourceManager() = default;

        /**
         * \brief 创建新资源并加入缓存
         * \param [in] name : 资源名称；若 LUT 中已存在同名资源则直接返回缓存项
         * \param [in] argc : 传给 newResource 的可变参数个数
         * \param [in] ... : 传给 newResource 的参数列表
         * \return 成功返回新建资源；创建或 onCreate 失败时返回 nullptr
         */
        ResourcePtr createResource(const String &name, int32_t argc, ...);

        /**
         * \brief 由子类实例化具体资源对象
         * \param [in] name : 资源名称
         * \param [in] argc : 可变参数个数
         * \param [in] args : 可变参数列表
         * \return 成功返回新建资源；子类实现决定失败时是否返回 nullptr
         */
        virtual ResourcePtr newResource(const String &name, int32_t argc, va_list args) = 0;

        /**
         * \brief 在资源加入缓存前覆盖默认 UUID
         * \param [in] resource : 刚创建、尚未 insertCache 的资源对象
         * \param [in] uuid : 要使用的 UUID；为 UUID::INVALID 时不修改
         * \note 须在 insertCache 之前调用，保证缓存索引与资源 UUID 一致
         */
        void applyCreationUUID(const ResourcePtr &resource, const UUID &uuid);
        
        /**
         * \brief 按文件名同步加载资源
         * \param [in] archive : 读取资源的档案对象
         * \param [in] filename : 资源文件名
         * \return 成功返回已加载资源；缓存命中、读档或 onLoad 失败时返回 nullptr
         */
        ResourcePtr load(Archive *archive, const String &filename);

        /**
         * \brief 从档案读取并反序列化资源（子类可覆写）
         * \param [in] archive : 读取资源的档案对象
         * \param [in] filename : 资源文件名
         * \return 成功返回资源对象并设置 filename；读档或反序列化失败时返回 nullptr
         */
        virtual ResourcePtr loadResource(Archive *archive, const String &filename);

        /**
         * \brief 按 UUID 同步加载资源
         * \param [in] archive : 读取资源的档案对象
         * \param [in] uuid : 资源 UUID
         * \return 成功返回已加载资源；缓存命中、读档或 onLoad 失败时返回 nullptr
         */
        ResourcePtr load(Archive *archive, const UUID &uuid);

        /**
         * \brief 按 UUID 从档案读取并反序列化资源（子类可覆写）
         * \param [in] archive : 读取资源的档案对象
         * \param [in] uuid : 资源 UUID
         * \return 成功返回资源对象；读档或反序列化失败时返回 nullptr
         */
        virtual ResourcePtr loadResource(Archive *archive, const UUID &uuid);
        
        /**
         * \brief 从数据流反序列化资源（须指定文件名）
         * \param [in] filename : 资源文件名，用于设置 Resource::mFilename
         * \param [in,out] stream : 输入数据流
         * \return 成功返回资源对象；子类实现决定失败时是否返回 nullptr
         */
        virtual ResourcePtr loadResource(const String &filename, DataStream &stream) = 0;

        /**
         * \brief 从数据流反序列化资源（基类未实现）
         * \param [in,out] stream : 输入数据流
         * \return 基类实现触发断言并返回 nullptr
         */
        virtual ResourcePtr loadResource(DataStream &stream);

        /**
         * \brief 将资源保存到指定文件名
         * \param [in] archive : 目标档案对象
         * \param [in] filename : 保存文件名
         * \param [in] res : 要保存的资源对象
         * \return 参数无效、onSave 或写档失败时返回对应错误码；否则返回 T3D_OK
         */
        TResult save(Archive *archive, const String &filename, Resource *res);

        /**
         * \brief 将资源保存到以其 UUID 为键的路径
         * \param [in] archive : 目标档案对象
         * \param [in] res : 要保存的资源对象
         * \return 参数无效、onSave 或写档失败时返回对应错误码；否则返回 T3D_OK
         */
        TResult save(Archive *archive, Resource *res);

        /**
         * \brief 将资源序列化写入数据流
         * \param [in,out] stream : 输出数据流
         * \param [in] res : 要保存的资源对象
         * \return 由子类实现决定
         */
        virtual TResult saveResource(DataStream &stream, Resource *res) = 0;

        /**
         * \brief 按文件名在 LUT 中查找资源（不触发加载）
         * \param [in] filename : 资源文件名或逻辑名称
         * \return 找到返回资源裸指针；否则返回 nullptr
         */
        Resource *lookup(const String &filename) const;
        
        /**
         * \brief 按 UUID 在缓存池中查找资源（不触发加载）
         * \param [in] uuid : 资源 UUID
         * \return 找到返回资源裸指针；否则返回 nullptr
         */
        Resource *lookup(const UUID &uuid) const;
        
        /**
         * \brief 将资源加入 UUID 缓存池
         * \param [in] uuid : 资源 UUID
         * \param [in] resource : 要缓存的资源对象
         * \return 始终返回 true；UUID 已存在时记录警告但仍更新 mResToUUIDMap
         * \remarks 非克隆资源会同时写入 LUT
         */
        bool insertCache(const UUID &uuid, const ResourcePtr &resource);

        /**
         * \brief 从缓存池与 LUT 移除资源
         * \param [in] resource : 要移除的资源对象
         */
        void removeCache(const ResourcePtr &resource);

        /**
         * \brief 将非克隆资源加入按文件名/名称索引的 LUT
         * \param [in] resource : 要索引的资源对象
         * \return 始终返回 true
         * \remarks 优先使用 getFilename() 作为键，文件名为空时使用 getName()
         */
        bool insertLUT(const ResourcePtr &resource);

        /**
         * \brief 从 LUT 移除指定键
         * \param [in] name : LUT 键（文件名或逻辑名称）
         */
        void removeLUT(const String &name);

    protected:        
        /// 资源查找表：String（文件路径或名称）→ Resource*
        using ResourcesLookup = TUnorderedMap<String, Resource*>;
        
        /// 资源缓存池：UUID → ResourcePtr
        using ResourcesCache = TUnorderedMap<UUID, ResourcePtr, UUIDHash, UUIDEqual>;

        /// 资源裸指针到 UUID 的反向映射
        using ResToUUIDMap = TUnorderedMap<Resource*, UUID>;

        /// 按文件名/名称索引的 LUT，避免重复读档
        ResourcesLookup mResourcesLookup {};
        /// UUID 缓存池
        ResourcesCache mResourcesCache {};
        /// 资源对象到 UUID 的映射
        ResToUUIDMap mResToUUIDMap {}; 
    };
}


#endif  /*__T3D_RESOURCE_MANAGER_H__*/
