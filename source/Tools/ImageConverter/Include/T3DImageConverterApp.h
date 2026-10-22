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

#ifndef __T3D_IMAGE_CONVERTER_APP_H__
#define __T3D_IMAGE_CONVERTER_APP_H__


#include "T3DImageConverterPrerequisites.h"
#include "T3DImageConverterOptions.h"
#include "T3DImportSummary.h"
#include <functional>


namespace Tiny3D
{
    /**
     * \brief ImageConverter 转换批处理回调接口（供 IPC 模式实时推送进度/日志/单张结果）
     */
    struct ImageBatchCallbacks
    {
        std::function<void(uint32_t current, uint32_t total, const String &stage, const String &currentFile)> onProgress;
        std::function<void(const String &level, const String &text)> onLog;
        std::function<void(const ImportItemRecord &record)> onItemResult;
    };

    /**
     * \brief ImageConverter 应用编排器
     */
    class ImageConverterApp : public ConsoleApplication
    {
    public:
        ImageConverterApp();
        ~ImageConverterApp() override;

        int32_t getExitCode() const { return mExitCode; }

        TResult applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

        void applicationDidEnterBackground() override;

        void applicationWillEnterForeground() override;

        void applicationWillTerminate() override;

        void applicationLowMemory() override;

        /**
         * \brief 执行一批转换任务（命令行模式与 IPC ReqImport 共用）
         * \param [in] options : 已解析的转换参数
         * \param [out] outRecords : 每张图片的转换结果
         * \param [out] outTotalElapsedMs : 批次总耗时（毫秒）
         * \param [in] callbacks : 可选回调（IPC 模式用于实时推送）
         * \return 全部成功返回 true，存在任意失败返回 false
         */
        bool processBatch(
            const ImageConverterOptions &options,
            std::vector<ImportItemRecord> &outRecords,
            uint64_t &outTotalElapsedMs,
            const ImageBatchCallbacks *callbacks = nullptr);

    private:
        ImportItemRecord processSingleImage(
            uint32_t index1Based,
            uint32_t totalCount,
            const String &srcPath,
            const ImageConverterOptions &options,
            const ImageBatchCallbacks *callbacks);

        static String resolveOutputTtexPath(
            const String &srcPath,
            const ImageConverterOptions &options);

        static String computeFileMD5(const String &filePath, bool &outOk);
        static bool isSettingsEqual(const TextureImportSettings &a, const TextureImportSettings &b);

    private:
        ImageImporterPtr    mImporter {nullptr};
        TextureExporterPtr  mExporter {nullptr};
        int32_t             mExitCode {0};
    };
}


#endif  /*__T3D_IMAGE_CONVERTER_APP_H__*/
