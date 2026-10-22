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

#ifndef __T3D_IMPORT_SUMMARY_H__
#define __T3D_IMPORT_SUMMARY_H__


#include "T3DImageConverterPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief 单张图片导入结果记录（供命令行汇总输出与 IPC MsgResult 共用）
     */
    struct ImportItemRecord : public Allocator
    {
        String      srcPath {};
        String      dstPath {};
        String      uuid {};
        uint32_t    width {0};
        uint32_t    height {0};
        uint32_t    mipLevels {0};
        bool        premultipliedAlpha {false};
        bool        success {false};
        bool        skipped {false};
        String      errorMessage {};
        uint64_t    elapsedMs {0};
    };

    /**
     * \brief 命令行模式进度与汇总输出器（遵循 §6.1 输出格式规范）
     */
    class ImportSummary : public Allocator
    {
    public:
        static void printItemLine(
            size_t index1Based,
            size_t totalCount,
            const ImportItemRecord &rec,
            bool verbose);

        static void printBatchSummary(
            const std::vector<ImportItemRecord> &records,
            uint64_t totalElapsedMs);
    };
}


#endif  /*__T3D_IMPORT_SUMMARY_H__*/
