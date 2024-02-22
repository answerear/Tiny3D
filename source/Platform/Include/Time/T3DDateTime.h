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

#ifndef __T3D_DATE_TIME_H__
#define __T3D_DATE_TIME_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    class ITime;

    /**
    * @class DateTime
    * @brief 各种时间相关操作的类.
    */
    class T3D_PLATFORM_API DateTime
    {
    public:
        enum class TimeFormat : uint32_t
        {
            HH_MM_SS_XXX = 0,        /// 转成字符串格式为：HH:MM:SS.XXX
            HH_MM_SS,
            HH_MM,
            HHMMSS,
        };

        enum class DateFormat : uint32_t
        {
            YY_MM_DD = 0,
            MM_DD,
            YY_MM,
            YYMMDD,
        };

    public:
        /**
        * @brief Default Constructor for QGDateTime.
        */
        DateTime();

        /**
        * @brief Constructor for QGDateTime.
        * @param [in] nYear : 年
        * @param [in] nMonth : 月
        * @param [in] nDay : 日
        * @param [in] nHour : 时
        * @param [in] nMin : 分
        * @param [in] nSecond : 秒
        * @param [in] millisecond : 毫秒
        */
        DateTime(int32_t year, int32_t month, int32_t day, 
            int32_t hour, int32_t minute, int32_t second, int32_t millisecond);

        /**
        * @brief 把QGDateTime中时间转换成从1970年1月1日开始的毫秒数.
        * @return 返回时间毫秒数.
        */
        int64_t toMSecsSinceEpoch() const;

        /**
        * @brief 把QGDataTime中时间转成字符串.
        * @note 字符串格式形如：YYYY-MM-DD HH:MM:SS.XXX
        * @return 返回String对象
        */
        String toString() const;

        /**
        * @brief 把QGDataTime中时间转成字符串.
        * @note 字符串格式形如：HH:MM:SS.XXX
        * @return 返回String对象
        */
        String timeToString(TimeFormat eFormat = TimeFormat::HH_MM_SS_XXX) const;

        /**
        * @brief 把QGDataTime中时间转成字符串.
        * @note 字符串格式形如：YYYY-MM-DD
        * @return 返回String对象
        */
        String dateToString(DateFormat eFormat = DateFormat::YY_MM_DD) const;

        /**
        * @brief 判断时间是否相等
        */
        bool operator ==(const DateTime &other) const;

        /**
        * @brief 判断时间是否相等
        */
        bool operator !=(const DateTime &other) const;

        /**
        * @brief 判断时间是否相等
        */
        bool operator <(const DateTime &other) const;

        /**
        * @brief 判断时间是否相等
        */
        bool operator <=(const DateTime &other) const;

        /**
        * @brief 判断时间是否相等
        */
        bool operator >(const DateTime &other) const;

        /**
        * @brief 判断时间是否相等
        */
        bool operator >=(const DateTime &other) const;

    public:
        /**
        * @brief 静态接口，获取当前时间的QGDateTime对象.
        * @return 返回当前时间的QGDateTime对象.
        */
        static DateTime currentDateTime();

        /**
        * @brief 静态接口，获取当前格林威治时间的QGDateTime对象.
        * @return 返回当前格林威治时间的QGDateTime对象.
        */
        static DateTime currentDateTimeUTC();

        /**
        * @brief 静态接口，获取从1970年1月1日到现在的时间秒数
        * @return 返回时间秒数
        */
        static int64_t currentSecsSinceEpoch();

        /**
        * @brief 静态接口，获取从1970年1月1日到现在的时间毫秒数.
        * @return 返回时间毫秒数.
        */
        static int64_t currentMSecsSinceEpoch();

        /**
        * @brief 静态接口，把从1970年1月1日开始的毫秒数转换成QGDateTime对象.
        * @param [in] msecs : 从1970年1月1日开始的毫秒数
        * @return 返回QGDateTime对象.
        */
        static DateTime fromMSecsSinceEpoch(int64_t msecs);

        /**
        * @brief 静态接口，把从1970年1月1日开始的秒数转换成QGDateTime对象.
        * @param [in] msecs : 从1970年1月1日开始的秒数
        * @return 返回QGDateTime对象.
        */
        static DateTime fromSecsSinceEpoch(int64_t sces);

    public:
        /** 返回年 */
        int32_t Year() const { return mYear; }
        /** 返回月 */
        int32_t Month() const { return mMonth; }
        /** 返回日 */
        int32_t Day() const { return mDay; }
        /** 返回时 */
        int32_t Hour() const { return mHour; }
        /** 返回分 */
        int32_t Minute() const { return mMinute; }
        /** 返回秒 */
        int32_t Second() const { return mSecond; }
        /** 返回毫秒 */
        int32_t Millisecond() const { return mMillisecond; }

    private:
        static ITime *getTime();

        int32_t mYear;
        int32_t mMonth;
        int32_t mDay;
        int32_t mHour;
        int32_t mMinute;
        int32_t mSecond;
        int32_t mMillisecond;

        static ITime    *mTime;
    };
}


#endif  /*__T3D_DATE_TIME_H__*/
