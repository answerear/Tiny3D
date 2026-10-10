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

#include "T3DRmlFileInterface.h"

#include "Kernel/T3DArchive.h"
#include "Resource/T3DAssetManager.h"

#include <filesystem>
#include <string>
#include <system_error>
#include <unordered_map>

namespace Tiny3D
{
    struct RmlFileInterface::WatchMap
    {
        std::unordered_map<std::string, std::filesystem::file_time_type> Files;
    };

    //--------------------------------------------------------------------------

    static bool endsWithIgnoreCase(const Rml::String &path, const char *suffix)
    {
        const size_t length = std::char_traits<char>::length(suffix);
        if (path.size() < length)
        {
            return false;
        }

        const size_t offset = path.size() - length;
        for (size_t i = 0; i < length; ++i)
        {
            unsigned char left = static_cast<unsigned char>(path[offset + i]);
            unsigned char right = static_cast<unsigned char>(suffix[i]);
            if (left >= 'A' && left <= 'Z')
            {
                left = static_cast<unsigned char>(left - 'A' + 'a');
            }
            if (right >= 'A' && right <= 'Z')
            {
                right = static_cast<unsigned char>(right - 'A' + 'a');
            }
            if (left != right)
            {
                return false;
            }
        }
        return true;
    }

    //--------------------------------------------------------------------------

    static bool diskFile(const Rml::String &source, std::filesystem::path &out)
    {
        Archive *archive = T3D_ASSET_MGR.getArchive();
        if (archive == nullptr)
        {
            return false;
        }

        const String type = archive->getArchiveType();
        if (type != "FileSystem" && type != "MetaFileSystem")
        {
            return false;
        }

        const String root = archive->getPath();
        if (root.empty())
        {
            return false;
        }

        out = std::filesystem::path(root.c_str()) / std::filesystem::path(source.c_str());
        std::error_code error;
        return std::filesystem::is_regular_file(out, error);
    }

    //--------------------------------------------------------------------------

    RmlFileInterface::RmlFileInterface()
        : mWatch(T3D_NEW WatchMap())
    {
    }

    //--------------------------------------------------------------------------

    RmlFileInterface::~RmlFileInterface()
    {
        T3D_DELETE mWatch;
        mWatch = nullptr;
    }

    //--------------------------------------------------------------------------

    void RmlFileInterface::watch(const Rml::String &path)
    {
        if (mWatch == nullptr)
        {
            return;
        }
        if (!endsWithIgnoreCase(path, ".rml") && !endsWithIgnoreCase(path, ".rcss"))
        {
            return;
        }
        if (mWatch->Files.find(path) != mWatch->Files.end())
        {
            return;
        }

        std::filesystem::path disk;
        if (!diskFile(path, disk))
        {
            return;
        }

        std::error_code error;
        const auto stamp = std::filesystem::last_write_time(disk, error);
        if (error)
        {
            return;
        }
        mWatch->Files.emplace(path, stamp);
    }

    //--------------------------------------------------------------------------

    RmlFileInterface::Change RmlFileInterface::pollChanges()
    {
        if (mWatch == nullptr || mWatch->Files.empty())
        {
            return Change::None;
        }

        bool style = false;
        bool document = false;
        for (auto &entry : mWatch->Files)
        {
            std::filesystem::path disk;
            if (!diskFile(entry.first, disk))
            {
                continue;
            }

            std::error_code error;
            const auto stamp = std::filesystem::last_write_time(disk, error);
            if (error || stamp == entry.second)
            {
                continue;
            }

            entry.second = stamp;
            if (endsWithIgnoreCase(entry.first, ".rml"))
            {
                document = true;
            }
            else
            {
                style = true;
            }
        }

        if (document)
        {
            return Change::Document;
        }
        return style ? Change::Style : Change::None;
    }

    //--------------------------------------------------------------------------
    struct RmlFile
    {
        TArray<uint8_t> Data {};
        size_t Pos {0};
    };

    //--------------------------------------------------------------------------

