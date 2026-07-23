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

#include "IO/T3DFileDataStream.h"

#include "T3DLocale.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    FileDataStream::FileDataStream()
        : m_pFileHandle(nullptr)
        , m_lSize(0)
        , m_bIsOpened(false)
        , m_pData(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    FileDataStream::~FileDataStream()
    {
        T3D_POD_SAFE_DELETE_ARRAY(m_pData);

        if (isOpened())
        {
            close();
        }
    }

    //--------------------------------------------------------------------------

    bool FileDataStream::open(const char *szFileName, uint32_t unMode)
    {
        String filename = T3D_LOCALE.UTF8ToANSI(szFileName);

        // ===================== 旧版本实现（保留备查） =====================
        // 问题：
        //   1) 文本分支把只读/只写误写成读写：READ_ONLY|TEXT -> "r+t"（应为
        //      "rt"）、WRITE_ONLY|TEXT -> "w+t"（应为 "wt"），其中 "r+t" 要求
        //      写权限，打开只读文件会直接失败。
        //   2) 没有 else 兜底，且进入前不重置 m_pFileHandle，遇到未覆盖的模式
        //      组合会残留上一次的旧句柄，导致逻辑错乱与句柄泄漏。
        //   3) 组合覆盖不全（大量精确 == 判断，脆弱且不闭合）。
        //   4) "t" 是 MSVC 扩展，跨平台（glibc/bionic）上属未定义行为。
        //
        // if (unMode == E_MODE_READ_ONLY)
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "rb");
        // }
        // else if (unMode == E_MODE_WRITE_ONLY)
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "wb");
        // }
        // else if (unMode == (E_MODE_WRITE_ONLY|E_MODE_APPEND)
        //          || unMode == E_MODE_APPEND)
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "ab");
        // }
        // else if (unMode == (E_MODE_WRITE_ONLY|E_MODE_TRUNCATE)
        //     || unMode == E_MODE_TRUNCATE)
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "wb");
        // }
        // else if (unMode == E_MODE_READ_WRITE)
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "r+b");
        // }
        // else if (unMode == (E_MODE_READ_WRITE|E_MODE_TRUNCATE))
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "w+b");
        // }
        // else if (unMode == (E_MODE_READ_WRITE|E_MODE_APPEND))
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "a+b");
        // }
        // else if (unMode == (E_MODE_READ_ONLY|E_MODE_TEXT))
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "r+t");
        // }
        // else if (unMode == (E_MODE_WRITE_ONLY|E_MODE_TEXT))
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "w+t");
        // }
        // else if (unMode == (E_MODE_WRITE_ONLY|E_MODE_APPEND|E_MODE_TEXT)
        //          || unMode == (E_MODE_APPEND|E_MODE_TEXT))
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "a+t");
        // }
        // else if (unMode == (E_MODE_READ_WRITE|E_MODE_TEXT))
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "r+t");
        // }
        // else if (unMode == (E_MODE_READ_WRITE|E_MODE_TRUNCATE|E_MODE_TEXT))
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "w+t");
        // }
        // else if (unMode == (E_MODE_READ_WRITE|E_MODE_APPEND|E_MODE_TEXT))
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "a+t");
        // }
        // else if (unMode == (E_MODE_WRITE_ONLY|E_MODE_TRUNCATE|E_MODE_TEXT))
        // {
        //     m_pFileHandle = fopen(filename.c_str(), "wt");
        // }
        //
        // m_lSize = 0;
        //
        // return (m_bIsOpened = (m_pFileHandle != nullptr));
        // ===================================================================

        // 复位状态：避免在已打开或重复 open 的情况下残留旧句柄/旧缓存。
        if (m_pFileHandle != nullptr)
        {
            fclose(m_pFileHandle);
            m_pFileHandle = nullptr;
        }
        T3D_POD_SAFE_DELETE_ARRAY(m_pData);
        m_lSize = 0;
        m_bIsOpened = false;

        const bool bRead   = (unMode & E_MODE_READ_ONLY) != 0;
        const bool bAppend = (unMode & E_MODE_APPEND) != 0;
        const bool bTrunc  = (unMode & E_MODE_TRUNCATE) != 0;
        const bool bText   = (unMode & E_MODE_TEXT) != 0;
        // WRITE_ONLY / TRUNCATE / APPEND 任意一个都意味着需要写：
        //   - TRUNCATE 表示清空文件内容，本质是写；
        //   - APPEND   表示在文件末尾追加，本质也是写。
        const bool bWrite  = (unMode & E_MODE_WRITE_ONLY) != 0 || bTrunc || bAppend;

        // 按位拼装标准 fopen 模式串。基础字符 + 可选 '+'（可读写）+ 可选 'b'。
        // 文本模式不追加 'b'（文本是 C 标准默认模式，跨平台安全；不使用 MSVC
        // 专有的 't'）。
        char szFopenMode[8] = { 0 };
        size_t nIndex = 0;

        if (bAppend)
        {
            // 追加：'a' 隐含写（且文件不存在时创建）；若同时要求读则用 'a+'。
            szFopenMode[nIndex++] = 'a';
            if (bRead)
            {
                szFopenMode[nIndex++] = '+';
            }
        }
        else if (bRead && bWrite)
        {
            // 读写：截断用 "w+"（创建/清空），否则用 "r+"（要求文件已存在）。
            szFopenMode[nIndex++] = bTrunc ? 'w' : 'r';
            szFopenMode[nIndex++] = '+';
        }
        else if (bWrite)
        {
            // 只写（含只传 WRITE_ONLY 或只传 TRUNCATE 的情况）：
            // 'w' 隐含创建并截断。
            szFopenMode[nIndex++] = 'w';
        }
        else if (bRead)
        {
            // 只读。
            szFopenMode[nIndex++] = 'r';
        }
        else
        {
            // 非法/未指定的打开模式，直接失败。
            T3D_ASSERT(false);
            return false;
        }

        if (!bText)
        {
            szFopenMode[nIndex++] = 'b';
        }
        szFopenMode[nIndex] = '\0';

        m_pFileHandle = fopen(filename.c_str(), szFopenMode);

        return (m_bIsOpened = (m_pFileHandle != nullptr));
    }

    void FileDataStream::close()
    {
        if (m_pFileHandle != nullptr)
        {
            fflush(m_pFileHandle);
            fclose(m_pFileHandle);
            m_pFileHandle = nullptr;
        }

        m_lSize = 0;
        m_bIsOpened = false;
    }

    //--------------------------------------------------------------------------

    size_t FileDataStream::read(void *pBuffer, size_t nSize)
    {
        size_t nBytesOfRead = 0;

        if (m_pFileHandle != nullptr)
        {
            nBytesOfRead = fread(pBuffer, 1, nSize, m_pFileHandle);
        }

        return nBytesOfRead;
    }

    //--------------------------------------------------------------------------

    size_t FileDataStream::write(void *pBuffer, size_t nSize)
    {
        size_t nBytesOfWritten = 0;

        if (m_pFileHandle != nullptr)
        {
            nBytesOfWritten = fwrite(pBuffer, 1, nSize, m_pFileHandle);
        }

        return nBytesOfWritten;
    }

    //--------------------------------------------------------------------------

    void FileDataStream::flush()
    {
        if (m_pFileHandle != nullptr)
        {
            fflush(m_pFileHandle);
        }
    }

    //--------------------------------------------------------------------------

    bool FileDataStream::seek(long_t lPos, bool relative)
    {
        bool ret = false;

        if (m_pFileHandle != nullptr)
        {
            if (relative)
            {
                if (fseek(m_pFileHandle, (long)lPos, SEEK_CUR) == 0)
                {
                    ret = true;
                }
            }
            else
            {
                if (fseek(m_pFileHandle, (long)lPos, SEEK_SET) == 0)
                {
                    ret = true;
                }
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    long_t FileDataStream::tell() const
    {
        long_t lPos = 0;

        if (m_pFileHandle != nullptr)
        {
            lPos = ftell(m_pFileHandle);
        }

        return lPos;
    }

    //--------------------------------------------------------------------------

    long_t FileDataStream::size() const
    {
        if (m_pFileHandle != nullptr)
        {
            if (m_lSize == 0)
            {
                long_t lPos = ftell(m_pFileHandle);
                fseek(m_pFileHandle, 0, SEEK_END);
                m_lSize = ftell(m_pFileHandle);
                fseek(m_pFileHandle, (long)lPos, SEEK_SET);
            }
        }

        return m_lSize;
    }

    //--------------------------------------------------------------------------

    bool FileDataStream::eof() const
    {
        bool bEnd = true;

        if (m_pFileHandle != nullptr)
        {
            long unPos = ftell(m_pFileHandle);
            long unSize = (long)size();
            bEnd = (unPos == unSize);
        }

        return bEnd;
    }

    //--------------------------------------------------------------------------

    size_t FileDataStream::read(uint8_t *&pData)
    {
        size_t bytesOfRead = 0;

        if (m_pFileHandle != nullptr)
        {
            if (m_pData == nullptr)
            {
                long_t s = size();
                m_pData = T3D_POD_NEW_ARRAY(uint8_t, s);
                uint32_t pos = ftell(m_pFileHandle);
                fseek(m_pFileHandle, 0, SEEK_SET);
                bytesOfRead = fread(m_pData, 1, s, m_pFileHandle);
                fseek(m_pFileHandle, pos, SEEK_SET);
            }
            else
            {
                bytesOfRead = m_lSize;
            }
            
            pData = m_pData;
        }

        return bytesOfRead;
    }
}
