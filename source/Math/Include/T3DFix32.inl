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

#include "rttr/registration.h"

namespace Tiny3D
{
    inline fix32::fix32() : m(0)
    {

    }

    inline fix32::fix32(float32_t value)
    {
        T3D_ASSERT(value <= MAX_FLOAT_VALUE &&value >= MIN_FLOAT_VALUE, "fix32 constructor value from float overflow !");
        m = (int32_t)(value * (1 << DECIMAL_BITS));
    }

    inline fix32::fix32(float64_t value)
    {
        T3D_ASSERT(value <= MAX_FLOAT_VALUE &&value >= MIN_FLOAT_VALUE, "fix32 constructor value from double overflow !");
        m = (int32_t)(value * (1 << DECIMAL_BITS));
    }

    inline fix32::fix32(int32_t value)
    {
        T3D_ASSERT(value <= MAX_INT_VALUE && value >= MIN_INT_VALUE, "fix32 constructor value from int32 overflow !");
        m = (value << DECIMAL_BITS);
    }

    inline fix32::fix32(int64_t value)
    {
        T3D_ASSERT(value <= 8796093022207 && value >= -8796093022208, "fix32 constructor value from int64 overflow !");
        m = (int32_t)((value + 2048) >> DECIMAL_BITS);
    }

    inline fix32::fix32(int32_t value, int32_t r)
    {
        m = value;
    }

    inline fix32::fix32(const fix32 &value)
    {
        m = value.m;
    }

    //--------------------------------------------------------------------------

    inline int32_t fix32::mantissa() const
    {
        return m;
    }

    inline int32_t &fix32::mantissa()
    {
        return m;
    }

    inline fix32 fix32::operator +() const
    {
        return fix32(m, 0);
    }

    inline fix32 fix32::operator -() const
    {
        return fix32(-m, 0);
    }

    //--------------------------------------------------------------------------

    inline bool fix32::eq_0() const
    {
        return (m == 0);
    }

    inline bool fix32::ne_0() const
    {
        return (m != 0);
    }

    inline bool fix32::gt_0() const
    {
        return (m > 0);
    }

    inline bool fix32::ge_0() const
    {
        return (m >= 0);
    }

    inline bool fix32::lt_0() const
    {
        return (m < 0);
    }

    inline bool fix32::le_0() const
    {
        return (m <= 0);
    }

    inline bool fix32::eq_1() const
    {
        return (m == (1 << DECIMAL_BITS));
    }

    inline bool fix32::is_INF() const
    {
        return (m == 0x7FFFFFFF);
    }

    inline bool fix32::is_MINUSINF() const
    {
        return (m == 0x80000000 || m == 0x80000001);
    }

    inline bool fix32::is_integer() const
    {
        return ((m & ((1 << DECIMAL_BITS) - 1)) == 0);
    }

    //--------------------------------------------------------------------------

    inline fix32 &fix32::operator =(const fix32 &value)
    {
        m = value.m;
        return *this;
    }

    inline fix32 operator +(const fix32 &fx, const fix32 &gx)
    {
        T3D_ASSERT(((fx.m ^ -gx.m) & 0x80000000) 
            || !((fx.m ^ (fx.m + gx.m)) & 0x80000000), "Add two fix32 value overflow !");
        return fix32(fx.m + gx.m, 0);
    }

    inline fix32 operator -(const fix32 &fx, const fix32 &gx)
    {
        T3D_ASSERT(((fx.m ^ -gx.m) & 0x80000000) 
            || !((fx.m ^ (fx.m + gx.m)) & 0x80000000), "Sub two fix32 value overflow !");
        return fix32(fx.m - gx.m, 0);
    }

    inline fix32 operator *(const fix32 &fx, const fix32 &gx)
    {
        if (fx.eq_0() || gx.eq_0())
            return fix32::ZERO;
        if (fx.eq_1())
            return gx;
        if (gx.eq_1())
            return fx;

        int64_t value = fx.m;
        value *= gx.m;

        T3D_ASSERT(value <= 8796093022207LL && value >= -8796093022208LL, "Mul two fix32 value overflow !");
        return fix32(value);
    }

