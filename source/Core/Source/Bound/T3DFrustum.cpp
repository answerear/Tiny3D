
#ifndef __T3D_COLOR4_H__
#define __T3D_COLOR4_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Color4
    {
    public:
        Color4(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
            : m_Alpha(a)
            , m_Red(r)
            , m_Green(g)
            , m_Blue(b)
        {

        }

        Color4(const Color4 &other)
        {
            m_Alpha = other.m_Alpha; m_Red = other.m_Red;
            m_Green = other.m_Green; m_Blue = other.m_Blue;
        }

        Color4 &operator =(const Color4 &other)
        {
            m_Alpha = other.m_Alpha; m_Red = other.m_Red;
            m_Green = other.m_Green; m_Blue = other.m_Blue;
            return *this;
        }

        bool operator ==(const Color4 &other) const
        {
            return (m_Alpha == other.m_Alpha && m_Red == other.m_Red
                && m_Green == other.m_Green && m_Blue == other.m_Blue);
        }

        uint8_t alpha() const   { return m_Alpha; }
        uint8_t &alpha()        { return m_Alpha; }

        uint8_t red() const     { return m_Red; }
        uint8_t &red()          { return m_Red; }

        uint8_t green() const   { return m_Green; }
        uint8_t &green()        { return m_Green; }

        uint8_t blue() const    { return m_Blue; }
        uint8_t &blue()         { return m_Blue; }

        uint32_t A8R8G8B8() const
        {
            return ((m_Alpha << 24) & 0xFF000000) | ((m_Red << 16) & 0x00FF0000)
                | ((m_Green << 8) & 0x0000FF00) | (m_Blue & 0x000000FF);
        }

        uint16_t A1R5G5B5() const
        {
            uint8_t alpha = (m_Alpha > 0 ? 1 : 0);
            return ((alpha << 15) & 0x8000) | ((m_Red << 10) & 0x7C00)
                | ((m_Green << 5) & 0x03E0) | (m_Blue & 0x001F);
        }

        uint16_t R5G6B5() const
        {
            return ((m_Red << 11) & 0xF800) | ((m_Green << 5) & 0x07E0) 
                | (m_Blue & 0x001F);
        }

        uint16_t A4R4G4B4() const
        {
            uint8_t alpha = (m_Alpha >> 1);
            return ((alpha << 12) & 0xF000) | ((m_Red << 8) & 0x0F00) 
                | ((m_Green << 4) & 0x00F0) | (m_Blue & 0x000F);
        }

        static const Color4 BLACK;
        static const Color4 WHITE;
        static const Color4 RED;
        static const Color4 GREEN;
        static const Color4 BLUE;

    private:
        uint8_t     m_Blue;
        uint8_t     m_Green;
        uint8_t     m_Red;
        uint8_t     m_Alpha;
    };
}


#endif  /*__T3D_COLOR4_H__*/
