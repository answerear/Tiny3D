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

#ifndef __T3D_WIN32_MEMORY_MANAGER_H__
#define __T3D_WIN32_MEMORY_MANAGER_H__


#include "T3DMemoryManager.h"
#include <windows.h>

#include "../../../../Utils/Include/T3DVariant.h"

namespace Tiny3D
{
    class Win32MemManager : public MemoryManager
    {
    public:
        Win32MemManager();

        Win32MemManager(const Win32MemManager &) = delete;

        Win32MemManager(Win32MemManager &&) = delete;

        ~Win32MemManager() override;

        Win32MemManager &operator=(const Win32MemManager &) = delete;

        Win32MemManager &operator=(Win32MemManager &&) = delete;

        void *allocate(size_t size, uint32_t alignment, bool isArray) override;

        void deallocate(uint8_t *addr, uint32_t alignment, bool isArray) override;

        void exit() override;
    };

    class DebugMemManager : public MemoryManager
    {
    public:
        DebugMemManager();

        DebugMemManager(const DebugMemManager &) = delete;

        DebugMemManager(DebugMemManager &&) = delete;

        ~DebugMemManager() override;

        DebugMemManager &operator=(const DebugMemManager &) = delete;

        DebugMemManager &operator=(DebugMemManager &&) = delete;

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
        };
        
        struct Block
        {
            /// 申请空间大小
            size_t size {0};
            /// 申请内存时候的调用堆栈信息
            void *address[kMaxCallStacks] {nullptr};
            /// 申请内存时候的文件名
            char callstacks[kMaxCallStacks][MAX_PATH] {};
            /// 申请内存时候文件对应行数
            int32_t line[kMaxCallStacks] {0};
            /// 堆栈层数
            uint16_t callStackCount {0};
            /// 是否数组
            bool isArray {false};
            /// 是否字节对齐
            bool isAlignment {false};
            /// 前一节点
            Block *prev {nullptr};
            /// 后一节点
            Block *next {nullptr};
        };

        /// 插入内存块
        void insertBlock(Block *block);

        /// 删除内存块
        void removeBlock(Block *block);

        /// 获取源码文件信息和源码行数
        bool getFileAndLine(const void *address, char szFile[MAX_PATH], int32_t &line);

        /// 初始化加载 dbghelp.dll
        bool initDbgHelpLib();

        /// 释放 dbghelp.dll
        void freeDbgHelpLib();

        /// 释放泄漏的内存
        void freeLeakMem();

        /// 输出信息
        void printInfo();

        /// 调用 new 次数
        size_t mNewCallsCount {0};
        /// 调用 delete 次数
        size_t mDeleteCallsCount {0};
        /// 内存块链表头
        Block *mHead {nullptr};
        /// 内存块链表尾
        Block *mTail {nullptr};
        /// 内存块数量
        uint32_t mNumBlocks {0};
        /// 最多申请内存块
        uint32_t mMaxNumBlocks {0};
        /// 当前申请的字节数
        size_t mNumBytes {0};
        /// 最多申请字节数
        size_t mMaxNumBytes {0};
        /// 记录在 2 的 n 次方范围内的内存申请次数
        /// 也就是按照 1、2、4、8、16 ... 等划分范围，统计分配内存区块落在某个范围的数量
        uint32_t mSizeRecord[kRecordNum] {0};
    };
}


#endif  /*__T3D_WIN32_MEMORY_MANAGER_H__*/
