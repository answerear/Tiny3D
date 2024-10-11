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


namespace Tiny3D
{
    NS_BEGIN(Editor)
    
    class ProjectManager : public Singleton<ProjectManager>
    {
    public:
        ProjectManager();

        ~ProjectManager() override;

        TResult createProject(const String &path, const String &name);

        TResult openProject(const String &path, const String &name);

        TResult closeProject();

        const String &getProjectPath() const { return mPath; }

        const String &getProjectName() const { return mName; }

        const String &getAssetsPath() const { return mAssetsPath; }

        const String &getTempPath() const {return mTempPath; }

    protected:
        TResult compileAllShaders(const String &tempPath, const String &assetsPath);

        TResult compileShaders(const String &inputPath, const String &outputPath);

        TResult compileShader(const String &inputPath, const String &outputPath);

        TResult setupBuiltinAssets(const String &tempPath);

        TResult createSimpleScene(const String &assetsPath);
        
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

        /// Assets 档案系统
        ArchivePtr mAssetsArchive {nullptr};
        /// Temp 档案系统
        ArchivePtr mBuiltinArchive {nullptr};
        /// Shaders 编译后存放档案系统
        ArchivePtr mCompiledShadersArchive {nullptr};
    };

    #define PROJECT_MGR (ProjectManager::getInstance())

    NS_END
}
