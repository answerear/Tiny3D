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

#ifndef __T3D_DEVICE_INFO_INTERFACE_H__
#define __T3D_DEVICE_INFO_INTERFACE_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class IDeviceInfo : public Allocator
    {
        T3D_DECLARE_INTERFACE(IDeviceInfo);

    public:
        /**
         * @brief 獲取平台類型
         */
        virtual uint32_t getPlatform() const = 0;

        /**
         * @brief 獲取軟件版本號字符串
         */
        virtual const String &getSoftwareVersion() const = 0;

        /**
         * @brief 設置軟件版本號字符串
         */
        virtual void setSoftwareVersion(const char *version) = 0;

        /**
         * @brief 獲取操作系統版本號字符串
         */
        virtual const String &getOSVersion() const = 0;

        /**
         * @brief 獲取設備機型版本信息字符串
         */
        virtual const String &getDeviceVersion() const = 0;

        /**
         * @brief 獲取屏幕寬度
         */
        virtual int32_t getScreenWidth() const = 0;

        /**
         * @brief 獲取屏幕高度
         */
        virtual int32_t getScreenHeight() const = 0;

        /**
         * @brief 獲取屏幕像素密度
         */
        virtual float getScreenDPI() const = 0;

        /**
         * @brief 獲取CPU類型信息
         */
        virtual const String &getCPUType() const = 0;

        /**
         * @brief 獲取CPU架構
         */
        virtual const String &getCPUArchitecture() const = 0;

        /**
         * @brief 獲取CPU核數
         */
        virtual int32_t getCPUCores() const = 0;

        /**
         * @brief 獲取系統內存總數
         */
        virtual uint64_t getSystemRAM() const = 0;

        /**
         * @brief 獲取設備ID
         */
        virtual const String &getDeviceID() const = 0;
    };
}


#endif  /*__T3D_DEVICE_INFO_INTERFACE_H__*/
