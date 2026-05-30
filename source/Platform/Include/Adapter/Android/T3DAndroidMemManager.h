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

#ifndef __T3D_ANDROID_MEMORY_MANAGER_H__
#define __T3D_ANDROID_MEMORY_MANAGER_H__


#include "T3DMemoryManager.h"
#include <pthread.h>

namespace Tiny3D
{
    class AndroidMemManager : public MemoryManager
    {
    public:
        AndroidMemManager();

        AndroidMemManager(const AndroidMemManager &) = delete;

        AndroidMemManager(AndroidMemManager &&) = delete;

        ~AndroidMemManager() override;

        AndroidMemManager &operator=(const AndroidMemManager &) = delete;

        AndroidMemManager &operator=(AndroidMemManager &&) = delete;

        void *allocate(size_t size, uint32_t alignment, bool isArray) override;

        void deallocate(uint8_t *addr, uint32_t alignment, bool isArray) override;

        void exit() override;
    };

    class AndroidDebugMemManager : public MemoryManager
    {
    public:
        AndroidDebugMemManager();

        AndroidDebugMemManager(const AndroidDebugMemManager &) = delete;

        AndroidDebugMemManager(AndroidDebugMemManager &&) = delete;

        ~AndroidDebugMemManager() override;

        AndroidDebugMemManager &operator=(const AndroidDebugMemManager &) = delete;

        AndroidDebugMemManager &operator=(AndroidDebugMemManager &&) = delete;

        void *allocate(size_t size, uint32_t alignment, bool isArray) override;

        void deallocate(uint8_t *addr, uint32_t alignment, bool isArray) override;

        void exit() override;
        
    private:
        enum : uint32_t
        {
            kBeginMask = 0xDEADC0DE,
            kEndMask = 0xDEADC0DE,
            kRecordNum = 32,
            kMaxCallStacks = 32,
            kMaxPathLen = 256,
        };
        
        struct Block
        {
            size_t size {0};
            void *address[kMaxCallStacks] {nullptr};
            uint16_t callStackCount {0};
            bool isArray {false};
            bool isAlignment {false};
            Block *prev {nullptr};
            Block *next {nullptr};
        };

        void insertBlock(Block *block);

        void removeBlock(Block *block);

        void freeLeakMem();

        void printInfo();

        size_t mNewCallsCount {0};
        size_t mDeleteCallsCount {0};
        Block *mHead {nullptr};
        Block *mTail {nullptr};
        uint32_t mNumBlocks {0};
        uint32_t mMaxNumBlocks {0};
        size_t mNumBytes {0};
        size_t mMaxNumBytes {0};
        uint32_t mSizeRecord[kRecordNum] {0};

        pthread_mutex_t mMutex;
    };
}


#endif  /*__T3D_ANDROID_MEMORY_MANAGER_H__*/
