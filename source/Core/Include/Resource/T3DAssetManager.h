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


#ifndef __T3D_ASSET_MANAGER_H__
#define __T3D_ASSET_MANAGER_H__


#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 资源加载门面，为应用层提供基于组合档案的一步加载入口
     * \remarks 内部维护 CompositeArchive 搜索链；loadXxx 转调对应 ResourceManager 并传入组合档案，应用层无需直接操作 Archive
     */
    class T3D_ENGINE_API AssetManager
        : public Object
        , public Singleton<AssetManager>
    {
    public:
        /**
         * \brief 门面运行模式
         */
        enum class Mode : uint32_t
        {
            /// 运行时模式：组合档案以只读方式创建
            kRuntime = 0,
            /// 编辑器模式：组合档案允许读写（kReadTruncate）
            kEditor
        };

        /**
         * \brief 创建 AssetManager 单例对象
         * \return 新建的 AssetManager 智能指针
         */
        static AssetManagerPtr create();

        /**
         * \brief 析构并卸载组合档案上的所有挂载
         */
        ~AssetManager() override;

        /**
         * \brief 按模式初始化并创建组合档案
         * \param [in] mode : 运行模式，决定组合档案的访问模式
         * \remarks 仅创建组合档案，不执行 mount；实际挂载由上层按模式调用 mount
         */
        void init(Mode mode);

        /// 获取当前运行模式
        Mode getMode() const { return mMode; }

        /**
         * \brief 将档案挂载到搜索链
         * \param [in] archive : 要挂载的档案对象
         * \param [in] priority : 搜索优先级，数值越小越先被搜索
         */
        void mount(Archive *archive, int32_t priority);

        /**
         * \brief 从搜索链卸载指定档案
         * \param [in] archive : 要卸载的档案对象
         */
        void unmount(Archive *archive);

        /**
         * \brief 卸载搜索链上的所有档案
         */
        void unmountAll();

        /**
         * \brief 获取内部组合档案对象
         * \return 组合档案裸指针；未 init 且尚未 mount 时可能为 nullptr，ensureArchive 后会创建
         */
        Archive *getArchive() const;

        /**
         * \brief 按文件名加载 Mesh
         * \param [in] filename : 资源文件名
         * \return 成功返回 Mesh 智能指针；加载失败时返回 nullptr
         */
        MeshPtr loadMesh(const String &filename);

        /**
         * \brief 按 UUID 加载 Mesh
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Mesh 智能指针；加载失败时返回 nullptr
         */
        MeshPtr loadMesh(const UUID &uuid);

        /**
         * \brief 按文件名加载 Texture
         * \param [in] filename : 资源文件名
         * \return 成功返回 Texture 智能指针；加载失败时返回 nullptr
         */
        TexturePtr loadTexture(const String &filename);

        /**
         * \brief 按 UUID 加载 Texture
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Texture 智能指针；加载失败时返回 nullptr
         */
        TexturePtr loadTexture(const UUID &uuid);

        /**
         * \brief 按文件名加载 Material
         * \param [in] filename : 资源文件名
         * \return 成功返回 Material 智能指针；加载失败时返回 nullptr
         */
        MaterialPtr loadMaterial(const String &filename);

        /**
         * \brief 按 UUID 加载 Material
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Material 智能指针；加载失败时返回 nullptr
         */
        MaterialPtr loadMaterial(const UUID &uuid);

        /**
         * \brief 按文件名加载 Shader
         * \param [in] filename : 资源文件名
         * \return 成功返回 Shader 智能指针；加载失败时返回 nullptr
         */
        ShaderPtr loadShader(const String &filename);

        /**
         * \brief 按 UUID 加载 Shader
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Shader 智能指针；加载失败时返回 nullptr
         */
        ShaderPtr loadShader(const UUID &uuid);

        /**
         * \brief 按文件名加载 Image
         * \param [in] filename : 资源文件名
         * \return 成功返回 Image 智能指针；加载失败时返回 nullptr
         */
        ImagePtr loadImage(const String &filename);

        /**
         * \brief 按 UUID 加载 Image
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Image 智能指针；加载失败时返回 nullptr
         */
        ImagePtr loadImage(const UUID &uuid);

        /**
         * \brief 新建 Prefab 资源并纳入资源管理器缓存
         * \param [in] name : 资源名称，一般用预制体文件名
         * \param [in] root : 预制体根 GameObject，可为 nullptr 后续再设置
         * \return 成功返回 Prefab 智能指针，失败返回 nullptr
         */
        PrefabPtr createPrefab(const String &name, GameObjectPtr root = nullptr);

        /**
         * \brief 按文件名加载 Prefab
         * \param [in] name : 预制体文件名
         * \return 成功返回 Prefab 智能指针；加载失败时返回 nullptr
         */
        PrefabPtr loadPrefab(const String &name);

        /**
         * \brief 按 UUID 加载 Prefab
         * \param [in] uuid : 预制体资源 UUID
         * \return 成功返回 Prefab 智能指针；加载失败时返回 nullptr
         */
        PrefabPtr loadPrefab(const UUID &uuid);

        /**
         * \brief 按文件名保存 Prefab
         * \param [in] name : 目标文件名
         * \param [in] prefab : 待保存预制体
         * \return 成功返回 T3D_OK
         */
        TResult savePrefab(const String &name, Prefab *prefab);

        /**
         * \brief 按文件名加载 Scene
         * \param [in] name : 场景文件名
         * \return 成功返回 Scene 智能指针；加载失败时返回 nullptr
         */
        ScenePtr loadScene(const String &name);

        /**
         * \brief 按 UUID 加载 Scene
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Scene 智能指针；加载失败时返回 nullptr
         */
        ScenePtr loadScene(const UUID &uuid);

    protected:
        /// 默认构造
        AssetManager() = default;

        /**
         * \brief 懒创建组合档案
         * \remarks mArchive 为 nullptr 时按 mMode 创建 CompositeArchive
         */
        void ensureArchive();

    protected:
        /// 当前运行模式
        Mode                    mMode {Mode::kRuntime};
        /// 组合档案搜索链
        CompositeArchivePtr     mArchive {nullptr};
    };

    #define T3D_ASSET_MGR   (AssetManager::getInstance())

    T3D_EXTERN_SINGLETON(AssetManager)
}


#endif  /*__T3D_ASSET_MANAGER_H__*/
