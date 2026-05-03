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


#include "Performance/T3DPerformance.h"
#include "T3DLog.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    #define LOG_TAG_PERFORMANCE     "Performance"

    //--------------------------------------------------------------------------

    TMap<String, Performance::SampleData> Performance::mSamples;

    //--------------------------------------------------------------------------

    void Performance::beginSample(const char *name)
    {
        auto &data = mSamples[name];
        data.startTime = DateTime::currentMicroSeconds();
    }

    //--------------------------------------------------------------------------

    void Performance::endSample(const char *name)
    {
        int64_t endTime = DateTime::currentMicroSeconds();

        auto it = mSamples.find(name);
        if (it != mSamples.end())
        {
            SampleData &data = it->second;
            data.lastDuration = endTime - data.startTime;
            data.totalDuration += data.lastDuration;
            data.count++;
        }
    }

    //--------------------------------------------------------------------------

    int64_t Performance::getLastSampleMicroseconds(const char *name)
    {
        auto it = mSamples.find(name);
        if (it != mSamples.end())
        {
            return it->second.lastDuration;
        }
        return 0;
    }

    //--------------------------------------------------------------------------

    int64_t Performance::getTotalSampleMicroseconds(const char *name)
    {
        auto it = mSamples.find(name);
        if (it != mSamples.end())
        {
            return it->second.totalDuration;
        }
        return 0;
    }

    //--------------------------------------------------------------------------

    uint32_t Performance::getSampleCount(const char *name)
    {
        auto it = mSamples.find(name);
        if (it != mSamples.end())
        {
            return it->second.count;
        }
        return 0;
    }

    //--------------------------------------------------------------------------

    void Performance::logAllSamples()
    {
        T3D_LOG_INFO(LOG_TAG_PERFORMANCE,
            "================ Performance Samples ================");

        for (const auto &pair : mSamples)
        {
            const String &name = pair.first;
            const SampleData &data = pair.second;

            int64_t avgUs = (data.count > 0) ? (data.totalDuration / data.count) : 0;

            T3D_LOG_INFO(LOG_TAG_PERFORMANCE,
                "[%s] count=%u, last=%lld us, total=%lld us, avg=%lld us",
                name.c_str(),
                data.count,
                (long long)data.lastDuration,
                (long long)data.totalDuration,
                (long long)avgUs);
        }

        T3D_LOG_INFO(LOG_TAG_PERFORMANCE,
            "=====================================================");
    }

    //--------------------------------------------------------------------------

    void Performance::reset()
    {
        mSamples.clear();
    }
}
