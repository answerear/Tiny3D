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


#include "Kernel/T3DCompositeArchive.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const CompositeArchive::ARCHIVE_TYPE = "Composite";

    //--------------------------------------------------------------------------

    CompositeArchivePtr CompositeArchive::create(const String &name, AccessMode mode)
    {
        CompositeArchivePtr archive = T3D_NEW CompositeArchive(name, mode);
        return archive;
    }

    //--------------------------------------------------------------------------

    CompositeArchive::CompositeArchive(const String &name, AccessMode mode)
        : Archive(name, mode)
    {

    }

    //--------------------------------------------------------------------------

    CompositeArchive::~CompositeArchive()
    {
        unmountAll();
    }

    //--------------------------------------------------------------------------

    bool CompositeArchive::isWritable(const Archive *archive)
    {
        if (archive == nullptr)
        {
            return false;
        }

        uint32_t mode = (uint32_t)archive->getAccessMode();
        uint32_t writeMode = (uint32_t)AccessMode::kAppend
            | (uint32_t)AccessMode::kTruncate;
        return (mode & writeMode) != 0;
    }

    //--------------------------------------------------------------------------

    void CompositeArchive::mount(Archive *archive, int32_t priority)
    {
        if (archive == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_ENGINE,
                "Mount a null archive to composite archive [%s] !",
                getName().c_str());
            return;
        }

        // 按优先级升序插入，数值越小越先被搜索
        Entry entry;
        entry.archive = archive;
        entry.priority = priority;

        auto itr = mEntries.begin();
        while (itr != mEntries.end() && itr->priority <= priority)
        {
            ++itr;
        }
        mEntries.insert(itr, entry);
    }

    //--------------------------------------------------------------------------

    void CompositeArchive::unmount(Archive *archive)
    {
        for (auto itr = mEntries.begin(); itr != mEntries.end(); ++itr)
        {
            if (itr->archive == archive)
            {
                mEntries.erase(itr);
                break;
            }
        }
    }

    //--------------------------------------------------------------------------

    void CompositeArchive::unmountAll()
    {
        mEntries.clear();
    }

    //--------------------------------------------------------------------------

    String CompositeArchive::getArchiveType() const
    {
        return ARCHIVE_TYPE;
    }

    //--------------------------------------------------------------------------

    String CompositeArchive::getPath() const
    {
        return getName();
    }

    //--------------------------------------------------------------------------

    ArchivePtr CompositeArchive::clone() const
    {
        CompositeArchivePtr archive = create(getName(), getAccessMode());

        // 逐个克隆子档案，保持原有优先级顺序
        for (const auto &entry : mEntries)
        {
            if (entry.archive != nullptr)
            {
                ArchivePtr sub = entry.archive->clone();
                archive->mount(sub, entry.priority);
            }
        }

        return archive;
    }

    //--------------------------------------------------------------------------

    bool CompositeArchive::exists(const String &name) const
    {
        for (const auto &entry : mEntries)
        {
            if (entry.archive != nullptr && entry.archive->exists(name))
            {
                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------

    TResult CompositeArchive::read(const String &name,
        const ArchiveReadCallback &callback, void *userData)
    {
        TResult ret = T3D_ERR_FILE_NOT_EXIST;

        // 按优先级从高到低依次尝试，命中即返回
        for (const auto &entry : mEntries)
        {
            if (entry.archive == nullptr)
            {
                continue;
            }

            ret = entry.archive->read(name, callback, userData);
            if (ret == T3D_OK)
            {
                return T3D_OK;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult CompositeArchive::write(const String &name,
        const ArchiveWriteCallback &callback, void *userData)
    {
        // 写回默认落到优先级最高（第一个）的子档案
        // 编辑器下第一个即工程 assets 路径（可写）；运行时挂载的是只读搜索链，
        // 此处直接断言不支持写入
        if (mEntries.empty())
        {
            T3D_ASSERT(false);
            T3D_LOG_ERROR(LOG_TAG_ENGINE,
                "Write to an empty composite archive [%s] is not supported !",
                name.c_str());
            return T3D_ERR_NOT_WRAITABLE_FILE;
        }

        Archive *first = mEntries.front().archive;
        if (!isWritable(first))
        {
            T3D_ASSERT(false);
            T3D_LOG_ERROR(LOG_TAG_ENGINE,
                "The highest priority archive is read-only, "
                "write [%s] is not supported !", name.c_str());
            return T3D_ERR_NOT_WRAITABLE_FILE;
        }

        return first->write(name, callback, userData);
    }

    //--------------------------------------------------------------------------

    TResult CompositeArchive::read(const UUID &uuid,
        const ArchiveReadCallback &callback, void *userData)
    {
        TResult ret = T3D_ERR_FILE_NOT_EXIST;

        // 按优先级从高到低依次尝试，命中即返回
        for (const auto &entry : mEntries)
        {
            if (entry.archive == nullptr)
            {
                continue;
            }

            ret = entry.archive->read(uuid, callback, userData);
            if (ret == T3D_OK)
            {
                return T3D_OK;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult CompositeArchive::write(const UUID &uuid,
        const ArchiveWriteCallback &callback, void *userData)
    {
        // 写回默认落到优先级最高（第一个）的子档案
        if (mEntries.empty())
        {
            T3D_ASSERT(false);
            T3D_LOG_ERROR(LOG_TAG_ENGINE,
                "Write to an empty composite archive [%s] is not supported !",
                getName().c_str());
            return T3D_ERR_NOT_WRAITABLE_FILE;
        }

        Archive *first = mEntries.front().archive;
        if (!isWritable(first))
        {
            T3D_ASSERT(false);
            T3D_LOG_ERROR(LOG_TAG_ENGINE,
                "The highest priority archive is read-only, "
                "write by uuid is not supported !");
            return T3D_ERR_NOT_WRAITABLE_FILE;
        }

        return first->write(uuid, callback, userData);
    }

    //--------------------------------------------------------------------------
}