    inline fix32 operator /(const fix32 &fx, const fix32 &gx)
    {
        if (fx.eq_0())
            return fix32::ZERO;
        if (gx.eq_1())
            return fx;
        if (gx.eq_0())
            return fix32::INF;

        int64_t value = fx.m;
        value <<= fix32::DECIMAL_BITS;
        value /= gx.m;

        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Div two fix32 value overflow !");
        return fix32((int32_t)value, 0);
    }

    //--------------------------------------------------------------------------

    inline fix32 &fix32::operator +=(const fix32 &value)
    {
        return *this = *this + value;
    }

    inline fix32 &fix32::operator -=(const fix32 &value)
    {
        return *this = *this - value;
    }

    inline fix32 &fix32::operator *=(const fix32 &value)
    {
        return *this = *this * value;
    }

    inline fix32 &fix32::operator /=(const fix32 &value)
    {
        return *this = *this / value;
    }

    inline bool operator ==(const fix32 &fx, const fix32 &gx)
    {
        return fx.m == gx.m;
    }

    inline bool operator !=(const fix32 &fx, const fix32 &gx)
    {
        return fx.m != gx.m;
    }

    inline bool operator >=(const fix32 &fx, const fix32 &gx)
    {
        return fx.m >= gx.m;
    }

    inline bool operator <=(const fix32 &fx, const fix32 &gx)
    {
        return fx.m <= gx.m;
    }

    inline bool operator >(const fix32 &fx, const fix32 &gx)
    {
        return fx.m > gx.m;
    }

    inline bool operator <(const fix32 &fx, const fix32 &gx)
    {
        return fx.m < gx.m;
    }

    //--------------------------------------------------------------------------

    inline fix32 &fix32::operator =(int32_t value)
    {
        T3D_ASSERT(value <= MAX_INT_VALUE && value >= MIN_INT_VALUE, "Assigning a fix32 value from a int32 overflow !");
        m = (value << DECIMAL_BITS);
        return *this;
    }

    inline fix32 operator +(const fix32 &fx, int32_t value)
    {
        T3D_ASSERT(((fx.m ^ (value << fix32::DECIMAL_BITS)) & 0x80000000)
            || !((fx.m ^ (fx.m + (value << fix32::DECIMAL_BITS))) & 0x80000000), "Adding a fix32 value to a int32 value overflow !");
        return fix32(fx.m + (value << fix32::DECIMAL_BITS), 0);
    }

    inline fix32 operator +(int32_t value, const fix32 &fx)
    {
        T3D_ASSERT(((fx.m ^ (value << fix32::DECIMAL_BITS)) & 0x80000000)
            || !((fx.m ^ (fx.m + (value << fix32::DECIMAL_BITS))) & 0x80000000), "Adding a int32 value to a fix32 value overflow !");
        return fix32(fx.m + (value << fix32::DECIMAL_BITS), 0);
    }

    inline fix32 operator - (const fix32 &fx, int32_t value)
    {
        T3D_ASSERT(((fx.m ^ (value << fix32::DECIMAL_BITS)) & 0x80000000)
            || !((fx.m ^ (fx.m + (value << fix32::DECIMAL_BITS))) & 0x80000000), "Substracting a fix32 value from a int32 value overflow !");
        return fix32(fx.m - (value << fix32::DECIMAL_BITS), 0);
    }

    inline fix32 operator -(int32_t value, const fix32 &fx)
    {
        T3D_ASSERT(((fx.m ^ (value << fix32::DECIMAL_BITS)) & 0x80000000)
            || !((fx.m ^ (fx.m + (value << fix32::DECIMAL_BITS))) & 0x80000000), "Substracting a int32 value from a fix32 value overflow !");
        return fix32((value << fix32::DECIMAL_BITS) - fx.m, 0);
    }

    inline fix32 operator *(const fix32 &fx, int32_t value)
    {
#ifdef T3D_DEBUG
        int32_t result = (int32_t)(((int64_t)fx.m * value) >> 44);
        T3D_ASSERT(result == 0 || result == -1, "result == 0 || result == -1");
#endif
        return fix32(fx.m * value, 0);
    }

