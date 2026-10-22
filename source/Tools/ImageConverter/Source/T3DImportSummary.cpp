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


#include "T3DImportSummary.h"
#include <cstdio>


namespace Tiny3D
{
    namespace
    {
        String extractFileName(const String &path)
        {
            String dir;
            String name;
            StringUtil::split(path, dir, name);
            return name.empty() ? path : name;
        }
    }

    //--------------------------------------------------------------------------

    void ImportSummary::printItemLine(
        size_t index1Based,
        size_t totalCount,
        const ImportItemRecord &rec,
        bool verbose)
    {
        const String srcDisplay = verbose ? rec.srcPath : extractFileName(rec.srcPath);
        const String dstDisplay = verbose ? rec.dstPath : extractFileName(rec.dstPath);

        if (!rec.success)
        {
            fprintf(stderr, "[%zu/%zu] FAIL %-20s (%s)\n",
                index1Based,
                totalCount,
                srcDisplay.c_str(),
                rec.errorMessage.c_str());
        }
        else if (rec.skipped)
        {
            printf("[%zu/%zu] SKIP %-20s -> %-20s (unchanged, uuid=%s)\n",
                index1Based,
                totalCount,
                srcDisplay.c_str(),
                dstDisplay.c_str(),
                rec.uuid.c_str());
        }
        else
        {
            printf("[%zu/%zu] OK   %-20s -> %-20s (%ux%u, mips=%u, pma=%s, uuid=%s) [%llums]\n",
                index1Based,
                totalCount,
                srcDisplay.c_str(),
                dstDisplay.c_str(),
                rec.width,
                rec.height,
                rec.mipLevels,
                rec.premultipliedAlpha ? "true" : "false",
                rec.uuid.c_str(),
                static_cast<unsigned long long>(rec.elapsedMs));
        }
    }

    //--------------------------------------------------------------------------

    void ImportSummary::printBatchSummary(
        const std::vector<ImportItemRecord> &records,
        uint64_t totalElapsedMs)
    {
        uint32_t okCount = 0;
        uint32_t skipCount = 0;
        uint32_t failCount = 0;

        for (const auto &r : records)
        {
            if (!r.success)
            {
                ++failCount;
            }
            else if (r.skipped)
            {
                ++skipCount;
            }
            else
            {
                ++okCount;
            }
        }

        printf("Summary: total=%zu, ok=%u, skipped=%u, failed=%u, time=%llums\n",
            records.size(),
            okCount,
            skipCount,
            failCount,
            static_cast<unsigned long long>(totalElapsedMs));
    }
}
