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
     * @brief 资源加载门面，为应用层提供“一步加载”的统一入口
     * @remarks
     *   - 内部维护一个 CompositeArchive 搜索链，应用层无需感知 Archive。
     *   - editor 模式挂载 MetaFileSystem 搜索链，runtime 模式挂载 BundleFileSystem
     *     搜索链；挂载动作由上层（编辑器 / 应用启动流程）按模式触发。
     *   - loadXxx(name/uuid) 内部转调对应的资源管理器并传入组合档案，跨档案（跨包）
     *     的依赖加载自动生效。
     *   - 本类保持“来源无关”：只认 Archive 接口 + 优先级，不感知平台 / 包内 /
     *     可写路径 / AAssetManager 等存储细节。
     */
    class T3D_ENGINE_API AssetManager
        : public Object
        , public Singleton<AssetManager>
    {
    public:
        /**
         * @brief 门面运行模式
         */
        enum class Mode : uint32_t
        {
            /// 运行时模式，挂载只读的包内 / 补丁包搜索链
            kRuntime = 0,
            /// 编辑器模式，挂载可写的工程资源搜索链
            kEditor
        };

        /**
         * @brief 创建资源门面对象
         */
        static AssetManagerPtr create();

        /**
         * @brief 析构函数
         */
        ~AssetManager() override;

        /**
         * @brief 按模式初始化门面，创建对应的组合档案
         * @param [in] mode : 运行模式
         * @remarks 只准备好组合档案，不做实际挂载；挂载由上层调用 mount 完成
         */
        void init(Mode mode);

        /**
         * @brief 获取当前运行模式
         */
        Mode getMode() const { return mMode; }

        /**
         * @brief 挂载一个档案到搜索链
         * @param [in] archive : 要挂载的档案对象
         * @param [in] priority : 优先级，数值越小越先被搜索
         */
        void mount(Archive *archive, int32_t priority);

        /**
         * @brief 从搜索链卸载指定档案
         * @param [in] archive : 要卸载的档案对象
         */
        void unmount(Archive *archive);

        /**
         * @brief 卸载搜索链上的所有档案
         */
        void unmountAll();

        /**
         * @brief 获取内部组合档案对象
         * @remarks 供需要直接使用 Archive* 的少数场景（如保存资源）使用
         */
        Archive *getArchive() const;

        /**
         * @brief 根据文件名加载 mesh
         */
        MeshPtr loadMesh(const String &filename);

        /**
         * @brief 根据 UUID 加载 mesh
         */
        MeshPtr loadMesh(const UUID &uuid);

        /**
         * @brief 根据文件名加载纹理
         */
        TexturePtr loadTexture(const String &filename);

        /**
         * @brief 根据 UUID 加载纹理
         */
        TexturePtr loadTexture(const UUID &uuid);

        /**
         * @brief 根据文件名加载材质
         */
        MaterialPtr loadMaterial(const String &filename);

        /**
         * @brief 根据 UUID 加载材质
         */
        MaterialPtr loadMaterial(const UUID &uuid);

        /**
         * @brief 根据文件名加载着色器
         */
        ShaderPtr loadShader(const String &filename);

        /**
         * @brief 根据 UUID 加载着色器
         */
        ShaderPtr loadShader(const UUID &uuid);

        /**
         * @brief 根据文件名加载预制体
         */
        PrefabPtr loadPrefab(const String &name);

        /**
         * @brief 根据文件名加载场景
         */
        ScenePtr loadScene(const String &name);

        /**
         * @brief 根据 UUID 加载场景
         */
        ScenePtr loadScene(const UUID &uuid);

    protected:
        /**
         * @brief 构造函数
         */
        AssetManager() = default;

        /**
         * @brief 确保组合档案已创建
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
