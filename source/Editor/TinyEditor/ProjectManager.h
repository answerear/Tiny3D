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

        const String &getMetaName() const { return mMetaName; }

        Meta *getMeta() const { return mMeta; } 

        AssetNode *removeChild(const String &name, bool destroy);

        void removeAllChildren() override;

        using TreeNode::addChild;
        using TreeNode::removeChild;
        using TreeNode::removeAllChildren;

        void destroy();

        bool readMeta();

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
        MetaPtr mMeta {nullptr};

        using WaitingForDestroyNodesLUT = TUnorderedMap<String, AssetNode *>;
        using WaitingForDestroyNodes = TList<AssetNode *>;

        static WaitingForDestroyNodesLUT msWaitingForDestroyNodesLUT;
        static WaitingForDestroyNodes msWaitingForDestroyNodes;
    };

    class ProjectManager
        : public Allocator
        , public Singleton<ProjectManager>
    {
    public:
        ProjectManager();

        ~ProjectManager() override;

        TResult createProject(const String &path, const String &name);

        TResult openProject(const String &path, const String &name);

        TResult closeProject();

        void update();

        const String &getProjectPath() const { return mPath; }

        const String &getProjectName() const { return mName; }

        const String &getAssetsPath() const { return mAssetsPath; }

        const String &getTempPath() const {return mTempPath; }

        const ProjectSettings &getProjectSettings() const { return mProjectSettings; }

        ProjectSettings &getProjectSettings() { return mProjectSettings; }

        AssetNode *getAssetRoot() const { return mAssetRoot; }

        TResult makeFolder(AssetNode *parent, const String &path, AssetNode *&node);

        TResult addFile(AssetNode *parent, const String &path, AssetNode *&node);

    protected:
        TResult compileAllShaders(const String &tempPath, const String &assetsPath);

        TResult compileShaders(const String &inputPath, const String &outputPath);

        TResult compileShader(const String &inputPath, const String &outputPath);

        TResult setupBuiltinAssets(const String &tempPath);

        TResult createSimpleScene(const String &assetsPath);

        TResult loadStartupScene();

        TResult populate();

        TResult populate(const String &path, AssetNode *parent, bool generateFolderNode, AssetNode *&node);

        TResult generateAssetNode(const String &path, AssetNode *parent, AssetNode *&node);
        
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
    };

    #define PROJECT_MGR (ProjectManager::getInstance())

    NS_END
}
