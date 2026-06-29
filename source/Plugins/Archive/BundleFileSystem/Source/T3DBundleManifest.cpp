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


#include "T3DBundleManifest.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const BundleManifest::MAGIC = "T3D_BUNDLE_MANIFEST 1";

    // 别名行前缀：行格式为 "ALIAS <逻辑UUID> <目标UUID>"
    static const char * const ALIAS_TAG = "ALIAS";

    //--------------------------------------------------------------------------

    String BundleManifest::normalize(const String &path)
    {
        String result = path;
        for (auto &ch : result)
        {
            if (ch == '\\')
            {
                ch = '/';
            }
        }

        // 去掉开头的 "./"
        while (result.size() >= 2 && result[0] == '.' && result[1] == '/')
        {
            result = result.substr(2);
        }

        // 去掉开头多余的 '/'
        while (!result.empty() && result[0] == '/')
        {
            result = result.substr(1);
        }

        return result;
    }

    //--------------------------------------------------------------------------

    String BundleManifest::basename(const String &path)
    {
        String norm = normalize(path);
        size_t pos = norm.find_last_of('/');
        if (pos == String::npos)
        {
            return norm;
        }
        return norm.substr(pos + 1);
    }

    //--------------------------------------------------------------------------

    void BundleManifest::clear()
    {
        mPathToUUID.clear();
        mNameToUUID.clear();
        mAmbiguousNames.clear();
        mAliasByUUID.clear();
    }

    //--------------------------------------------------------------------------

    void BundleManifest::addAlias(const UUID &from, const UUID &to)
    {
        String key = from.toString();
        if (key.empty())
        {
            return;
        }
        mAliasByUUID[key] = to;
    }

    //--------------------------------------------------------------------------

    void BundleManifest::addEntry(const String &path, const UUID &uuid)
    {
        String key = normalize(path);
        if (key.empty())
        {
            return;
        }

        mPathToUUID[key] = uuid;

        // 维护文件名回退索引：相同文件名出现多次时标记为有歧义，不参与回退
        String name = basename(key);
        if (!name.empty())
        {
            if (mAmbiguousNames.find(name) != mAmbiguousNames.end())
            {
                // 已经是歧义文件名
            }
            else
            {
                auto it = mNameToUUID.find(name);
                if (it == mNameToUUID.end())
                {
                    mNameToUUID[name] = uuid;
                }
                else if (!(it->second == uuid))
                {
                    // 不同 UUID 的同名文件，标记歧义并从回退索引移除
                    mNameToUUID.erase(it);
                    mAmbiguousNames[name] = true;
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    bool BundleManifest::parse(DataStream &stream)
    {
        clear();

        // 第一行为魔数版本头，跳过（不做强校验，容忍后续版本演进）
        if (!stream.eof())
        {
            stream.readLine();
        }

        while (!stream.eof())
        {
            String line = stream.readLine();
            if (line.empty())
            {
                continue;
            }

            size_t p1 = line.find(' ');
            if (p1 == String::npos)
            {
                continue;
            }

            size_t p2 = line.find(' ', p1 + 1);
            if (p2 == String::npos)
            {
                continue;
            }

            String uuidStr = line.substr(0, p1);
            String midField = line.substr(p1 + 1, p2 - p1 - 1);
            String path = line.substr(p2 + 1);

            if (uuidStr.empty() || path.empty())
            {
                continue;
            }

            // 别名行："ALIAS <逻辑UUID> <目标UUID>"
            if (uuidStr == ALIAS_TAG)
            {
                if (midField.empty())
                {
                    continue;
                }
                UUID from, to;
                from.fromString(midField);
                to.fromString(path);
                addAlias(from, to);
                continue;
            }

            // 普通条目："<UUID> <类型整数> <相对路径>"，类型字段(midField)运行时不需要
            UUID uuid;
            uuid.fromString(uuidStr);
            addEntry(path, uuid);
        }

        return true;
    }

    //--------------------------------------------------------------------------

    bool BundleManifest::getUUIDByPath(const String &path, UUID &uuid) const
    {
        String key = normalize(path);

        auto it = mPathToUUID.find(key);
        if (it != mPathToUUID.end())
        {
            uuid = it->second;
            return true;
        }

        // 回退：按文件名查找（仅当文件名唯一）
        String name = basename(key);
        auto nit = mNameToUUID.find(name);
        if (nit != mNameToUUID.end())
        {
            uuid = nit->second;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool BundleManifest::exists(const String &path) const
    {
        UUID uuid;
        return getUUIDByPath(path, uuid);
    }

    //--------------------------------------------------------------------------

    bool BundleManifest::getRedirect(const UUID &uuid, UUID &target) const
    {
        auto it = mAliasByUUID.find(uuid.toString());
        if (it != mAliasByUUID.end())
        {
            target = it->second;
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------
}
