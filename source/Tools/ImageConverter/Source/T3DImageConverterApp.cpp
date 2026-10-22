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


#include "T3DImageConverterApp.h"
#include "T3DImageConverterCommand.h"
#include "T3DImageImporter.h"
#include "T3DTextureExporter.h"
#include "T3DImportSummary.h"
#include "T3DImgconvIPCServer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImageConverterApp::ImageConverterApp() = default;

    //--------------------------------------------------------------------------

    ImageConverterApp::~ImageConverterApp()
    {
        mImporter = nullptr;
        mExporter = nullptr;
    }

    //--------------------------------------------------------------------------

    TResult ImageConverterApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
    {
        ImageConverterOptions options;
        ImageConverterCommand cmd;
        String parseError;
        if (!cmd.parse(argc, argv, options, &parseError))
        {
            if (!parseError.empty())
            {
                fprintf(stderr, "Error: %s\n\n", parseError.c_str());
            }
            cmd.printHelp();
            mExitCode = 2;
            return T3D_ERR_INVALID_PARAM;
        }

        if (options.showHelp)
        {
            cmd.printHelp();
            mExitCode = 0;
            return T3D_OK;
        }

        mImporter = ImageImporter::create();
        mExporter = TextureExporter::create();

        if (options.ipcMode)
        {
            ImgconvIPCServer server(this, options.ipcPort, options.ipcToken);
            mExitCode = server.run();
            mImporter = nullptr;
            mExporter = nullptr;
            return (mExitCode == 0) ? T3D_OK : T3D_ERR_FAIL;
        }

        std::vector<ImportItemRecord> records;
        uint64_t totalElapsedMs = 0;
        bool allOk = processBatch(options, records, totalElapsedMs, nullptr);

        for (size_t i = 0; i < records.size(); ++i)
        {
            ImportSummary::printItemLine(i + 1, records.size(), records[i], options.verbose);
        }
        ImportSummary::printBatchSummary(records, totalElapsedMs);

        mImporter = nullptr;
        mExporter = nullptr;
        mExitCode = allOk ? 0 : 1;
        return allOk ? T3D_OK : T3D_ERR_FAIL;
    }

    //--------------------------------------------------------------------------

    void ImageConverterApp::applicationDidEnterBackground()
    {
    }

    //--------------------------------------------------------------------------

    void ImageConverterApp::applicationWillEnterForeground()
    {
    }

    //--------------------------------------------------------------------------

    void ImageConverterApp::applicationWillTerminate()
    {
    }

    //--------------------------------------------------------------------------

    void ImageConverterApp::applicationLowMemory()
    {
    }

    //--------------------------------------------------------------------------

    bool ImageConverterApp::processBatch(
        const ImageConverterOptions &options,
        std::vector<ImportItemRecord> &outRecords,
        uint64_t &outTotalElapsedMs,
        const ImageBatchCallbacks *callbacks)
    {
        outRecords.clear();
        outRecords.reserve(options.srcPaths.size());

        const int64_t batchStartMs = DateTime::currentMSecsSinceEpoch();
        bool allSuccess = true;
        const uint32_t total = static_cast<uint32_t>(options.srcPaths.size());

        for (size_t i = 0; i < options.srcPaths.size(); ++i)
        {
            const String &srcPath = options.srcPaths[i];
            const uint32_t idx1 = static_cast<uint32_t>(i + 1);

            ImportItemRecord rec = processSingleImage(idx1, total, srcPath, options, callbacks);
            if (!rec.success)
            {
                allSuccess = false;
            }

            if (callbacks != nullptr && callbacks->onItemResult)
            {
                callbacks->onItemResult(rec);
            }

            outRecords.push_back(std::move(rec));
        }

        const int64_t batchEndMs = DateTime::currentMSecsSinceEpoch();
        outTotalElapsedMs = (batchEndMs >= batchStartMs)
            ? static_cast<uint64_t>(batchEndMs - batchStartMs)
            : 0;

        return allSuccess;
    }

    //--------------------------------------------------------------------------

    ImportItemRecord ImageConverterApp::processSingleImage(
        uint32_t index1Based,
        uint32_t totalCount,
        const String &srcPath,
        const ImageConverterOptions &options,
        const ImageBatchCallbacks *callbacks)
    {
        const int64_t itemStartMs = DateTime::currentMSecsSinceEpoch();

        ImportItemRecord rec;
        rec.srcPath = srcPath;
        rec.dstPath = resolveOutputTtexPath(srcPath, options);

        auto failWith = [&](const String &errMsg) -> ImportItemRecord
        {
            rec.success = false;
            rec.errorMessage = errMsg;
            int64_t nowMs = DateTime::currentMSecsSinceEpoch();
            rec.elapsedMs = (nowMs >= itemStartMs) ? static_cast<uint64_t>(nowMs - itemStartMs) : 0;
            if (callbacks != nullptr && callbacks->onLog)
            {
                callbacks->onLog("error", errMsg);
            }
            return rec;
        };

        if (callbacks != nullptr && callbacks->onProgress)
        {
            callbacks->onProgress(index1Based, totalCount, "decode", srcPath);
        }

        if (!Dir::exists(srcPath))
        {
            return failWith("Source file not found: " + srcPath);
        }

        // 1. 计算源图 MD5 (SourceHash)
        bool md5Ok = false;
        const String sourceHash = computeFileMD5(srcPath, md5Ok);
        if (!md5Ok)
        {
            return failWith("Failed to read source file for MD5 calculation: " + srcPath);
        }

        // 2. 检查目标 .ttex.meta 是否已存在，若存在则尝试加载 MetaTexture
        const String dstMetaPath = rec.dstPath + ".meta";
        MetaTexturePtr existingMeta = nullptr;
        if (Dir::exists(dstMetaPath))
        {
            FileDataStream fs;
            if (fs.open(dstMetaPath.c_str(), FileDataStream::E_MODE_READ_ONLY))
            {
                MetaPtr loadedMeta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
                fs.close();
                if (loadedMeta != nullptr && loadedMeta->getType() == Meta::Type::kTexture)
                {
                    existingMeta = smart_pointer_cast<MetaTexture>(loadedMeta);
                }
            }
        }

        // 3. 合并最终生效的 TextureImportSettings
        const TextureImportSettings *existingSettingsPtr =
            (existingMeta != nullptr) ? &existingMeta->getImportSettings() : nullptr;
        const TextureImportSettings effectiveSettings = options.resolveSettings(existingSettingsPtr);

        // 4. 决定本次使用的 UUID（§5.3 规则）
        UUID targetUUID = UUID::INVALID;
        if (options.explicitUUID != UUID::INVALID)
        {
            targetUUID = options.explicitUUID;
        }
        else if (options.uuidPolicy == ImageConverterOptions::UUIDPolicy::kReuse
                 && existingMeta != nullptr
                 && existingMeta->getUUID() != UUID::INVALID)
        {
            targetUUID = existingMeta->getUUID();
        }
        else
        {
            targetUUID = UUID::generate();
        }

        rec.uuid = targetUUID.toString();
        rec.premultipliedAlpha = effectiveSettings.PremultipliedAlpha;

        // 5. 增量跳过检查（§5.5 规则）：
        // 未传 -f，且 .ttex 与 .ttex.meta 均存在，且 SourceHash 一致，且 ImportSettings 完全一致，且 UUID 一致，且当前 .ttex 文件格式（文本/二进制）与期望一致
        auto isExistingFormatMatching = [&]() -> bool
        {
            FileDataStream fs;
            if (!fs.open(rec.dstPath.c_str(), FileDataStream::E_MODE_READ_ONLY))
            {
                return false;
            }
            uint8_t magic[4] = { 0, 0, 0, 0 };
            size_t n = fs.read(magic, sizeof(magic));
            fs.close();
            bool existingIsBinary = (n == sizeof(magic)
                && magic[0] == 'T' && magic[1] == '3'
                && magic[2] == 'D' && magic[3] == 'B');
            return existingIsBinary == (!options.isTextMode);
        };

        if (!options.forceRebuild
            && Dir::exists(rec.dstPath)
            && existingMeta != nullptr
            && existingMeta->getUUID() == targetUUID
            && !existingMeta->getSourceHash().empty()
            && existingMeta->getSourceHash() == sourceHash
            && isSettingsEqual(existingMeta->getImportSettings(), effectiveSettings)
            && isExistingFormatMatching())
        {
            // 从已有 .ttex 或默认填充尺寸信息（若需要，可快速保留 0 或读取）
            rec.success = true;
            rec.skipped = true;
            int64_t nowMs = DateTime::currentMSecsSinceEpoch();
            rec.elapsedMs = (nowMs >= itemStartMs) ? static_cast<uint64_t>(nowMs - itemStartMs) : 0;
            return rec;
        }

        if (callbacks != nullptr && callbacks->onProgress)
        {
            callbacks->onProgress(index1Based, totalCount, "convert", srcPath);
        }

        // 6. 导入源图生成 Texture2D
        String dstDir, dstTitle, dstExt;
        Dir::parsePath(rec.dstPath, dstDir, dstTitle, dstExt);

        Texture2DPtr texture = nullptr;
        String importErr;
        if (T3D_FAILED(mImporter->importTexture(srcPath, dstTitle, effectiveSettings, targetUUID, texture, &importErr))
            || texture == nullptr)
        {
            return failWith(importErr.empty() ? ("Failed to import image: " + srcPath) : importErr);
        }

        rec.width = static_cast<uint32_t>(texture->getWidth());
        rec.height = static_cast<uint32_t>(texture->getHeight());
        rec.mipLevels = static_cast<uint32_t>(texture->getMipmaps());

        if (callbacks != nullptr && callbacks->onProgress)
        {
            callbacks->onProgress(index1Based, totalCount, "export", srcPath);
        }

        // 7. 导出 .ttex 与（可选）配对 .ttex.meta
        String exportErr;
        if (T3D_FAILED(mExporter->exportTexture(
            texture,
            rec.dstPath,
            options.isGeneratingMeta,
            sourceHash,
            effectiveSettings,
            existingMeta,
            &exportErr,
            options.isTextMode)))
        {
            T3D_TEXTURE_MGR.unload(texture);
            return failWith(exportErr.empty() ? ("Failed to export texture: " + rec.dstPath) : exportErr);
        }

        T3D_TEXTURE_MGR.unload(texture);

        rec.success = true;
        rec.skipped = false;
        int64_t nowMs = DateTime::currentMSecsSinceEpoch();
        rec.elapsedMs = (nowMs >= itemStartMs) ? static_cast<uint64_t>(nowMs - itemStartMs) : 0;
        return rec;
    }

    //--------------------------------------------------------------------------

    String ImageConverterApp::resolveOutputTtexPath(
        const String &srcPath,
        const ImageConverterOptions &options)
    {
        if (!options.dstFilePath.empty())
        {
            return options.dstFilePath;
        }

        String srcDir, title, ext;
        Dir::parsePath(srcPath, srcDir, title, ext);

        const String outFileName = title + "." + Resource::EXT_TEXTURE;
        const String targetDir = options.dstDir.empty() ? srcDir : options.dstDir;

        if (targetDir.empty())
        {
            return outFileName;
        }

        char lastChar = targetDir.back();
        if (lastChar == '/' || lastChar == '\\')
        {
            return targetDir + outFileName;
        }
        return targetDir + "/" + outFileName;
    }

    //--------------------------------------------------------------------------

    String ImageConverterApp::computeFileMD5(const String &filePath, bool &outOk)
    {
        outOk = false;
        FileDataStream fs;
        if (!fs.open(filePath.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            return String();
        }

        const size_t sz = fs.size();
        std::vector<uint8_t> bytes(sz);
        if (sz > 0)
        {
            if (fs.read(bytes.data(), sz) != sz)
            {
                fs.close();
                return String();
            }
        }
        fs.close();

        MD5 md5(bytes.data(), sz);
        outOk = true;
        return md5.toString();
    }

    //--------------------------------------------------------------------------

    bool ImageConverterApp::isSettingsEqual(const TextureImportSettings &a, const TextureImportSettings &b)
    {
        return a.TextureType == b.TextureType
            && a.PremultipliedAlpha == b.PremultipliedAlpha
            && a.GenerateMipmaps == b.GenerateMipmaps
            && a.FlipY == b.FlipY
            && a.MaxSize == b.MaxSize
            && a.sRGB == b.sRGB
            && a.Sampler.MinFilter == b.Sampler.MinFilter
            && a.Sampler.MagFilter == b.Sampler.MagFilter
            && a.Sampler.MipFilter == b.Sampler.MipFilter
            && a.Sampler.AddressU == b.Sampler.AddressU
            && a.Sampler.AddressV == b.Sampler.AddressV
            && a.Sampler.AddressW == b.Sampler.AddressW;
    }
}