    static bool readWholeFile(const Rml::String &path, TArray<uint8_t> &out)
    {
        out.clear();
        Archive *archive = T3D_ASSET_MGR.getArchive();
        if (archive == nullptr)
        {
            return false;
        }

        struct Payload
        {
            TArray<uint8_t> *Bytes {nullptr};
            bool Ok {false};
        } payload;
        payload.Bytes = &out;

        TResult ret = archive->read(path, [](DataStream &stream, const String &, void *userData) -> TResult
        {
            auto *payload = static_cast<Payload *>(userData);
            const long_t length = stream.size();
            if (length < 0)
            {
                return T3D_ERR_FAIL;
            }

            payload->Bytes->resize(static_cast<size_t>(length));
            if (length > 0)
            {
                const size_t got = stream.read(payload->Bytes->data(), payload->Bytes->size());
                if (got != payload->Bytes->size())
                {
                    return T3D_ERR_FAIL;
                }
            }

            payload->Ok = true;
            return T3D_OK;
        }, &payload);

        return ret == T3D_OK && payload.Ok;
    }

    //--------------------------------------------------------------------------

    Rml::FileHandle RmlFileInterface::Open(const Rml::String &path)
    {
        TArray<uint8_t> bytes;
        if (!readWholeFile(path, bytes))
        {
            T3D_LOG_WARNING(LOG_TAG_RMLUI, "Open failed: %s", path.c_str());
            return 0;
        }

        watch(path);
        auto *file = T3D_NEW RmlFile();
        file->Data.swap(bytes);
        return reinterpret_cast<Rml::FileHandle>(file);
    }

    //--------------------------------------------------------------------------

    void RmlFileInterface::Close(Rml::FileHandle file)
    {
        T3D_DELETE reinterpret_cast<RmlFile *>(file);
    }

    //--------------------------------------------------------------------------

    size_t RmlFileInterface::Read(void *buffer, size_t size, Rml::FileHandle file)
    {
        auto *src = reinterpret_cast<RmlFile *>(file);
        if (src == nullptr || buffer == nullptr || src->Pos >= src->Data.size())
        {
            return 0;
        }

        const size_t remain = src->Data.size() - src->Pos;
        const size_t count = size < remain ? size : remain;
        memcpy(buffer, src->Data.data() + src->Pos, count);
        src->Pos += count;
        return count;
    }

    //--------------------------------------------------------------------------

    bool RmlFileInterface::Seek(Rml::FileHandle file, long offset, int origin)
    {
        auto *src = reinterpret_cast<RmlFile *>(file);
        if (src == nullptr)
        {
            return false;
        }

        const long length = static_cast<long>(src->Data.size());
        long next = static_cast<long>(src->Pos);
        if (origin == SEEK_SET)
        {
            next = offset;
        }
        else if (origin == SEEK_CUR)
        {
            next += offset;
        }
        else if (origin == SEEK_END)
        {
            next = length + offset;
        }
        else
        {
            return false;
        }

        if (next < 0 || next > length)
        {
            return false;
        }

        src->Pos = static_cast<size_t>(next);
        return true;
    }

    //--------------------------------------------------------------------------

    size_t RmlFileInterface::Tell(Rml::FileHandle file)
    {
        auto *src = reinterpret_cast<RmlFile *>(file);
        return src != nullptr ? src->Pos : 0;
    }

    //--------------------------------------------------------------------------

    size_t RmlFileInterface::Length(Rml::FileHandle file)
    {
        auto *src = reinterpret_cast<RmlFile *>(file);
        return src != nullptr ? src->Data.size() : 0;
    }

    //--------------------------------------------------------------------------

    bool RmlFileInterface::LoadFile(const Rml::String &path, Rml::String &out_data)
    {
        TArray<uint8_t> bytes;
        if (!readWholeFile(path, bytes))
        {
            return false;
        }

        watch(path);
        out_data.assign(reinterpret_cast<const char *>(bytes.data()), bytes.size());
        return true;
    }

    //--------------------------------------------------------------------------
}
