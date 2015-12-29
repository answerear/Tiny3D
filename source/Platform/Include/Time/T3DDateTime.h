

#ifndef __T3D_DATE_TIME_H__
#define __T3D_DATE_TIME_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    /**
     * @class T3DDateTime
     * @brief 各种时间相关操作的类.
     */
    class T3D_PLATFORM_API DateTime
    {
    public:
        enum ETimeFormat
        {
            E_HH_MM_SS_XXX = 0,     /// 转成字符串格式为：HH:MM:SS.XXX
            E_HH_MM_SS,
            E_HH_MM,
        };

        enum EDateFormat
        {
            E_YY_MM_DD = 0,
            E_MM_DD,
            E_YY_MM,
        };

    public:
        /**
         * @brief Default Constructor for T3DDateTime.
         */
        DateTime();

        /**
         * @brief Constructor for T3DDateTime.
         * @param [in] nYear : 年
         * @param [in] nMonth : 月
         * @param [in] nDay : 日
         * @param [in] nHour : 时
         * @param [in] nMin : 分
         * @param [in] nSecond : 秒
         * @param [in] millisecond : 毫秒
         */
        DateTime(int32_t nYear, int32_t nMonth, int32_t nDay, int32_t nHour, int32_t nMin, int32_t nSecond, int32_t millisecond);

        /**
         * @brief 把T3DDateTime中时间转换成从1970年1月1日开始的毫秒数.
         * @return 返回时间毫秒数.
         */
        uint64_t toMSecsSinceEpoch() const;

        /**
         * @brief 把VDataTime中时间转成字符串.
         * @note 字符串格式形如：YYYY-MM-DD HH:MM:SS.XXX
         * @return 返回std::string对象
         */
        String toString() const;

        /**
         * @brief 把VDataTime中时间转成字符串.
         * @note 字符串格式形如：HH:MM:SS.XXX
         * @return 返回std::string对象
         */
        String timeToString(ETimeFormat eFormat = E_HH_MM_SS_XXX) const;

        /**
         * @brief 把VDataTime中时间转成字符串.
         * @note 字符串格式形如：YYYY-MM-DD
         * @return 返回std::string对象
         */
        String dateToString(EDateFormat eFormat = E_YY_MM_DD) const;

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
         * @brief 静态接口，获取当前时间的T3DDateTime对象.
         * @return 返回当前时间的T3DDateTime对象.
         */
        static DateTime currentDateTime();

        /**
         * @brief 静态接口，获取当前格林威治时间的T3DDateTime对象.
         * @return 返回当前格林威治时间的T3DDateTime对象.
         */
        static DateTime currentDateTimeUTC();

        /**
         * @brief 静态接口，获取从1970年1月1日到现在的时间秒数
         * @return 返回时间秒数
         */
        static uint64_t currentSecsSinceEpoch();

        /**
         * @brief 静态接口，获取从1970年1月1日到现在的时间毫秒数.
         * @return 返回时间毫秒数.
         */
        static uint64_t currentMSecsSinceEpoch();

        /**
         * @brief 静态接口，把从1970年1月1日开始的毫秒数转换成T3DDateTime对象.
         * @param [in] msecs : 从1970年1月1日开始的毫秒数
         * @return 返回T3DDateTime对象.
         */
        static DateTime fromMSecsSinceEpoch(uint64_t msecs);

        /**
         * @brief 静态接口，把从1970年1月1日开始的秒数转换成T3DDateTime对象.
         * @param [in] msecs : 从1970年1月1日开始的秒数
         * @return 返回T3DDateTime对象.
         */
        static DateTime fromSecsSinceEpoch(uint64_t sces);

    public:
        /** 返回年 */
        int32_t Year() const
        {
            return m_nYear;
        }
        /** 返回月 */
        int32_t Month() const
        {
            return m_nMonth;
        }
        /** 返回日 */
        int32_t Day() const
        {
            return m_nDay;
        }
        /** 返回时 */
        int32_t Hour() const
        {
            return m_nHour;
        }
        /** 返回分 */
        int32_t Minute() const
        {
            return m_nMinute;
        }
        /** 返回秒 */
        int32_t Second() const
        {
            return m_nSecond;
        }
        /** 返回毫秒 */
        int32_t Millisecond() const
        {
            return m_nMillisecond;
        }

    private:
        int32_t m_nYear;
        int32_t m_nMonth;
        int32_t m_nDay;
        int32_t m_nHour;
        int32_t m_nMinute;
        int32_t m_nSecond;
        int32_t m_nMillisecond;
    };
}


#endif  /*__T3D_DATE_TIME_H__*/
