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


#include "Adapter/Android/T3DAndroidMemManager.h"
#include "T3DMacro.h"
#include <unwind.h>
#include <dlfcn.h>
#include <cstdlib>
#include <cstring>
#include <android/log.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    AndroidMemManager::AndroidMemManager()
    {
        
    }

    //--------------------------------------------------------------------------

    AndroidMemManager::~AndroidMemManager()
    {
        
    }

    //--------------------------------------------------------------------------

    void *AndroidMemManager::allocate(size_t size, uint32_t alignment, bool isArray)
    {
        return ::operator new(size);
    }

    //--------------------------------------------------------------------------

    void AndroidMemManager::deallocate(uint8_t *addr, uint32_t alignment, bool isArray)
    {
        ::operator delete(addr);
    }

    //--------------------------------------------------------------------------

    void AndroidMemManager::exit()
    {
        
    }

    //--------------------------------------------------------------------------
    // Debug version - with memory leak detection
    //--------------------------------------------------------------------------

    struct UnwindData
    {
        void **buffer;
        uint16_t maxFrames;
        uint16_t frameCount;
    };

    static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context *context, void *arg)
    {
        UnwindData *data = static_cast<UnwindData *>(arg);
        uintptr_t pc = _Unwind_GetIP(context);
        if (pc != 0)
        {
            if (data->frameCount < data->maxFrames)
            {
                data->buffer[data->frameCount] = reinterpret_cast<void *>(pc);
                data->frameCount++;
            }
            else
            {
                return _URC_END_OF_STACK;
            }
        }
        return _URC_NO_REASON;
    }

    static uint16_t captureBacktrace(void **buffer, uint16_t maxFrames)
    {
        UnwindData data;
        data.buffer = buffer;
        data.maxFrames = maxFrames;
        data.frameCount = 0;
        _Unwind_Backtrace(unwindCallback, &data);
        return data.frameCount;
    }

    //--------------------------------------------------------------------------

    AndroidDebugMemManager::AndroidDebugMemManager()
    {
        pthread_mutex_init(&mMutex, nullptr);
    }

    //--------------------------------------------------------------------------

    AndroidDebugMemManager::~AndroidDebugMemManager()
    {
        printInfo();
        freeLeakMem();
        pthread_mutex_destroy(&mMutex);
    }

    //--------------------------------------------------------------------------

    void AndroidDebugMemManager::exit()
    {
        // Resolve symbols at exit if needed (on Android, dladdr is used at print time)
    }

    //--------------------------------------------------------------------------

    void *AndroidDebugMemManager::allocate(size_t size, uint32_t alignment, bool isArray)
    {
        if (size == 0)
        {
            return nullptr;
        }

        uint32_t extendedSize = (uint32_t)(sizeof(Block) + sizeof(uint32_t) + size + sizeof(uint32_t));
        char *addr = (char *)malloc(extendedSize);
        if (addr == nullptr)
        {
            return nullptr;
        }

        Block *block = (Block *)addr;
        block->size = size;
        block->isArray = isArray;

        bool isAlignment = (alignment > 0);
        block->isAlignment = isAlignment;
        block->callStackCount = 0;

        // Capture call stack using _Unwind_Backtrace
        block->callStackCount = captureBacktrace(block->address, kMaxCallStacks);

        pthread_mutex_lock(&mMutex);
        insertBlock(block);

        addr += sizeof(Block);
        uint32_t *beginMask = (uint32_t *)addr;
        *beginMask = kBeginMask;
        addr += sizeof(uint32_t);

        uint32_t *endMask = (uint32_t *)(addr + size);
        *endMask = kEndMask;

        mNewCallsCount++;
        mNumBlocks++;
        mNumBytes += size;

        if (mNumBytes > mMaxNumBytes)
        {
            mMaxNumBytes = mNumBytes;
        }
        
        if (mNumBlocks > mMaxNumBlocks)
        {
            mMaxNumBlocks = mNumBlocks;
        }
        
        uint32_t uiTwoPowerI = 1;
        uint32_t i;
        for (i = 0; i <= kRecordNum - 2; i++, uiTwoPowerI <<= 1)
        {
            if (size <= uiTwoPowerI)
            {
                mSizeRecord[i]++;
                break;
            }
        }
        
        if (i == kRecordNum - 1)
        {
            mSizeRecord[i]++;
        }

        pthread_mutex_unlock(&mMutex);

        return addr;
    }
    
    //--------------------------------------------------------------------------

    void AndroidDebugMemManager::deallocate(uint8_t *addr, uint32_t alignment, bool isArray)
    {
        if (addr == nullptr)
            return;

        addr -= sizeof(uint32_t);

        uint32_t *beginMask = (uint32_t *)(addr);
        T3D_ASSERT(*beginMask == kBeginMask);

        addr -= sizeof(Block);

        Block *block = (Block *)addr;

        pthread_mutex_lock(&mMutex);
        removeBlock(block);

        T3D_ASSERT(block->isArray == isArray);
        T3D_ASSERT(mNumBlocks > 0 && mNumBytes >= block->size);
        bool isAlignment = (alignment > 0);
        T3D_ASSERT(block->isAlignment == isAlignment);
        uint32_t *endMask = (uint32_t *)(addr + sizeof(Block) + sizeof(uint32_t) + block->size);
        T3D_ASSERT(*endMask == kEndMask);

        mDeleteCallsCount++;
        mNumBlocks--;
        mNumBytes -= block->size;
        
        free(addr);
        
        pthread_mutex_unlock(&mMutex);
    }
    
    //--------------------------------------------------------------------------

    void AndroidDebugMemManager::insertBlock(Block *block)
    {
        if (mTail != nullptr)
        {
            block->prev = mTail;
            block->next = nullptr;
            mTail->next = block;
            mTail = block;
        }
        else
        {
            block->prev = nullptr;
            block->next = nullptr;
            mHead = block;
            mTail = block;
        }
    }

    //--------------------------------------------------------------------------

    void AndroidDebugMemManager::removeBlock(Block *block)
    {
        if (block->prev)
        {
            block->prev->next = block->next;
        }
        else
        {
            mHead = block->next;
        }

        if (block->next)
        {
            block->next->prev = block->prev;
        }
        else
        {
            mTail = block->prev;
        }
    }

    //--------------------------------------------------------------------------

    void AndroidDebugMemManager::freeLeakMem()
    {
        Block *block = mHead;
        while (block != nullptr)
        {
            Block *temp = block;
            block = block->next;
            free(temp);
        }
        mHead = nullptr;
        mTail = nullptr;
    }

    //--------------------------------------------------------------------------

    void AndroidDebugMemManager::printInfo()
    {
        __android_log_print(ANDROID_LOG_WARN, "Tiny3D",
            "#########################  begin print leak mem  ######################");

        __android_log_print(ANDROID_LOG_WARN, "Tiny3D", "Max Bytes: %zu", mMaxNumBytes);
        __android_log_print(ANDROID_LOG_WARN, "Tiny3D", "Max Blocks: %u", mMaxNumBlocks);
        __android_log_print(ANDROID_LOG_WARN, "Tiny3D", "Leak Bytes: %zu", mNumBytes);
        __android_log_print(ANDROID_LOG_WARN, "Tiny3D", "The Number of Leak Blocks: %u", mNumBlocks);
        __android_log_print(ANDROID_LOG_WARN, "Tiny3D", "The Number of New Calls: %zu", mNewCallsCount);
        __android_log_print(ANDROID_LOG_WARN, "Tiny3D", "The Number of Delete Calls: %zu", mDeleteCallsCount);

        __android_log_print(ANDROID_LOG_WARN, "Tiny3D", "Memory Distribution:");
        for (uint32_t i = 0; i < kRecordNum; i++)
        {
            __android_log_print(ANDROID_LOG_WARN, "Tiny3D",
                "\t The Number of Blocks [%u, %u): %u",
                i == 0 ? 0 : (1 << i), 1 << (i + 1), mSizeRecord[i]);
        }

        if (mHead)
        {
            __android_log_print(ANDROID_LOG_WARN, "Tiny3D",
                "\n------------------ Memory Leak: %u ------------------", mNumBlocks);
        }
        else
        {
            __android_log_print(ANDROID_LOG_WARN, "Tiny3D",
                "\n------------------ No Memory Leak ------------------");
        }
        
        Block *pBlock = mHead;
        unsigned int uiLeakNum = 0;
        while (pBlock)
        {
            uiLeakNum++;
            __android_log_print(ANDROID_LOG_ERROR, "Tiny3D",
                "$$$$$$$$$$$$$$$$  Begin Leak %u  $$$$$$$$$$$$$$$$$", uiLeakNum);
            __android_log_print(ANDROID_LOG_ERROR, "Tiny3D",
                "\tSize: %zu", pBlock->size);
            __android_log_print(ANDROID_LOG_ERROR, "Tiny3D",
                "\tIs Array: %d", pBlock->isArray);

            for (unsigned int i = 0; i < pBlock->callStackCount; i++)
            {
                Dl_info info;
                if (dladdr(pBlock->address[i], &info) && info.dli_sname)
                {
                    __android_log_print(ANDROID_LOG_ERROR, "Tiny3D",
                        "\t#%u: %p %s + %zu (%s)",
                        i, pBlock->address[i], info.dli_sname,
                        (size_t)((char *)pBlock->address[i] - (char *)info.dli_saddr),
                        info.dli_fname ? info.dli_fname : "unknown");
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, "Tiny3D",
                        "\t#%u: %p [unknown]", i, pBlock->address[i]);
                }
            }
            __android_log_print(ANDROID_LOG_ERROR, "Tiny3D",
                "$$$$$$$$$$$$$$$$$ End Leak %u  $$$$$$$$$$$$$$$$$$$", uiLeakNum);
            pBlock = pBlock->next;
        }

        __android_log_print(ANDROID_LOG_WARN, "Tiny3D",
            "#########################  end print leak mem  ######################");
    }

    //--------------------------------------------------------------------------
}
