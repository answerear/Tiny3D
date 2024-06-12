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


#include "LauncherPrerequisites.h"


namespace Tiny3D
{
    NS_BEGIN(Launcher)

    TSTRUCT(RTTRConstructAsPointer)
    struct ProjectInfo
    {
        TPROPERTY()
        int64_t     recent {0};

        TPROPERTY()
        bool        favourites {false};

        TPROPERTY()
        String      name {};

        TPROPERTY()
        String      path {};

        TPROPERTY()
        String      tags {};
    };

    using Projects = TArray<ProjectInfo*>;

    TENUM()
    enum class SortType : uint32_t
    {
        kRecent = 0,
        kName,
        kPath,
        kTag
    };

    TSTRUCT()
    struct ProjectData
    {
        /// 用于保存的项目列表，按照创建时间排序
        TPROPERTY()
        Projects    projects {};
        
        /// 当前排序形式
        TPROPERTY()
        SortType    sortType {SortType::kRecent};
        
        /// 升序降序排序
        TPROPERTY()
        bool        ascending {true};
    };
    
    class ProjectManager
        : public EventHandler
        , public Singleton<ProjectManager>
    {
    public:
        ProjectManager();

        ~ProjectManager() override;
        
        TResult loadProjects();

        TResult saveProjects();

        TResult createProject(const String &path, const String &name);

        TResult removeProject(const String &path, const String &name);

        TResult removeProject(int32_t index);

        TResult openProject(const String &path, const String &name);

        const Projects &getProjects() const { return mProjectData.projects; }
        
        const Projects &getDisplayProjects() const { return mDisplayProjects; }

        SortType getSortType() const { return mProjectData.sortType; }
        
        void setSortType(SortType type) { mProjectData.sortType = type; }

        bool isSortAscending() const { return mProjectData.ascending; }
        
        void setSortAscending(bool ascending) { mProjectData.ascending = ascending; }

        void sort();

        TResult startTinyEditor(const String &path, const String &name, bool isNewProject);
        
    protected:        
        void releaseProjectInfo();
        
    protected:
        /// 用于展示的，按照指定排序规则排序的结果
        Projects    mDisplayProjects {};
        
        /// 要存储的项目数据
        ProjectData mProjectData {};

        static const char *PROJECT_DATA_FILE;
    };

    #define PROJECT_MGR     ProjectManager::getInstance()
    
    NS_END
}
