

#include "Time/T3DDateTime.h"
#include <time.h>
#include <sys/timeb.h>


namespace Tiny3D
{
    DateTime::DateTime()
        : m_nYear(0)
        , m_nMonth(0)
        , m_nDay(0)
        , m_nHour(0)
        , m_nMinute(0)
        , m_nSecond(0)
        , m_nMillisecond(0)
    {

    }

    DateTime::DateTime(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, int32_t millisecond)
        : m_nYear(year)
        , m_nMonth(month)
        , m_nDay(day)
        , m_nHour(hour)
        , m_nMinute(minute)
        , m_nSecond(second)
        , m_nMillisecond(millisecond)
    {

    }

    uint64_t DateTime::toMSecsSinceEpoch() const
    {
        tm dt;
        dt.tm_year = m_nYear;
        dt.tm_mon = m_nMonth;
        dt.tm_mday = m_nDay;
        dt.tm_hour = m_nHour;
        dt.tm_min = m_nMinute;
        dt.tm_sec = m_nSecond;

        time_t t = mktime(&dt);
        uint64_t time = t * 1000 + m_nMillisecond;
        return time;
    }

    TString DateTime::toString() const
    {
        char text[128];
        snprintf(text, 128, "%d-%02d-%02d %02d:%02d:%02d.%03d", m_nYear, m_nMonth, m_nDay, m_nHour, m_nMinute, m_nSecond, m_nMillisecond);
        TString s(text);
        return s;
    }

    TString DateTime::timeToString(ETimeFormat eFormat /* = E_HH_MM_SS_XXX */) const
    {
        char text[128];
        switch (eFormat)
        {
        case E_HH_MM_SS_XXX:
            snprintf(text, 128, "%02d:%02d:%02d.%03d", m_nHour, m_nMinute, m_nSecond, m_nMillisecond);
            break;

        case E_HH_MM_SS:
            snprintf(text, 128, "%02d:%02d:%02d", m_nHour, m_nMinute, m_nSecond);
            break;

        case E_HH_MM:
            snprintf(text, 128, "%02d:%02d", m_nHour, m_nMinute);
            break;

        default:
            snprintf(text, 128, "%02d:%02d:%02d.%03d", m_nHour, m_nMinute, m_nSecond, m_nMillisecond);
            break;
        }

        TString s(text);
        return s;
    }

    TString DateTime::dateToString(EDateFormat eFormat /* = E_YY_MM_DD */) const
    {
        char text[128];
        switch (eFormat)
        {
        case E_YY_MM_DD:
            snprintf(text, 128, "%d-%02d-%02d", m_nYear, m_nMonth, m_nDay);
            break;

        case E_MM_DD:
            snprintf(text, 128, "%02d-%02d", m_nMonth, m_nDay);
            break;

        case E_YY_MM:
            snprintf(text, 128, "%d-%02d", m_nYear, m_nMonth);
            break;

        default:
            snprintf(text, 128, "%d-%02d-%02d", m_nYear, m_nMonth, m_nDay);
            break;
        }

        TString s(text);
        return s;
    }

    bool DateTime::operator ==(const DateTime &other) const
    {
        return (other.m_nYear == m_nYear && other.m_nMonth == m_nMonth && other.m_nDay == m_nDay
                && other.m_nHour == m_nHour && other.m_nMinute == m_nMinute && other.m_nSecond == m_nSecond
                && other.m_nMillisecond == m_nMinute);
    }

    bool DateTime::operator !=(const DateTime &other) const
    {
        return (other.m_nYear != m_nYear || other.m_nMonth != m_nMonth || other.m_nDay != m_nDay
                || other.m_nHour != m_nHour || other.m_nMinute != m_nMinute || other.m_nSecond != m_nSecond
                || other.m_nMillisecond != m_nMinute);
    }