    inline fix32 operator *(int32_t value, const fix32 &fx)
    {
#ifdef T3D_DEBUG
        int32_t result = (int32_t)(((int64_t)fx.m * value) >> 44);
        T3D_ASSERT(result == 0 || result == -1, "result == 0 || result == -1");
#endif
        return fix32(fx.m * value, 0);
    }

    inline fix32 operator /(const fix32 &fx, int32_t value)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Deviding a fix32 value into a int32 value overflow !");
        
        if (value == 0)
        {
            return (fx.lt_0() ? fix32::MINUSINF : fix32::INF);
        }
        
        return fix32(fx.m / value, 0);
    }

    inline fix32 operator /(int32_t value, const fix32 &fx)
    {
        T3D_ASSERT(fx.ne_0(), "Devisor can not be zero !");
        
        if (fx.eq_0()) 
        {
            T3D_ASSERT(value == 0, "Devisor is zero !");
            return (value < 0 ? fix32::MINUSINF : fix32::INF);
        }
        
        return fix32((int32_t)(((int64_t)value << fix32::DECIMAL_BITS) 
            / fx.m), 0);
    }

    inline fix32 &fix32::operator +=(int32_t value)
    {
        return *this = *this + value;
    }

    inline fix32 &fix32::operator -=(int32_t value)
    {
        return *this = *this - value;
    }

    inline fix32 &fix32::operator *=(int32_t value)
    {
        return *this = *this * value;
    }

    inline fix32 &fix32::operator /=(int32_t value)
    {
        return *this = *this / value;
    }

    inline bool operator ==(const fix32 &fx, int32_t value)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Equal a fix32 value with a int32 value overflow !");
        return (fx.m == (value << fix32::DECIMAL_BITS));
    }

    inline bool operator ==(int32_t value, const fix32 &fx)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Equal a fix32 value with a int32 value overflow !");
        return (fx.m == (value << fix32::DECIMAL_BITS));
    }

    inline bool operator !=(const fix32 &fx, int32_t value)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Not equal value overflow !");
        return (fx.m != (value << fix32::DECIMAL_BITS));
    }

    inline bool operator !=(int32_t value, const fix32 &fx)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Not equal value overflow !");
        return (fx.m != (value << fix32::DECIMAL_BITS));
    }

    inline bool operator >=(const fix32 &fx, int32_t value)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Greater or equal value overflow !");
        return (fx.m >= (value << fix32::DECIMAL_BITS));
    }

    inline bool operator >=(int32_t value, const fix32 &fx)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Greater or equal value overflow !");
        return ((value << fix32::DECIMAL_BITS) >= fx.m);
    }

    inline bool operator <=(const fix32 &fx, int32_t value)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Less or equal value overflow !");
        return (fx.m <= (value << fix32::DECIMAL_BITS));
    }

    inline bool operator <=(int32_t value, const fix32 &fx)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Less or equal value overflow !");
        return ((value << fix32::DECIMAL_BITS) <= fx.m);
    }

    inline bool operator >(const fix32 &fx, int32_t value)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Greater value overflow !");
        return (fx.m > (value << fix32::DECIMAL_BITS));
    }

    inline bool operator >(int32_t value, const fix32 &fx)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Greater value overflow !");
        return ((value << fix32::DECIMAL_BITS) > fx.m);
    }

    inline bool operator <(const fix32 &fx, int32_t value)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Less value overflow !");
        return (fx.m < (value << fix32::DECIMAL_BITS));
    }

    inline bool operator <(int32_t value, const fix32 &fx)
    {
        T3D_ASSERT(value <= fix32::MAX_INT_VALUE 
            && value >= fix32::MIN_INT_VALUE, "Less value overflow !");
        return ((value << fix32::DECIMAL_BITS) < fx.m);
    }

    inline fix32::operator int32_t() const
    {
        return (m >> 12);
    }

    //--------------------------------------------------------------------------

    inline fix32 &fix32::operator =(float32_t value)
    {
        T3D_ASSERT(value <= fix32::MAX_FLOAT_VALUE 
            && value >= fix32::MIN_FLOAT_VALUE, "Assignment value overflow !");
        m = (int32_t)(value * (1 << fix32::DECIMAL_BITS));
        return *this;
    }

    inline fix32 operator +(const fix32 &fx, float32_t value)
    {
        return fx + fix32(value);
    }

    inline fix32 operator +(float32_t value, const fix32 &fx)
    {
        return fx + fix32(value);
    }

    inline fix32 operator -(const fix32 &fx, float32_t value)
    {
        return fx - fix32(value);
    }

    inline fix32 operator -(float32_t value, const fix32 &fx)
    {
        return fix32(value) - fx;
    }

    inline fix32 operator *(const fix32 &fx, float32_t value)
    {
        return fx * fix32(value);
    }

    inline fix32 operator *(float32_t value, const fix32 &fx)
    {
        return fx * fix32(value);
    }

    inline fix32 operator /(const fix32 &fx, float32_t value)
    {
        return fx / fix32(value);
    }

    inline fix32 operator /(float32_t value, const fix32 &fx)
    {
        return fix32(value) / fx;
    }

    inline fix32 &fix32::operator +=(float32_t value)
    {
        return *this = *this + value;
    }

    inline fix32 &fix32::operator -=(float32_t value)
    {
        return *this = *this - value;
    }

    inline fix32 &fix32::operator *=(float32_t value)
    {
        return *this = *this * value;
    }

    inline fix32 &fix32::operator /=(float32_t value)
    {
        return *this = *this / value;
    }

    inline bool operator ==(const fix32 &fx, float32_t value)
    {
        return (fx == fix32(value));
    }

    inline bool operator ==(float32_t value, const fix32 &fx)
    {
        return (fx == fix32(value));
    }

    inline bool operator !=(const fix32 &fx, float32_t value)
    {
        return (fx != fix32(value));
    }

    inline bool operator !=(float32_t value, const fix32 &fx)
    {
        return (fx != fix32(value));
    }

    inline bool operator >=(const fix32 &fx, float32_t value)
    {
        return (fx >= fix32(value));
    }

    inline bool operator >=(float32_t value, const fix32 &fx)
    {
        return (fix32(value) >= fx);
    }

    inline bool operator <=(const fix32 &fx, float32_t value)
    {
        return (fx <= fix32(value));
    }

    inline bool operator <=(float32_t value, const fix32 &fx)
    {
        return (fix32(value) <= fx);
    }

    inline bool operator >(const fix32 &fx, float32_t value)
    {
        return (fx > fix32(value));
    }

    inline bool operator >(float32_t value, const fix32 &fx)
    {
        return (fix32(value) > fx);
    }

    inline bool operator <(const fix32 &fx, float32_t value)
    {
        return (fx < fix32(value));
    }

    inline bool operator <(float32_t value, const fix32 &fx)
    {
        return (fix32(value) < fx);
    }

    //--------------------------------------------------------------------------
    
    inline fix32::operator float32_t() const
    {
        return (float32_t)m / float32_t(1 << DECIMAL_BITS);
    }

    //--------------------------------------------------------------------------

    inline fix32 operator <<(const fix32 &a, int32_t b)
    {
        T3D_ASSERT(b >= 0, "Outstreaming operator value must be greater than zero !");
        return fix32((a.m << b), 0);
    }

    inline fix32 operator >> (const fix32 &a, int32_t b)
    {
        T3D_ASSERT(b >= 0, "Instreaming operator value must be greater than zero !");
        return fix32((a.m >> b), 0);
    }

    inline fix32 &fix32::operator <<=(int32_t b)
    {
        return *this = *this << b;
    }

    inline fix32 &fix32::operator >>=(int32_t b)
    {
        return *this = *this >> b;
    }

    //--------------------------------------------------------------------------

    inline fix32::operator int64_t() const
    {
        return ((int64_t)m << DECIMAL_BITS);
    }

    //--------------------------------------------------------------------------

    inline std::ostream &operator <<(std::ostream &os, const fix32 &val)
    {
        os << float32_t(val);
        return os;
    }

    //--------------------------------------------------------------------------

    inline std::istream &operator >>(std::istream &is, fix32 &val)
    {
        float32_t fval;
        is >> fval;
        val = fval;
        return is;
    }
    
    //--------------------------------------------------------------------------
}
