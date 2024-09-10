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


#include "T3DMetaFSArchivePrerequisites.h"


namespace Tiny3D
{
    using UUIDMap = TUnorderedMap<UUID, String, UUIDHash, UUIDEqual>;
    using PathMap = TUnorderedMap<String, UUID>;
    
    TSTRUCT()
    struct Mappings
    {
        /// UUID => 文件路径映射表
        UUIDMap uuids {};
        
        /// 文件路径 => UUID 映射表
        TPROPERTY()
        PathMap paths {};
    };
    
    class MFSProjectManager : public IProjectManager, public Singleton<MFSProjectManager>
    {
    public:
        MFSProjectManager();

        ~MFSProjectManager() override;

        TResult createProject(const String &path, const String &name) override;

        TResult openProject(const String &path, const String &name) override;

        TResult closeProject() override;

        const String &getProjectPath() const override { return mPath; }

        const String &getProjectName() const override { return mName; }

        const String &getAssetsPath() const override { return mAssetsPath; }

        const String &getTempPath() const override {return mTempPath; }

        void refresh(const UUID &uuid, const String &path);

        const String &getPathByUUID(const UUID &uuid) const;

        const UUID &getUUIDByPath(const String &path) const;
        
    protected:
        /// 扫描对应路径的文件，建立映射关系
        TResult buildMappings();

        /// 读映射文件
        TResult readMappings();

        /// 写映射文件
        TResult writeMappings();
        
    protected:
        static const char *ASSETS;
        static const char *SCENES;
        static const char *TEMP;
        static const char *MAPPINGS_FILE_NAME;
        
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

        /// 工程根目录
        String mProjectPath {};
        /// 映射表
        Mappings mMappings {};
        /// 是否刷新而写文件
        bool mIsDirty {false};
    };

    #define PROJECT_MGR (MFSProjectManager::getInstance())
}
