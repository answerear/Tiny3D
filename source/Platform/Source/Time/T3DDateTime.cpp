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

#include "T3DDateTime.h"
#include <time.h>
#include "Adapter/T3DTimeInterface.h"
#include "T3DPlatform.h"
#include "Adapter/T3DFactoryInterface.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ITime *DateTime::msTime = nullptr;

    //--------------------------------------------------------------------------

    DateTime::DateTime()
        : mYear(0)
        , mMonth(0)
        , mDay(0)
        , mHour(0)
        , mMinute(0)
        , mSecond(0)
        , mMillisecond(0)
    {

    }

    //--------------------------------------------------------------------------

    DateTime::DateTime(int32_t year, int32_t month, int32_t day, 
        int32_t hour, int32_t minute, int32_t second, int32_t millisecond)
        : mYear(year)
        , mMonth(month)
        , mDay(day)
        , mHour(hour)
        , mMinute(minute)
        , mSecond(second)
        , mMillisecond(millisecond)
    {

    }

    //--------------------------------------------------------------------------

    int64_t DateTime::toMSecsSinceEpoch() const
    {
        tm dt;
        dt.tm_year = mYear - 1900;
        dt.tm_mon = mMonth - 1;
        dt.tm_mday = mDay;
        dt.tm_hour = mHour;
        dt.tm_min = mMinute;
        dt.tm_sec = mSecond;
        dt.tm_isdst = 0;

        time_t t = mktime(&dt);
        int64_t time = (int64_t)t * 1000 + mMillisecond;
        return time;
    }

    //--------------------------------------------------------------------------

    String DateTime::toString() const
    {
        char text[128];
        snprintf(text, 128, "%d-%02d-%02d %02d:%02d:%02d.%03d", 
            mYear, mMonth, mDay, mHour, mMinute, mSecond, mMillisecond);
        String s(text);

        return s;
    }

    //--------------------------------------------------------------------------

    String DateTime::timeToString(TimeFormat eFormat) const
    {
        char text[128];
        switch (eFormat)
        {
        case TimeFormat::HH_MM_SS_XXX:
            snprintf(text, 128, "%02d:%02d:%02d.%03d", 
                mHour, mMinute, mSecond, mMillisecond);
            break;

        case TimeFormat::HH_MM_SS:
            snprintf(text, 128, "%02d:%02d:%02d", mHour, mMinute, mSecond);
            break;

        case TimeFormat::HH_MM:
            snprintf(text, 128, "%02d:%02d", mHour, mMinute);
            break;

        case TimeFormat::HHMMSS:
            snprintf(text, 128, "%02d%02d%02d", mHour, mMinute, mSecond);
            break;

        default:
            snprintf(text, 128, "%02d:%02d:%02d.%03d", 
                mHour, mMinute, mSecond, mMillisecond);
            break;
        }

        String s(text);
        return s;
    }

    //--------------------------------------------------------------------------

    String DateTime::dateToString(DateFormat eFormat) const
    {
        char text[128];
        switch (eFormat)
        {
        case DateFormat::YY_MM_DD:
            snprintf(text, 128, "%d-%02d-%02d", mYear, mMonth, mDay);
            break;

        case DateFormat::MM_DD:
            snprintf(text, 128, "%02d-%02d", mMonth, mDay);
            break;

        case DateFormat::YY_MM:
            snprintf(text, 128, "%d-%02d", mYear, mMonth);
            break;
        case DateFormat::YYMMDD:
            snprintf(text, 128, "%d%02d%02d", mYear, mMonth, mDay);
            break;
        default:
            snprintf(text, 128, "%d-%02d-%02d", mYear, mMonth, mDay);
            break;
        }

        String s(text);
        return s;
    }

    //--------------------------------------------------------------------------

    bool DateTime::operator ==(const DateTime &other) const
    {
        return (other.mYear == mYear 
            && other.mMonth == mMonth 
            && other.mDay == mDay
            && other.mHour == mHour 
            && other.mMinute == mMinute 
            && other.mSecond == mSecond
            && other.mMillisecond == mMinute);
    }

    //--------------------------------------------------------------------------

    bool DateTime::operator !=(const DateTime &other) const
    {
        return (other.mYear != mYear 
            || other.mMonth != mMonth 
            || other.mDay != mDay 
            || other.mHour != mHour 
            || other.mMinute != mMinute 
            || other.mSecond != mSecond 
            || other.mMillisecond != mMinute);
    }

    //--------------------------------------------------------------------------

    bool DateTime::operator <(const DateTime &other) const
    {
        return (mYear < other.mYear
            || (mYear == other.mYear && mMonth < other.mMonth)
            || (mYear == other.mYear && mMonth == other.mMonth 
                && mDay < other.mDay)
            || (mYear == other.mYear && mMonth == other.mMonth 
                && mDay == other.mDay && mHour < other.mHour)
            || (mYear == other.mYear && mMonth == other.mMonth 
                && mDay == other.mDay && mHour == other.mHour 
                && mMinute < other.mMinute)
            || (mYear == other.mYear && mMonth == other.mMonth 
                && mDay == other.mDay && mHour == other.mHour 
                && mMinute == other.mMinute && mSecond < other.mSecond)
            || (mYear == other.mYear && mMonth == other.mMonth 
                && mDay == other.mDay && mHour == other.mHour 
                && mMinute == other.mMinute && mSecond == other.mSecond
                && mMillisecond < other.mMillisecond));
    }

    //--------------------------------------------------------------------------

    bool DateTime::operator <=(const DateTime &other) const
    {
        return (*this < other || *this == other);
    }

    //--------------------------------------------------------------------------

    bool DateTime::operator >(const DateTime &other) const
    {
        return (mYear > other.mYear
            || (mYear == other.mYear && mMonth > other.mMonth)
            || (mYear == other.mYear && mMonth == other.mMonth 
                && mDay > other.mDay)
            || (mYear == other.mYear && mMonth == other.mMonth 
                && mDay == other.mDay && mHour > other.mHour)
            || (mYear == other.mYear && mMonth == other.mMonth 
                && mDay == other.mDay && mHour == other.mHour 
                && mMinute > other.mMinute)
            || (mYear == other.mYear && mMonth == other.mMonth 
                && mDay == other.mDay && mHour == other.mHour 
                && mMinute == other.mMinute && mSecond > other.mSecond)
            || (mYear == other.mYear && mMonth == other.mMonth 
                && mDay == other.mDay && mHour == other.mHour 
                && mMinute == other.mMinute && mSecond == other.mSecond
                && mMillisecond > other.mMillisecond));
    }

    //--------------------------------------------------------------------------

    bool DateTime::operator >=(const DateTime &other) const
    {
        return (*this > other || *this == other);
    }

    //--------------------------------------------------------------------------

    DateTime DateTime::currentDateTime()
    {
//         timeb timebuffer;
//         ftime(&timebuffer);
//         time_t time = timebuffer.time;
//         int32_t millisecond = timebuffer.millitm;
//         int32_t timezone = timebuffer.timezone * 60;
//         time = time - timezone;
        time_t time = getTime()->currentSecsSinceEpoch();
        int32_t millisecond = time % 1000;
        tm *t = localtime(&time);
        return DateTime(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
            t->tm_hour, t->tm_min, t->tm_sec, millisecond);
    }

    //--------------------------------------------------------------------------

    DateTime DateTime::currentDateTimeUTC()
    {
//         timeb timebuffer;
//         ftime(&timebuffer);
//         time_t time = timebuffer.time;
//         int32_t millisecond = timebuffer.millitm;
        time_t time = getTime()->currentSecsSinceEpoch();
        int32_t millisecond = time % 1000;
        tm *t = gmtime(&time);
        return DateTime(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
            t->tm_hour, t->tm_min, t->tm_sec, millisecond);
    }

    //--------------------------------------------------------------------------

    int64_t DateTime::currentSecsSinceEpoch()
    {
//         timeb timebuffer;
//         ftime(&timebuffer);
//         time_t time = timebuffer.time;
//         return time;
        return getTime()->currentSecsSinceEpoch();
    }

    //--------------------------------------------------------------------------

    int64_t DateTime::currentMSecsSinceEpoch()
    {
//         timeb timebuffer;
//         ftime(&timebuffer);
//         time_t time = timebuffer.time;
//         int32_t millisecond = timebuffer.millitm;
//         uint64_t t = (uint64_t)time * 1000 + millisecond;
//         return t;
        return getTime()->currentMSecsSinceEpoch();
    }

    //--------------------------------------------------------------------------

    DateTime DateTime::fromMSecsSinceEpoch(int64_t msecs)
    {
        time_t s = msecs / 1000;
        int32_t ms = msecs % 1000;
        tm *t = localtime(&s);
        return DateTime(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
            t->tm_hour, t->tm_min, t->tm_sec, ms);
    }

    //--------------------------------------------------------------------------

    DateTime DateTime::fromSecsSinceEpoch(int64_t sces)
    {
        time_t s = sces;
        tm *t = localtime(&s);
        return DateTime(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
            t->tm_hour, t->tm_min, t->tm_sec, 0);
    }

    //--------------------------------------------------------------------------

    ITime *DateTime::getTime()
    {
        if (msTime == nullptr)
        {
            msTime = T3D_PLATFORM_FACTORY.createPlatformTime();
        }

        return msTime;
    }

    //--------------------------------------------------------------------------

    void DateTime::exit()
    {
        T3D_SAFE_DELETE(msTime);
    }

    //--------------------------------------------------------------------------
}
