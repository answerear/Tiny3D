/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "T3DBundleBuilderApp.h"
#include "Meta/T3DMeta.h"
#include "Meta/T3DMetaShaderLab.h"

#include <string>


Tiny3D::BundleBuilderApp theApp;


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    BundleBuilderApp::BundleBuilderApp()
    {
    }

    //--------------------------------------------------------------------------

    BundleBuilderApp::~BundleBuilderApp()
    {
    }

    //--------------------------------------------------------------------------

    void BundleBuilderApp::printUsage() const
    {
        printf("Usage:\n");
        printf("  bundlebuilder --assets <dir> [--assets <dir> ...] --out <dir>\n");
        printf("\n");
        printf("  --assets <dir>  Project asset directory to scan (can repeat,\n");
        printf("                  e.g. add compiled Temp/shaders directory).\n");
        printf("  --out    <dir>  Output bundle directory.\n");
        printf("\n");
        printf("Note: Tiny3D.cfg is NOT packed into the bundle.\n");
    }

    //--------------------------------------------------------------------------

    bool BundleBuilderApp::parseArgs(int32_t argc, char *argv[])
    {
        for (int32_t i = 1; i < argc; ++i)
        {
            String arg = argv[i];
            if (arg == "--assets")
            {
                if (i + 1 >= argc)
                {
                    return false;
                }
                mAssetRoots.push_back(Dir::formatPath(argv[++i]));
            }
            else if (arg == "--out")
            {
                if (i + 1 >= argc)
                {
                    return false;
                }
                mOutDir = Dir::formatPath(argv[++i]);
            }
            else if (arg == "-h" || arg == "--help")
            {
                return false;
            }
            else
            {
                BB_LOG_WARNING("Unknown argument: %s", arg.c_str());
                return false;
            }
        }

        return !mAssetRoots.empty() && !mOutDir.empty();
    }

    //--------------------------------------------------------------------------

    bool BundleBuilderApp::createDirectories(const String &path)
    {
        if (path.empty())
        {
            return false;
        }

        String norm = Dir::formatPath(path);
        char sep = Dir::getNativeSeparator();

        size_t pos = 0;
        while (true)
        {
            size_t next = norm.find(sep, pos);
            String sub = (next == String::npos) ? norm : norm.substr(0, next);

            // 跳过空段以及形如 "D:" 的盘符段
            bool isDriveOnly = (sub.size() == 2 && sub[1] == ':');
            if (!sub.empty() && !isDriveOnly)
            {
                if (!Dir::exists(sub))
                {
                    Dir::makeDir(sub);
                }
            }

            if (next == String::npos)
            {
                break;
            }
            pos = next + 1;
        }

        return Dir::exists(norm);
    }

    //--------------------------------------------------------------------------

    String BundleBuilderApp::makeRelative(const String &root, const String &filePath)
    {
        String rel = filePath;
        if (filePath.size() > root.size()
            && filePath.compare(0, root.size(), root) == 0)
        {
            rel = filePath.substr(root.size());
        }

        // 去掉开头的路径分隔符
        while (!rel.empty() && (rel[0] == '/' || rel[0] == '\\'))
        {
            rel = rel.substr(1);
        }

        // 统一为 '/' 分隔符
        for (auto &ch : rel)
        {
            if (ch == '\\')
            {
                ch = '/';
            }
        }

        return rel;
    }

    //--------------------------------------------------------------------------

    void BundleBuilderApp::writeManifestEntry(DataStream &manifest, const UUID &uuid,
        int32_t type, const String &relativePath)
    {
        String line = uuid.toString();
        line += " ";
        line += std::to_string(type);
        line += " ";
        line += relativePath;
        manifest.writeLine(line);
        ++mEntryCount;
    }

    //--------------------------------------------------------------------------

    TResult BundleBuilderApp::processFile(const String &root, const String &filePath,
        DataStream &manifest)
    {
        TResult ret = T3D_OK;

        do
        {
            String dir, title, ext;
            if (!Dir::parsePath(filePath, dir, title, ext))
            {
                break;
            }

            // meta 文件本身不导出
            if (ext == "meta")
            {
                break;
            }

            String metaPath = filePath + ".meta";
            if (!Dir::exists(metaPath))
            {
                BB_LOG_WARNING("Skip file without meta: %s", filePath.c_str());
                break;
            }

            // 读取 meta，获取 UUID 与类型
            MetaPtr meta;
            FileDataStream fs;
            if (!fs.open(metaPath.c_str(), FileDataStream::E_MODE_READ_ONLY))
            {
                BB_LOG_ERROR("Open meta file failed: %s", metaPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }
            meta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
            fs.close();

            if (meta == nullptr)
            {
                BB_LOG_ERROR("Deserialize meta failed: %s", metaPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            const UUID &uuid = meta->getUUID();
            Meta::Type type = meta->getType();
            String relativePath = makeRelative(root, filePath);

            if (type == Meta::kShaderLab)
            {
                // ShaderLab 源文件运行时不使用，材质引用的是编译后的 .tshader
                // (其 UUID 即 ShaderUUID)。这里只把 名字 -> ShaderUUID 写入清单，
                // 编译后的 .tshader 在扫描其所在目录时以自身 UUID 导出。
                MetaShaderLab *lab = static_cast<MetaShaderLab *>(meta.get());
                const UUID &shaderUUID = lab->getShaderUUID();
                if (shaderUUID == UUID::INVALID)
                {
                    BB_LOG_WARNING("ShaderLab [%s] has no compiled ShaderUUID, "
                        "skip manifest entry.", filePath.c_str());
                    break;
                }
                writeManifestEntry(manifest, shaderUUID, (int32_t)type, relativePath);
                break;
            }

            // 普通资源：拷贝字节为以 UUID 命名的散列文件
            String dst = mOutDir + Dir::getNativeSeparator() + uuid.toString();
            if (!Dir::copy(filePath, dst, true))
            {
                BB_LOG_ERROR("Copy resource failed: %s -> %s",
                    filePath.c_str(), dst.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            ++mExportedCount;
            writeManifestEntry(manifest, uuid, (int32_t)type, relativePath);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BundleBuilderApp::scanDir(const String &root, const String &dir,
        DataStream &manifest)
    {
        TResult ret = T3D_OK;

        String searchPath = dir + Dir::getNativeSeparator() + "*.*";
        Dir d;
        bool working = d.findFile(searchPath);
        while (working)
        {
            if (d.isDots())
            {
                // . or ..
            }
            else if (d.isDirectory())
            {
                scanDir(root, d.getFilePath(), manifest);
            }
            else
            {
                processFile(root, d.getFilePath(), manifest);
            }

            working = d.findNextFile();
        }
        d.close();

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BundleBuilderApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
    {
        TResult ret = T3D_OK;

        do
        {
            if (!parseArgs(argc, argv))
            {
                printUsage();
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            // 确保输出目录存在
            if (!createDirectories(mOutDir))
            {
                BB_LOG_ERROR("Create output directory failed: %s", mOutDir.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 打开清单文件准备写入
            String manifestPath = mOutDir + Dir::getNativeSeparator() + BUNDLE_MANIFEST_NAME;
            FileDataStream manifest;
            uint32_t mode = FileDataStream::E_MODE_TRUNCATE
                | FileDataStream::E_MODE_READ_WRITE
                | FileDataStream::E_MODE_TEXT;
            if (!manifest.open(manifestPath.c_str(), mode))
            {
                BB_LOG_ERROR("Open manifest for writing failed: %s", manifestPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            // 写魔数版本头
            manifest.writeLine(BUNDLE_MANIFEST_MAGIC);

            // 逐个根目录扫描导出
            for (const String &root : mAssetRoots)
            {
                if (!Dir::exists(root))
                {
                    BB_LOG_WARNING("Asset root not found, skip: %s", root.c_str());
                    continue;
                }
                BB_LOG_INFO("Scanning asset root: %s", root.c_str());
                scanDir(root, root, manifest);
            }

            manifest.close();

            BB_LOG_INFO("Bundle build done. exported files: %zu, manifest entries: %zu, out: %s",
                mExportedCount, mEntryCount, mOutDir.c_str());
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void BundleBuilderApp::applicationDidEnterBackground()
    {
    }

    //--------------------------------------------------------------------------

    void BundleBuilderApp::applicationWillEnterForeground()
    {
    }

    //--------------------------------------------------------------------------

    void BundleBuilderApp::applicationWillTerminate()
    {
    }

    //--------------------------------------------------------------------------

    void BundleBuilderApp::applicationLowMemory()
    {
    }

    //--------------------------------------------------------------------------
}
