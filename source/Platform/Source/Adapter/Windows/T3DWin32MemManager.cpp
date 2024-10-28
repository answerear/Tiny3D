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


#include "Adapter/Windows/T3DWin32MemManager.h"
#include "T3DMacro.h"
#include <windows.h>
#include <DbgHelp.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Win32MemManager::Win32MemManager()
    {
        
    }

    //--------------------------------------------------------------------------

    Win32MemManager::~Win32MemManager()
    {
        
    }

    //--------------------------------------------------------------------------

    void *Win32MemManager::allocate(size_t size, uint32_t alignment, bool isArray)
    {
        return ::operator new(size);
    }

    //--------------------------------------------------------------------------

    void Win32MemManager::deallocate(uint8_t *addr, uint32_t alignment, bool isArray)
    {
        ::operator delete(addr);
    }

    //--------------------------------------------------------------------------

    void Win32MemManager::exit()
    {
        
    }

    //--------------------------------------------------------------------------

    typedef BOOL
    (WINAPI
     *tFMiniDumpWriteDump)
     (
     IN HANDLE hProcess,
     IN DWORD ProcessId,
     IN HANDLE hFile,
     IN MINIDUMP_TYPE DumpType,
     IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, OPTIONAL
     IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, OPTIONAL
     IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL
     );

    typedef BOOL
    (WINAPI
     *tFSymInitialize)
     (
     IN HANDLE   hProcess,
     IN PSTR     UserSearchPath,
     IN BOOL     fInvadeProcess
     );

    typedef BOOL
    (WINAPI
     *tFStackWalk64)
     (
     DWORD                             MachineType,
     HANDLE                            hProcess,
     HANDLE                            hThread,
     LPSTACKFRAME64                    StackFrame,
     PVOID                             ContextRecord,
     PREAD_PROCESS_MEMORY_ROUTINE64    ReadMemoryRoutine,
     PFUNCTION_TABLE_ACCESS_ROUTINE64  FunctionTableAccessRoutine,
     PGET_MODULE_BASE_ROUTINE64        GetModuleBaseRoutine,
     PTRANSLATE_ADDRESS_ROUTINE64      TranslateAddress
     );

    typedef BOOL
    (WINAPI
     *tFSymFromAddr)
     (
     IN  HANDLE              hProcess,
     IN  DWORD64             Address,
     OUT PDWORD64            Displacement,
     IN OUT PSYMBOL_INFO     Symbol
     );

    typedef BOOL
    (WINAPI
     *tFSymGetLineFromAddr64)
     (
     IN  HANDLE                  hProcess,
     IN  DWORD64                 qwAddr,
     OUT PDWORD                  pdwDisplacement,
     OUT PIMAGEHLP_LINE64        Line64
     );

    typedef DWORD
    (WINAPI
     *tFSymGetOptions)
     (
     VOID
     );

    typedef DWORD
    (WINAPI
     *tFSymSetOptions)
     (
     IN DWORD   SymOptions
     );

    typedef PVOID
    (WINAPI
     *tFSymFunctionTableAccess64)
     (
     HANDLE  hProcess,
     DWORD64 AddrBase
     );

    typedef DWORD64
    (WINAPI
     *tFSymGetModuleBase64)
     (
     IN  HANDLE              hProcess,
     IN  DWORD64             qwAddr
     );
    
    //--------------------------------------------------------------------------

    static tFMiniDumpWriteDump fnMiniDumpWriteDump = nullptr;
    static tFSymInitialize fnSymInitialize = nullptr;
    static tFStackWalk64 fnStackWalk64 = nullptr;
    static tFSymFromAddr fnSymFromAddr = nullptr;
    static tFSymGetLineFromAddr64 fnSymGetLineFromAddr64 = nullptr;
    static tFSymGetOptions fnSymGetOptions = nullptr;
    static tFSymSetOptions fnSymSetOptions = nullptr;
    static tFSymFunctionTableAccess64 fnSymFunctionTableAccess64 = nullptr;
    static tFSymGetModuleBase64 fnSymGetModuleBase64 = nullptr;

    static HMODULE gsDbgHelpLib = nullptr;
    static HANDLE gsProcess = nullptr;

    static CRITICAL_SECTION gsLock;
    
    //--------------------------------------------------------------------------

    DebugMemManager::DebugMemManager()
    {
        initDbgHelpLib();
        ::InitializeCriticalSection(&gsLock);
    }

    //--------------------------------------------------------------------------

    DebugMemManager::~DebugMemManager()
    {
        // T3D_ASSERT(mNewCallsCount == mDeleteCallsCount);
        printInfo();
        freeDbgHelpLib();
        freeLeakMem();
        ::DeleteCriticalSection(&gsLock);
    }

    //--------------------------------------------------------------------------

    void DebugMemManager::exit()
    {
        Block * pBlock = mHead;
        static unsigned int uiLeakNum = 0;
        while (pBlock)
        {
            uiLeakNum++;
            // TCHAR szFile[MAX_PATH];
            // int	  line;
            for (unsigned int i = 0 ; i < pBlock->callStackCount ; i++)
            {
                if (!getFileAndLine(pBlock->address[i], pBlock->callstacks[i], pBlock->line[i]))
                {
                    break;
                }
            }
            pBlock = pBlock->next;
        }
    }

    //--------------------------------------------------------------------------

    void *DebugMemManager::allocate(size_t size, uint32_t alignment, bool isArray)
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
        
        uint16_t frames = ::RtlCaptureStackBackTrace(0, kMaxCallStacks, block->address, nullptr);
        block->callStackCount = frames;

        ::EnterCriticalSection(&gsLock);
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
        for (i = 0; i <= kRecordNum - 2 ; i++, uiTwoPowerI <<= 1)
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

        ::LeaveCriticalSection(&gsLock);

        return addr;
    }
    
    //--------------------------------------------------------------------------

    void DebugMemManager::deallocate(uint8_t *addr, uint32_t alignment, bool isArray)
    {
        if (addr == nullptr)
            return;

        addr -= sizeof(uint32_t);

        uint32_t *beginMask = (uint32_t *)(addr);
        T3D_ASSERT(*beginMask == kBeginMask);

        addr -= sizeof(Block);

        Block *block = (Block *)addr;

        ::EnterCriticalSection(&gsLock);
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
        
        ::LeaveCriticalSection(&gsLock);
    }
    
    //--------------------------------------------------------------------------

    void DebugMemManager::insertBlock(Block *block)
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

    void DebugMemManager::removeBlock(Block *block)
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

    bool DebugMemManager::getFileAndLine(const void *address, char szFile[260], int32_t &line)
    {
        IMAGEHLP_LINE64 Line;
        Line.SizeOfStruct = sizeof(Line);
        memset(&Line, 0, sizeof(Line));
        DWORD Offset = 0;

        if (fnSymGetLineFromAddr64(gsProcess, (DWORD64)address, &Offset, &Line))
        {
            strcpy_s(szFile, MAX_PATH, Line.FileName);
            line = Line.LineNumber;

            return true;
        }
        
        DWORD error = GetLastError();
        return false;
    }
    
    //--------------------------------------------------------------------------

    bool DebugMemManager::initDbgHelpLib()
    {
        char dbgName[MAX_PATH];
        ::GetModuleFileName(nullptr, dbgName, MAX_PATH);
        char *p = csrchr(dbgName, '\\');
        if (p != nullptr)
            *p = 0;
        strcat_s(dbgName, MAX_PATH, "\\dbghelp.dll");

        // 查找当前目录 DLL
        gsDbgHelpLib = LoadLibrary(dbgName);
        if (gsDbgHelpLib == nullptr)
        {
            // 使用系统 DLL
            gsDbgHelpLib = LoadLibrary("dbghelp.dll");
            if (gsDbgHelpLib == nullptr)
                return false;
        }

        fnMiniDumpWriteDump = (tFMiniDumpWriteDump)GetProcAddress(gsDbgHelpLib, "MiniDumpWriteDump");
        fnSymInitialize = (tFSymInitialize)GetProcAddress(gsDbgHelpLib, "SymInitialize");
        fnStackWalk64 = (tFStackWalk64)GetProcAddress(gsDbgHelpLib, "StackWalk64");
        fnSymFromAddr = (tFSymFromAddr)GetProcAddress(gsDbgHelpLib, "SymFromAddr");
        fnSymGetLineFromAddr64 = (tFSymGetLineFromAddr64)GetProcAddress(gsDbgHelpLib, "SymGetLineFromAddr64");
        fnSymGetOptions = (tFSymGetOptions)GetProcAddress(gsDbgHelpLib, "SymGetOptions");
        fnSymSetOptions = (tFSymSetOptions)GetProcAddress(gsDbgHelpLib, "SymSetOptions");
        fnSymFunctionTableAccess64 = (tFSymFunctionTableAccess64)GetProcAddress(gsDbgHelpLib, "SymFunctionTableAccess64");
        fnSymGetModuleBase64 = (tFSymGetModuleBase64)GetProcAddress(gsDbgHelpLib, "SymGetModuleBase64");
        
        if (fnMiniDumpWriteDump &&
            fnSymInitialize &&
            fnStackWalk64 &&
            fnSymFromAddr &&
            fnSymGetLineFromAddr64 &&
            fnSymGetOptions &&
            fnSymSetOptions &&
            fnSymFunctionTableAccess64 &&
            fnSymGetModuleBase64)
        {
            DWORD ProcessID = GetCurrentProcessId();
            gsProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
            fnSymInitialize(gsProcess, ".", TRUE);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    void DebugMemManager::freeDbgHelpLib()
    {
        if(gsDbgHelpLib != nullptr)
        {
            FreeLibrary(gsDbgHelpLib);
            CloseHandle(gsProcess);
            gsProcess = nullptr;
            gsDbgHelpLib = nullptr;
        }

        fnMiniDumpWriteDump = nullptr;
        fnSymInitialize = nullptr;
        fnStackWalk64 = nullptr;
        fnSymFromAddr = nullptr;
        fnSymGetLineFromAddr64 = nullptr;
        fnSymGetOptions = nullptr;
        fnSymSetOptions = nullptr;
        fnSymFunctionTableAccess64 = nullptr;
        fnSymGetModuleBase64 = nullptr;
    }

    //--------------------------------------------------------------------------

    void DebugMemManager::freeLeakMem()
    {
        Block *block = mHead;
        while (block != nullptr)
        {
            Block *temp = block;
            block = block->next;
            free(temp);
        }
    }

    //--------------------------------------------------------------------------

    static const uint32_t BUFFER_SIZE = 65536;
    static char gsBuffer[BUFFER_SIZE];
    
    void Win32OutputDebugString(const char *str, ...)
    {
        char *args = (char *)&str + sizeof(str);
        vsprintf_s(gsBuffer, BUFFER_SIZE, str, args);
        OutputDebugString(gsBuffer);
    }
    
    //--------------------------------------------------------------------------

    void DebugMemManager::printInfo()
    {
        Win32OutputDebugString("#########################  begin print leak mem  ######################\n");

        Win32OutputDebugString("Max Bytes: %lu\n", mMaxNumBytes);

        Win32OutputDebugString("Max Blocks: %u\n", mMaxNumBlocks);

        Win32OutputDebugString("Total Size: %lu\n", mNumBytes);

        Win32OutputDebugString("The Number of Block : %u\n", mNumBlocks);
        
        Win32OutputDebugString("The Number of New Calls: %lu\n", mNewCallsCount);

        Win32OutputDebugString("The Number of Delete Calls: %lu\n", mDeleteCallsCount);

        if (mHead)
        {
            Win32OutputDebugString("Memory Leak: %u\n", mNumBlocks);
        }
        else
        {
            Win32OutputDebugString("No Memory Leak\n");
        }
        
        Block * pBlock = mHead;
        static unsigned int uiLeakNum = 0;
        while (pBlock)
        {

            uiLeakNum++;
            Win32OutputDebugString("$$$$$$$$$$$$$$$$  Leak %u  $$$$$$$$$$$$$$$$$\n",uiLeakNum);
            Win32OutputDebugString("Size: %lu\n", pBlock->size);
            Win32OutputDebugString("Is Array:%d\n", pBlock->isArray);

            // TCHAR szFile[MAX_PATH];
            // int	  line;
            for (unsigned int i = 0 ; i < pBlock->callStackCount ; i++)
            {

                // if (!getFileAndLine(pBlock->address[i], szFile, line))
                // {
                //     break;
                // }
                Win32OutputDebugString("%s(%d)\n", pBlock->callstacks[i], pBlock->line[i]);
		
            }
            Win32OutputDebugString("$$$$$$$$$$$$$$$$$ Leak %d  $$$$$$$$$$$$$$$$$$$\n", uiLeakNum);
            pBlock = pBlock->next;
        }
        
        Win32OutputDebugString("leak block total num : %d\n", uiLeakNum);

        Win32OutputDebugString("#########################  end print leak mem  ######################\n");
    }

    //--------------------------------------------------------------------------
}
