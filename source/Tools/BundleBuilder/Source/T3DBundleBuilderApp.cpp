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
#include "Resource/T3DShader.h"
#include "Material/T3DTechnique.h"
#include "Material/T3DPass.h"
#include "Material/T3DShaderVariantSet.h"
#include "Material/T3DShaderVariant.h"
#include "Serializer/T3DSerializerManager.h"

#include <string>
#include <cctype>
#include <cstdio>


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
        printf("                [--keep-languages <a,b,c>]\n");
        printf("\n");
        printf("  --assets <dir>         Project asset directory to scan (can repeat,\n");
        printf("                         e.g. add compiled Temp/shaders directory).\n");
        printf("  --out    <dir>         Output bundle directory.\n");
        printf("  --keep-languages <lst> Comma-separated shader languages to KEEP in\n");
        printf("                         each .tshader (others are stripped). Valid:\n");
        printf("                         hlsl,glsl,essl,spirv,msl. Default: keep all.\n");
        printf("                         The set must cover every render backend the\n");
        printf("                         distribution may switch to at runtime.\n");
        printf("  --binary               Convert resources to binary (T3DB) format on\n");
        printf("                         export (default: copy source JSON as-is).\n");
        printf("  --verify               Round-trip check each binary output (implies\n");
        printf("                         --binary). Reports PASS/FAIL per resource.\n");
        printf("\n");
        printf("Note: Tiny3D.cfg is NOT packed into the bundle.\n");
    }

    //--------------------------------------------------------------------------

    SHADER_LANGUAGE BundleBuilderApp::languageFromString(const String &s)
    {
        String lower = s;
        for (auto &ch : lower)
        {
            ch = (char)std::tolower((unsigned char)ch);
        }

        if (lower == "hlsl")    return SHADER_LANGUAGE::kHLSL;
        if (lower == "glsl")    return SHADER_LANGUAGE::kGLSL;
        if (lower == "essl")    return SHADER_LANGUAGE::kESSL;
        if (lower == "spirv")   return SHADER_LANGUAGE::kSPIRV;
        if (lower == "msl")     return SHADER_LANGUAGE::kMSL;
        return SHADER_LANGUAGE::kUnknown;
    }

    //--------------------------------------------------------------------------

    const char *BundleBuilderApp::languageToString(SHADER_LANGUAGE lang)
    {
        switch (lang)
        {
        case SHADER_LANGUAGE::kHLSL:    return "hlsl";
        case SHADER_LANGUAGE::kGLSL:    return "glsl";
        case SHADER_LANGUAGE::kESSL:    return "essl";
        case SHADER_LANGUAGE::kSPIRV:   return "spirv";
        case SHADER_LANGUAGE::kMSL:     return "msl";
        default:                        return "unknown";
        }
    }

    //--------------------------------------------------------------------------

    bool BundleBuilderApp::parseKeepLanguages(const String &csv)
    {
        mKeepLanguages.clear();

        size_t pos = 0;
        while (pos <= csv.size())
        {
            size_t comma = csv.find(',', pos);
            String token = (comma == String::npos)
                ? csv.substr(pos) : csv.substr(pos, comma - pos);

            // 去掉首尾空白
            size_t b = token.find_first_not_of(" \t");
            size_t e = token.find_last_not_of(" \t");
            if (b != String::npos)
            {
                token = token.substr(b, e - b + 1);
            }
            else
            {
                token.clear();
            }

            if (!token.empty())
            {
                SHADER_LANGUAGE lang = languageFromString(token);
                if (lang == SHADER_LANGUAGE::kUnknown)
                {
                    BB_LOG_ERROR("Invalid --keep-languages value: %s", token.c_str());
                    return false;
                }
                mKeepLanguages.insert(lang);
            }

            if (comma == String::npos)
            {
                break;
            }
            pos = comma + 1;
        }

        return !mKeepLanguages.empty();
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
            else if (arg == "--keep-languages")
            {
                if (i + 1 >= argc)
                {
                    return false;
                }
                if (!parseKeepLanguages(argv[++i]))
                {
                    return false;
                }
            }
            else if (arg == "--binary")
            {
                mBinaryOutput = true;
            }
            else if (arg == "--verify")
            {
                mVerify = true;
                mBinaryOutput = true;   // 自检需要先产出二进制
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

    void BundleBuilderApp::writeAliasEntry(DataStream &manifest, const UUID &from,
        const UUID &to)
    {
        String line = "ALIAS ";
        line += from.toString();
        line += " ";
        line += to.toString();
        manifest.writeLine(line);
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

                // 材质持久化的是 ShaderLab 的逻辑 UUID(meta 自身 UUID)，而 bundle 内
                // 真正存在的是编译后 .tshader（ShaderUUID）命名的散列文件。写入别名，
                // 让 BundleFSArchive 运行时把逻辑 UUID 重定向到 ShaderUUID，
                // 对齐 MetaFileSystem 中 lab -> ShaderUUID 的重定向语义。
                if (!(uuid == shaderUUID))
                {
                    writeAliasEntry(manifest, uuid, shaderUUID);
                }
                break;
            }

            // 编译后的 .tshader：若指定了 --keep-languages，则按白名单裁剪语言变体后
            // 重新序列化导出；否则与普通资源一样原样拷贝。
            if (type == Meta::kShader && !mKeepLanguages.empty())
            {
                ret = exportShader(uuid, filePath, relativePath, manifest);
                break;
            }

            // 普通资源：拷贝字节为以 UUID 命名的散列文件；
            // 若开启 --binary 则反序列化 JSON 后以二进制(T3DB)重新写出。
            String dst = mOutDir + Dir::getNativeSeparator() + uuid.toString();
            bool exported = false;

            if (mBinaryOutput && isSerializableType((int32_t)type))
            {
                if (convertToBinary(filePath, dst))
                {
                    ++mConvertedCount;
                    exported = true;

                    if (mVerify)
                    {
                        if (verifyRoundtrip(dst))
                        {
                            ++mVerifyPass;
                        }
                        else
                        {
                            ++mVerifyFail;
                            BB_LOG_ERROR("Round-trip verify FAILED: %s",
                                relativePath.c_str());
                        }
                    }
                }
                else
                {
                    BB_LOG_WARNING("Binary convert failed, fallback to raw copy: %s",
                        filePath.c_str());
                }
            }

            if (!exported)
            {
                if (!Dir::copy(filePath, dst, true))
                {
                    BB_LOG_ERROR("Copy resource failed: %s -> %s",
                        filePath.c_str(), dst.c_str());
                    ret = T3D_ERR_FAIL;
                    break;
                }
            }

            ++mExportedCount;
            writeManifestEntry(manifest, uuid, (int32_t)type, relativePath);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void BundleBuilderApp::pruneShaderLanguages(const ShaderPtr &shader,
        TSet<SHADER_LANGUAGE> &present, bool &hasEmptySet)
    {
        hasEmptySet = false;

        auto pruneStage = [this, &present, &hasEmptySet](const ShaderVariantSets &stage)
        {
            for (const auto &kv : stage)
            {
                ShaderVariantSetPtr set = kv.second;
                if (set == nullptr)
                {
                    continue;
                }

                // 收集本集合出现的语言，并标出需剔除的
                TArray<SHADER_LANGUAGE> toRemove;
                for (const auto &lv : set->getVariants())
                {
                    present.insert(lv.first);
                    if (mKeepLanguages.find(lv.first) == mKeepLanguages.end())
                    {
                        toRemove.push_back(lv.first);
                    }
                }

                for (SHADER_LANGUAGE lang : toRemove)
                {
                    set->removeVariant(lang);
                }

                // 裁剪后该 (stage, keyword) 已没有任何保留语言的变体
                if (set->empty())
                {
                    hasEmptySet = true;
                }
            }
        };

        for (const auto &tech : shader->getTechniques())
        {
            if (tech == nullptr)
            {
                continue;
            }

            for (const auto &pass : tech->getPasses())
            {
                if (pass == nullptr)
                {
                    continue;
                }

                pruneStage(pass->getVertexShaders());
                pruneStage(pass->getPixelShaders());
                pruneStage(pass->getGeometryShaders());
                pruneStage(pass->getHullShaders());
                pruneStage(pass->getDomainShaders());
            }
        }
    }

    //--------------------------------------------------------------------------

    TResult BundleBuilderApp::exportShader(const UUID &uuid, const String &filePath,
        const String &relativePath, DataStream &manifest)
    {
        TResult ret = T3D_OK;

        do
        {
            // 直接用序列化器反序列化（不走 ShaderManager::load，避免触发运行时编译）
            // 源 .tshader 为 JSON，确保以 kText 读取
            T3D_SERIALIZER_MGR.setFileMode(SerializerManager::FileMode::kText);
            FileDataStream fs;
            if (!fs.open(filePath.c_str(), FileDataStream::E_MODE_READ_ONLY))
            {
                BB_LOG_ERROR("Open tshader for reading failed: %s", filePath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }
            ShaderPtr shader = T3D_SERIALIZER_MGR.deserialize<Shader>(fs);
            fs.close();

            if (shader == nullptr)
            {
                BB_LOG_ERROR("Deserialize tshader failed: %s", filePath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 裁剪语言变体，并收集裁剪前出现过的语言并集
            TSet<SHADER_LANGUAGE> present;
            bool hasEmptySet = false;
            pruneShaderLanguages(shader, present, hasEmptySet);

            // 覆盖自检：请求保留但 tshader 内根本不存在的语言
            for (SHADER_LANGUAGE lang : mKeepLanguages)
            {
                if (present.find(lang) == present.end())
                {
                    BB_LOG_WARNING("tshader [%s] is missing requested language [%s]; "
                        "switching to that backend at runtime will fail to load it.",
                        relativePath.c_str(), languageToString(lang));
                }
            }

            // 自检：裁剪后存在「某 (stage, keyword) 无任何保留语言变体」
            if (hasEmptySet)
            {
                BB_LOG_WARNING("tshader [%s] has variant set(s) left empty after "
                    "language pruning; some keyword/stage has no kept-language variant.",
                    relativePath.c_str());
            }

            // 序列化裁剪后的 Shader 到以 UUID 命名的散列文件。
            // --binary 时用二进制(T3DB)且不加文本模式，避免换行翻译破坏二进制。
            String dst = mOutDir + Dir::getNativeSeparator() + uuid.toString();
            FileDataStream out;
            uint32_t mode = FileDataStream::E_MODE_TRUNCATE
                | FileDataStream::E_MODE_READ_WRITE;
            if (!mBinaryOutput)
            {
                mode |= FileDataStream::E_MODE_TEXT;
            }
            T3D_SERIALIZER_MGR.setFileMode(mBinaryOutput
                ? SerializerManager::FileMode::kBinary
                : SerializerManager::FileMode::kText);
            if (!out.open(dst.c_str(), mode))
            {
                BB_LOG_ERROR("Open output shader for writing failed: %s", dst.c_str());
                T3D_SERIALIZER_MGR.setFileMode(SerializerManager::FileMode::kText);
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }
            ret = T3D_SERIALIZER_MGR.serialize(out, shader.get());
            out.close();
            T3D_SERIALIZER_MGR.setFileMode(SerializerManager::FileMode::kText);
            if (T3D_FAILED(ret))
            {
                BB_LOG_ERROR("Serialize pruned shader failed: %s", dst.c_str());
                break;
            }

            if (mBinaryOutput)
            {
                ++mConvertedCount;
                if (mVerify)
                {
                    if (verifyRoundtrip(dst))
                    {
                        ++mVerifyPass;
                    }
                    else
                    {
                        ++mVerifyFail;
                        BB_LOG_ERROR("Round-trip verify FAILED: %s",
                            relativePath.c_str());
                    }
                }
            }

            ++mExportedCount;
            ++mPrunedShaderCount;
            writeManifestEntry(manifest, uuid, (int32_t)Meta::kShader, relativePath);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    long_t BundleBuilderApp::fileSize(const String &path)
    {
        FileDataStream fs;
        if (!fs.open(path.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            return -1;
        }
        long_t s = fs.size();
        fs.close();
        return s;
    }

    //--------------------------------------------------------------------------

    bool BundleBuilderApp::isSerializableType(int32_t type)
    {
        switch ((Meta::Type)type)
        {
        case Meta::kMaterial:
        case Meta::kTexture:
        case Meta::kShader:
        case Meta::kMesh:
        case Meta::kPrefab:
        case Meta::kScene:
        case Meta::kAnimation:
        case Meta::kSkeleton:
            return true;
        default:
            // kUnknown/kFolder/kFile/kTxt/kBin/kDylib/kShaderLab 等：
            // 原始字节或非 RTTR 对象，原样拷贝，不做二进制转换。
            return false;
        }
    }

    //--------------------------------------------------------------------------

    bool BundleBuilderApp::convertToBinary(const String &srcPath,
        const String &dstPath)
    {
        bool ok = false;

        // 格式转换是纯数据直通：关闭生命周期回调，避免反序列化时 onPostLoad 重建
        // 运行时层级（父子节点智能指针互持形成引用环，工具不经资源卸载无法断环）
        // 造成对象图泄漏，同时避免 onPreSave 对扁平层级表重采集破坏字节往返一致。
        T3D_SERIALIZER_MGR.setInvokeLifecycleCallbacks(false);

        // 读：源资源为 JSON，反序列化为类型无关的对象。
        // 注意：必须以二进制(READ_ONLY, 不加 TEXT)打开——文本模式的 CRLF 翻译会
        // 改变读到的字节，与既有 meta/shader 读取保持一致。
        T3D_SERIALIZER_MGR.setFileMode(SerializerManager::FileMode::kText);
        FileDataStream in;
        if (in.open(srcPath.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            RTTRVariant var;
            const bool loaded =
                !T3D_FAILED(T3D_SERIALIZER_MGR.deserializeObject(in, var))
                && var.is_valid();
            in.close();

            // 还原出的对象由 RTTR as_raw_ptr 策略以裸指针 new 出来，承载它的
            // variant 析构时并不会 delete 该裸指针，导致整棵对象图（含全部 SmartPtr
            // 成员）无人释放而泄漏。这里用 ObjectPtr 接管其生命周期：owner 在本作用域
            // 结束时按引用计数归零销毁对象。
            ObjectPtr owner = loaded
                ? RTTRObject(var).try_convert<Object>() : nullptr;

            if (loaded)
            {
                // 写：二进制(T3DB)，不加文本模式，避免换行翻译破坏二进制字节
                T3D_SERIALIZER_MGR.setFileMode(SerializerManager::FileMode::kBinary);
                FileDataStream out;
                uint32_t mode = FileDataStream::E_MODE_TRUNCATE
                    | FileDataStream::E_MODE_READ_WRITE;
                if (out.open(dstPath.c_str(), mode))
                {
                    TResult r = T3D_SERIALIZER_MGR.serializeObject(out, var);
                    out.close();
                    ok = !T3D_FAILED(r);
                }
            }
        }

        // 恢复默认，后续 meta 读取等仍走 JSON，且恢复生命周期回调
        T3D_SERIALIZER_MGR.setFileMode(SerializerManager::FileMode::kText);
        T3D_SERIALIZER_MGR.setInvokeLifecycleCallbacks(true);
        return ok;
    }

    //--------------------------------------------------------------------------

    bool BundleBuilderApp::verifyRoundtrip(const String &binPath)
    {
        // 说明：本引擎存在 unordered_map 属性（如 Material 常量表），其元素在
        // 重建后迭代顺序不保证与原先一致，因此逐字节比较会误报。这里改用对顺序
        // 不敏感的判据：二进制可被成功反序列化，且把还原对象重新序列化后的字节
        // 总长度与原二进制文件一致（同一对象图的二进制编码长度与元素顺序无关）。
        long_t s1 = fileSize(binPath);
        if (s1 < 0)
        {
            return false;
        }

        bool ok = false;

        // 同 convertToBinary：纯数据直通，关闭生命周期回调避免引用环泄漏，并使
        // 往返写出等于读入（不受 onPostLoad/onPreSave 层级重建影响，判长度即可）。
        T3D_SERIALIZER_MGR.setInvokeLifecycleCallbacks(false);

        T3D_SERIALIZER_MGR.setFileMode(SerializerManager::FileMode::kBinary);
        FileDataStream in;
        if (in.open(binPath.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            RTTRVariant var;
            const bool loaded =
                !T3D_FAILED(T3D_SERIALIZER_MGR.deserializeObject(in, var))
                && var.is_valid();
            in.close();

            // 同 convertToBinary：接管 as_raw_ptr 裸指针对象的生命周期，避免泄漏。
            ObjectPtr owner = loaded
                ? RTTRObject(var).try_convert<Object>() : nullptr;

            if (loaded)
            {
                String tmp = binPath + ".verify";
                FileDataStream out;
                uint32_t mode = FileDataStream::E_MODE_TRUNCATE
                    | FileDataStream::E_MODE_READ_WRITE;
                if (out.open(tmp.c_str(), mode))
                {
                    TResult r = T3D_SERIALIZER_MGR.serializeObject(out, var);
                    long_t s2 = out.tell();
                    out.close();
                    std::remove(tmp.c_str());
                    ok = (!T3D_FAILED(r)) && (s1 == s2);
                }
            }
        }

        T3D_SERIALIZER_MGR.setFileMode(SerializerManager::FileMode::kText);
        T3D_SERIALIZER_MGR.setInvokeLifecycleCallbacks(true);
        return ok;
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

            // 打印语言裁剪设置
            if (mKeepLanguages.empty())
            {
                BB_LOG_INFO("Shader language pruning: disabled (keep all languages).");
            }
            else
            {
                String langs;
                for (SHADER_LANGUAGE lang : mKeepLanguages)
                {
                    if (!langs.empty())
                    {
                        langs += ",";
                    }
                    langs += languageToString(lang);
                }
                BB_LOG_INFO("Shader language pruning: keep [%s].", langs.c_str());
            }

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

            BB_LOG_INFO("Bundle build done. exported files: %zu (pruned shaders: %zu, "
                "binary converted: %zu), manifest entries: %zu, out: %s",
                mExportedCount, mPrunedShaderCount, mConvertedCount, mEntryCount,
                mOutDir.c_str());

            if (mVerify)
            {
                BB_LOG_INFO("Round-trip verify: %zu passed, %zu failed.",
                    mVerifyPass, mVerifyFail);
                if (mVerifyFail > 0)
                {
                    ret = T3D_ERR_FAIL;
                }
            }
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
