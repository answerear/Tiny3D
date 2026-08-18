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

#pragma once


#include "EditorPrerequisites.h"
#include "ProjectSettings.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    class AssetNode
        : public TreeNode<String, AssetNode>
        , public Allocator
    {
    public:
        static void GC();
        
        AssetNode() = default;
        
        AssetNode(const String &name);

        AssetNode(const String &name, const String &path, Meta *meta);

        ~AssetNode() override = default;

        const String &getKey() const override { return getMetaName(); }

        const String &getPath() const { buildPath(); return mPath; }

        const String &getFilename() const { return mFilename; }

        const String &getTitle() const { return mTitle; }

        const String &getMetaName() const { return mMetaName; }

        String getFullPath() const;

        Meta *getMeta() const { return mMeta; } 

        AssetNode *removeChild(const String &name, bool destroy);

        void removeAllChildren() override;

        using TreeNode::addChild;
        using TreeNode::removeChild;
        using TreeNode::removeAllChildren;

        void destroy();

        bool readMeta();

        void setUserData(void *data) { mUserData = data; }

        void *getUserData() const { return mUserData; }

        void debugOutput(int32_t depth = 0);
        
    protected:
        void buildPath() const;

        bool addChild(AssetNode *node, const Action &action) override;

        bool insertAfterChild(AssetNode *prevNode, pointer_t node, const Action &action) override;

        bool removeChild(AssetNode *node, const Action &action) override;

        void removeAllChildren(const Action &removeAction, const Action &deleteAction = nullptr) override;

        static void putWaitingForDestroyNode(AssetNode *node);
        
    protected:
        mutable bool mIsPathDirty {true};
        mutable String mPath {};
        String mMetaName {};
        String mFilename {};
        String mTitle {};
        MetaPtr mMeta {nullptr};
        void *mUserData {nullptr};

        using WaitingForDestroyNodesLUT = TUnorderedMap<String, AssetNode *>;
        using WaitingForDestroyNodes = TList<AssetNode *>;

        static WaitingForDestroyNodesLUT msWaitingForDestroyNodesLUT;
        static WaitingForDestroyNodes msWaitingForDestroyNodes;
    };

    class ProjectManager
        : public EventHandler
        , public Allocator
        , public Singleton<ProjectManager>
    {
    public:
        static const char *BUILTIN_CUBE_MESH_NAME;
        static const char *BUILTIN_CUBE_SUBMESH_NAME;
        static const char *BUILTIN_SPHERE_MESH_NAME;
        static const char *BUILTIN_SPHERE_SUBMESH_NAME;
        static const char *BUILTIN_CAPSULE_MESH_NAME;
        static const char *BUILTIN_CAPSULE_SUBMESH_NAME;
        static const char *BUILTIN_CYLINDER_MESH_NAME;
        static const char *BUILTIN_CYLINDER_SUBMESH_NAME;
        static const char *BUILTIN_QUAD_MESH_NAME;
        static const char *BUILTIN_QUAD_SUBMESH_NAME;
        static const char *BUILTIN_PLANE_MESH_NAME;
        static const char *BUILTIN_PLANE_SUBMESH_NAME;
        static const char *BUILTIN_SKYBOX_MATERIAL_NAME;
        
        ProjectManager();

        ~ProjectManager() override;

        TResult createProject(const String &path, const String &name);

        TResult openProject(const String &path, const String &name);

        TResult saveProject();

        TResult closeProject();

        bool isProjectOpened() const { return !mPath.empty(); }

        bool isSceneModified() const { return mIsSceneModified; }

        void setSceneModified(bool modified) { mIsSceneModified = modified; }

        bool isProjectModified() const { return isSceneModified(); }
        
        void update();

        void applicationDidEnterBackground();

        void applicationWillEnterForeground();

        void applicationFocusGained();

        void applicationFocusLost();

        const String &getProjectPath() const { return mPath; }

        const String &getProjectName() const { return mName; }

        const String &getAssetsPath() const { return mAssetsPath; }

        const String &getTempPath() const { return mTempPath; }

        const String &getBuiltinPath() const { return mBuiltinPath; }

        const ProjectSettings &getProjectSettings() const { return mProjectSettings; }

        ProjectSettings &getProjectSettings() { return mProjectSettings; }

        AssetNode *getAssetRoot() const { return mAssetRoot; }

        /**
         * @brief 获取内置资源的资产树根节点
         * @remarks 内置资源与工程资产挂在不同的档案上（见 mountAssetArchives），
         *          资产树也因此分成两棵。需要按 UUID 反查资产的地方（如 inspector
         *          的资源引用字段）必须同时检索这两棵树，否则引用了内置资源的对象
         *          会被误判成引用丢失
         */
        AssetNode *getBuiltinAssetRoot() const { return mBuiltinRoot; }

        TResult makeFolder(AssetNode *parent, const String &path, AssetNode *&node);

        TResult removeFolder(AssetNode *node);

        TResult addFile(AssetNode *parent, const String &path, AssetNode *&node);

        /**
         * @brief 判断节点是否就是工程的业务 C++ 根目录（Assets/Source）
         * @remarks C++ Class 菜单只允许在这个目录下创建，子目录和其它资产目录都不算
         */
        bool isCppSourceRoot(const AssetNode *node) const;

        /**
         * @brief 在 parent 目录下创建一对同名 C++ 类文件（.h + .cpp）
         * @param [in] parent : 目标文件夹节点，必须是 Assets/Source
         * @param [out] headerNode : 新建头文件的资产节点
         * @param [out] sourceNode : 新建源文件的资产节点
         */
        TResult createCppClass(AssetNode *parent, AssetNode *&headerNode, AssetNode *&sourceNode);

        /**
         * @brief 重新扫描 Assets 目录，重建资产树
         * @remarks 在编辑器内新增或删除资产文件后调用。调用方还需要广播
         *          kEvtRefreshAssets 让 Project 视图跟着重建，否则 UI 上还是旧的树
         */
        void refreshAssets();

        /**
         * @brief 确保 Assets 下的相对目录及其 meta 文件都存在
         * @param [in] relativePath : 相对于 Assets 的目录路径，支持多级
         * @remarks 通过档案写入资产时只会补齐目录本身，目录的 meta 要等文件监控异步补上，
         *          而资产树扫描要求每个目录都有 meta，所以写资产前先同步补齐
         */
        TResult ensureAssetFolder(const String &relativePath);

    protected:
        /// 目录还没有 meta 文件时生成一个，已存在则保留原有 UUID
        TResult writeFolderMeta(const String &path);

        /// 通用文件还没有 meta 时生成 MetaFile，已存在则保留原有 UUID
        TResult writeFileMeta(const String &path);

        TResult compileAllShaders(const String &tempPath, const String &assetsPath);

        TResult compileShaders(const String &inputPath, const String &outputPath);

        TResult compileShader(const String &inputPath, const String &outputPath);

        TResult setupBuiltinAssets(const String &tempPath);

        /**
         * @brief 从模板生成业务代码工程（Assets/Source 目录）
         * @remarks 模板在编辑器安装目录的 Editor/templates/GamePlugin 下，复制过来后
         *          把里面的 {ProjectName} 占位符换成工程名。生成失败不影响工程本身可用，
         *          只是没有业务代码而已
         */
        TResult createGamePluginScaffold();

        /**
         * @brief 把工程名规整成合法的 C++ / CMake 标识符
         * @remarks 工程名允许有空格和中文，但要拿来当 CMake 目标名和动态库文件名，
         *          非法字符一律换成下划线，数字开头再补一个前缀
         */
        static String sanitizeIdentifier(const String &name);

        /// 把文件里所有 {ProjectName} 占位符替换成实际工程名
        static TResult replaceTemplatePlaceholders(const String &filepath,
            const String &pluginName);

        /// 递归遍历模板目录，逐个文件做占位符替换
        static TResult replaceTemplatePlaceholdersInDir(const String &dir,
            const String &pluginName);

        /**
         * @brief 把工程的三个档案按优先级挂载到资源门面搜索链
         * @remarks 优先级：assets > compiledShaders > builtin
         */
        void mountAssetArchives();

        TResult createSimpleScene(const String &assetsPath);

        TResult loadStartupScene();

        TResult populate();

        /**
         * @brief 构建内置资源的资产树
         * @remarks 只在打开 / 新建工程时调用一次。内置资源是在 setupBuiltinAssets 里
         *          一次性复制到 Temp 下的，会话期间不会变，因此不随 populate 在窗口
         *          重新获得焦点时一起重建
         */
        TResult populateBuiltin();

        TResult populate(const String &path, AssetNode *parent, bool generateFolderNode, AssetNode *&node);

        TResult generateAssetNode(const String &path, AssetNode *parent, AssetNode *&node);

        void registerAllEvents();

        bool onModifiedScene(EventParam *param, TINSTANCE sender);
        
    protected:
        static const char *ASSETS;
        static const char *SCENES;
        static const char *TEMP;
        
        /// 文件系统监控器
        FileSystemMonitor *mFSMonitor {nullptr};
        
        /// 工程路径
        String mPath {};
        /// 工程名称
        String mName {};
        /// Assets 路径
        String mAssetsPath {};
        /// 工程临时文件路径
        String mTempPath {};
        /// 内置资源路径
        String mBuiltinPath {};
        /// shader 编译后临时文件路径
        String mCompiledShadersPath {};

        /// 工程设置
        ProjectSettings mProjectSettings {};

        /// Assets 档案系统
        ArchivePtr mAssetsArchive {nullptr};
        /// Temp 档案系统
        ArchivePtr mBuiltinArchive {nullptr};
        /// Shaders 编译后存放档案系统
        ArchivePtr mCompiledShadersArchive {nullptr};

        AssetNode *mAssetRoot {nullptr};
        /// 内置资源的资产树根节点
        AssetNode *mBuiltinRoot {nullptr};

        /// 场景是否被修改标记
        bool mIsSceneModified {false};
    };

    #define PROJECT_MGR (ProjectManager::getInstance())

    NS_END
}
