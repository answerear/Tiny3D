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


#include "Adapter/Windows/T3DWin32Time.h"
#include <windows.h>
#include <time.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const uint64_t Win32Time::EPOCH = ((uint64_t)116444736000000000ULL);

    //--------------------------------------------------------------------------

    Win32Time::Win32Time()
    {

    }

    //--------------------------------------------------------------------------

    Win32Time::~Win32Time()
    {

    }

    //--------------------------------------------------------------------------

    int64_t Win32Time::currentSecsSinceEpoch()
    {
//         LARGE_INTEGER liTime, liFreq;
//         ::QueryPerformanceFrequency(&liFreq);
//         ::QueryPerformanceCounter(&liTime);
//         return (int64_t)(liTime.QuadPart / liFreq.QuadPart);
        FILETIME    file_time;
        SYSTEMTIME  system_time;
        ULARGE_INTEGER ularge;

        GetSystemTime(&system_time);
        SystemTimeToFileTime(&system_time, &file_time);
        ularge.LowPart = file_time.dwLowDateTime;
        ularge.HighPart = file_time.dwHighDateTime;

        return (int64_t)((ularge.QuadPart - EPOCH) / 10000000L);
    }

    //--------------------------------------------------------------------------

    int64_t Win32Time::currentMSecsSinceEpoch()
    {
//         LARGE_INTEGER liTime, liFreq;
//         ::QueryPerformanceFrequency(&liFreq);
//         ::QueryPerformanceCounter(&liTime);
//         int64_t sec = (int64_t)(liTime.QuadPart / liFreq.QuadPart);
//         int64_t sec = (int64_t)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart);
//         int64_t msec = (int64_t)(liTime.QuadPart * 1000.0 / liFreq.QuadPart 
//             - sec * 1000.0);
//         return (sec * 1000 + msec);
//         return liTime.QuadPart / 1000;
        FILETIME    file_time;
        SYSTEMTIME  system_time;
        ULARGE_INTEGER ularge;

        GetSystemTime(&system_time);
        SystemTimeToFileTime(&system_time, &file_time);
        ularge.LowPart = file_time.dwLowDateTime;
        ularge.HighPart = file_time.dwHighDateTime;

        int64_t sec = (int64_t)((ularge.QuadPart - EPOCH) / 10000000L);
        return sec * 1000 + system_time.wMilliseconds;
    }
}