    bool DateTime::operator <(const DateTime &other) const
    {
        return (m_nYear < other.m_nYear
                || (m_nYear == other.m_nYear && m_nMonth < other.m_nMonth)
                || (m_nYear == other.m_nYear && m_nMonth == other.m_nMonth && m_nDay < other.m_nDay)
                || (m_nYear == other.m_nYear && m_nMonth == other.m_nMonth && m_nDay == other.m_nDay
                    && m_nHour < other.m_nHour)
                || (m_nYear == other.m_nYear && m_nMonth == other.m_nMonth && m_nDay == other.m_nDay
                    && m_nHour == other.m_nHour && m_nMinute < other.m_nMinute)
                || (m_nYear == other.m_nYear && m_nMonth == other.m_nMonth && m_nDay == other.m_nDay
                    && m_nHour == other.m_nHour && m_nMinute == other.m_nMinute && m_nSecond < other.m_nSecond)
                || (m_nYear == other.m_nYear && m_nMonth == other.m_nMonth && m_nDay == other.m_nDay
                    && m_nHour == other.m_nHour && m_nMinute == other.m_nMinute && m_nSecond == other.m_nSecond
                    && m_nMillisecond < other.m_nMillisecond));
    }

    bool DateTime::operator <=(const DateTime &other) const
    {
        return (*this < other || *this == other);
    }

    bool DateTime::operator >(const DateTime &other) const
    {
        return (m_nYear > other.m_nYear
                || (m_nYear == other.m_nYear && m_nMonth > other.m_nMonth)
                || (m_nYear == other.m_nYear && m_nMonth == other.m_nMonth && m_nDay > other.m_nDay)
                || (m_nYear == other.m_nYear && m_nMonth == other.m_nMonth && m_nDay == other.m_nDay
                    && m_nHour > other.m_nHour)
                || (m_nYear == other.m_nYear && m_nMonth == other.m_nMonth && m_nDay == other.m_nDay
                    && m_nHour == other.m_nHour && m_nMinute > other.m_nMinute)
                || (m_nYear == other.m_nYear && m_nMonth == other.m_nMonth && m_nDay == other.m_nDay
                    && m_nHour == other.m_nHour && m_nMinute == other.m_nMinute && m_nSecond > other.m_nSecond)
                || (m_nYear == other.m_nYear && m_nMonth == other.m_nMonth && m_nDay == other.m_nDay
                    && m_nHour == other.m_nHour && m_nMinute == other.m_nMinute && m_nSecond == other.m_nSecond
                    && m_nMillisecond > other.m_nMillisecond));
    }

    bool DateTime::operator >=(const DateTime &other) const
    {
        return (*this > other || *this == other);
    }

    DateTime DateTime::currentDateTime()
    {
        timeb timebuffer;
        ftime(&timebuffer);
        time_t time = timebuffer.time;
        int32_t millisecond = timebuffer.millitm;
        int32_t timezone = timebuffer.timezone * 60;
        time = time - timezone;
        tm *t = gmtime(&time);
        return DateTime(t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, millisecond);
    }

    DateTime DateTime::currentDateTimeUTC()
    {
        timeb timebuffer;
        ftime(&timebuffer);
        time_t time = timebuffer.time;
        int32_t millisecond = timebuffer.millitm;
        //      time = time - timezone;
        tm *t = gmtime(&time);
        return DateTime(t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, millisecond);
    }

    uint64_t DateTime::currentSecsSinceEpoch()
    {
        timeb timebuffer;
        ftime(&timebuffer);
        time_t time = timebuffer.time;
        //int32_t millisecond = timebuffer.millitm;
        //int32_t timezone = timebuffer.timezone * 60;
        //      time = time - timezone;
        return time;
    }

    uint64_t DateTime::currentMSecsSinceEpoch()
    {
        timeb timebuffer;
        ftime(&timebuffer);
        time_t time = timebuffer.time;
        int32_t millisecond = timebuffer.millitm;
        //int32_t timezone = timebuffer.timezone * 60;
        //      time = time - timezone;
        uint64_t t = (uint64_t)time * 1000 + millisecond;
        return t;
    }

    DateTime DateTime::fromMSecsSinceEpoch(uint64_t msecs)
    {
        time_t s = msecs / 1000;
        int32_t ms = msecs % 1000;
        tm *t = localtime(&s);
        return DateTime(t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, ms);
    }

    DateTime DateTime::fromSecsSinceEpoch(uint64_t sces)
    {
        time_t s = sces;
        tm *t = localtime(&s);
        return DateTime(t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, 0);
    }
}
