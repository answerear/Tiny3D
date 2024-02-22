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

namespace Tiny3D
{
    inline fix64::fix64() : m(0)
    {

    }

    inline fix64::fix64(const fix32 &fx)
    {
        float32_t value = (float32_t)fx;
        m = (int64_t)(value * (1 << DECIMAL_BITS));
    }

    inline fix64::fix64(float32_t value)
    {
//         float a = MIN_FLOAT_VALUE;
//         float b = MAX_FLOAT_VALUE;
        T3D_ASSERT(value <= MAX_FLOAT_VALUE && value >= MIN_FLOAT_VALUE, "fix64");
        m = (int64_t)(value * (1 << DECIMAL_BITS));
    }

    inline fix64::fix64(float64_t value)
    {
        T3D_ASSERT(value <= MAX_FLOAT_VALUE && value >= MIN_FLOAT_VALUE, "fix64");
        m = (int64_t)(value * (1 << DECIMAL_BITS));
    }

    inline fix64::fix64(int32_t value)
    {
        m = (((int64_t)value) << DECIMAL_BITS);
    }

    inline fix64::fix64(int64_t value)
    {
        T3D_ASSERT(value <= MAX_INT_VALUE && value >= MIN_INT_VALUE, "fix64");
        m = (value << DECIMAL_BITS);
    }

    inline fix64::fix64(int64_t value, int32_t r)
    {
        m = value;
    }

    inline fix64::fix64(const fix64 &value)
    {
        m = value.m;
    }

    //--------------------------------------------------------------------------

    inline int64_t fix64::mantissa() const
    {
        return m;
    }

    inline int64_t &fix64::mantissa()
    {
        return m;
    }

    inline fix64 fix64::operator +() const
    {
        return fix64(m, 0);
    }

    inline fix64 fix64::operator -() const
    {
        return fix64(-m, 0);
    }

    //--------------------------------------------------------------------------

    inline bool fix64::eq_0() const
    {
        return (m == 0);
    }

    inline bool fix64::ne_0() const
    {
        return (m != 0);
    }

    inline bool fix64::gt_0() const
    {
        return (m > 0);
    }

    inline bool fix64::ge_0() const
    {
        return (m >= 0);
    }

    inline bool fix64::lt_0() const
    {
        return (m < 0);
    }

    inline bool fix64::le_0() const
    {
        return (m <= 0);
    }

    inline bool fix64::eq_1() const
    {
        return (m == (1LL << DECIMAL_BITS));
    }

    inline bool fix64::is_INF() const
    {
        return (m == 0x7FFFFFFFFFFFFFFFLL);
    }

    inline bool fix64::is_MINUSINF() const
    {
        return (m == 0x8000000000000000LL || m == 0x800000000000001LL);
    }

    inline bool fix64::is_integer() const
    {
        return ((m & ((1 << DECIMAL_BITS) - 1)) == 0);
    }

    //--------------------------------------------------------------------------

    inline fix64 &fix64::operator =(const fix64 &value)
    {
        m = value.m;
        return *this;
    }

    inline fix64 operator +(const fix64 &fx, const fix64 &gx)
    {
        T3D_ASSERT(((fx.m ^ -gx.m) & 0x8000000000000000LL)
            || !((fx.m ^ (fx.m + gx.m)) & 0x8000000000000000LL), "fix64");
        return fix64(fx.m + gx.m, 0);
    }

    inline fix64 operator -(const fix64 &fx, const fix64 &gx)
    {
        T3D_ASSERT(((fx.m ^ -gx.m) & 0x8000000000000000LL)
            || !((fx.m ^ (fx.m + gx.m)) & 0x8000000000000000LL), "fix64");
        return fix64(fx.m - gx.m, 0);
    }

    inline fix64 operator *(const fix64 &fx, const fix64 &gx)
    {
        if (fx.eq_0() || gx.eq_0())
            return fix64::ZERO;
        if (fx.eq_1())
            return gx;
        if (gx.eq_1())
            return fx;

        int64_t value = fx.m;
        value *= gx.m;

        T3D_ASSERT(value <= fix64::MAX_INT_VALUE
            && value >= fix64::MIN_INT_VALUE, "fix64");
        return fix64(value, 0);
    }

    inline fix64 operator /(const fix64 &fx, const fix64 &gx)
    {
        if (fx.eq_0())
            return fix64::ZERO;
        if (gx.eq_1())
            return fx;
        if (gx.eq_0())
            return fix64::INF;

        int64_t value = fx.m;
        value <<= fix64::DECIMAL_BITS;
        value /= gx.m;

        T3D_ASSERT(value <= fix64::MAX_INT_VALUE
            && value >= fix64::MIN_INT_VALUE, "fix64");
        return fix64(value, 0);
    }

    //--------------------------------------------------------------------------

    inline fix64 &fix64::operator +=(const fix64 &value)
    {
        return *this = *this + value;
    }

    inline fix64 &fix64::operator -=(const fix64 &value)
    {
        return *this = *this - value;
    }

    inline fix64 &fix64::operator *=(const fix64 &value)
    {
        return *this = *this * value;
    }

    inline fix64 &fix64::operator /=(const fix64 &value)
    {
        return *this = *this / value;
    }

    inline bool operator ==(const fix64 &fx, const fix64 &gx)
    {
        return fx.m == gx.m;
    }

    inline bool operator !=(const fix64 &fx, const fix64 &gx)
    {
        return fx.m != gx.m;
    }

    inline bool operator >=(const fix64 &fx, const fix64 &gx)
    {
        return fx.m >= gx.m;
    }

    inline bool operator <=(const fix64 &fx, const fix64 &gx)
    {
        return fx.m <= gx.m;
    }

    inline bool operator >(const fix64 &fx, const fix64 &gx)
    {
        return fx.m > gx.m;
    }

    inline bool operator <(const fix64 &fx, const fix64 &gx)
    {
        return fx.m < gx.m;
    }

    //--------------------------------------------------------------------------

    inline fix64 &fix64::operator =(int32_t value)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= MAX_INT_VALUE && i64 >= MIN_INT_VALUE, "fix64");
        m = (i64 << DECIMAL_BITS);
        return *this;
    }

    inline fix64 operator +(const fix64 &fx, int32_t value)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(((fx.m ^ (i64 << fix64::DECIMAL_BITS))
            & 0x8000000000000000LL)
            || !((fx.m ^ (fx.m + (i64 << fix64::DECIMAL_BITS)))
                & 0x8000000000000000LL), "fix64");
        return fix64(fx.m + (i64 << fix64::DECIMAL_BITS), 0);
    }

    inline fix64 operator +(int32_t value, const fix64 &fx)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(((fx.m ^ (i64 << fix64::DECIMAL_BITS))
            & 0x8000000000000000LL)
            || !((fx.m ^ (fx.m + (i64 << fix64::DECIMAL_BITS)))
                & 0x8000000000000000LL), "fix64");
        return fix64(fx.m + (i64 << fix64::DECIMAL_BITS), 0);
    }

    inline fix64 operator - (const fix64 &fx, int32_t value)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(((fx.m ^ (i64 << fix64::DECIMAL_BITS))
            & 0x8000000000000000LL)
            || !((fx.m ^ (fx.m + (i64 << fix64::DECIMAL_BITS)))
                & 0x8000000000000000LL), "fix64");
        return fix64(fx.m - (i64 << fix64::DECIMAL_BITS), 0);
    }

    inline fix64 operator -(int32_t value, const fix64 &fx)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(((fx.m ^ (i64 << fix64::DECIMAL_BITS)) 
            & 0x8000000000000000LL)
            || !((fx.m ^ (fx.m + (i64 << fix64::DECIMAL_BITS))) 
                & 0x8000000000000000LL), "fix64");
        return fix64((i64 << fix64::DECIMAL_BITS) - fx.m, 0);
    }

    inline fix64 operator *(const fix64 &fx, int32_t value)
    {
        return fix64(fx.m * value, 0);
    }

    inline fix64 operator *(int32_t value, const fix64 &fx)
    {
        return fix64(fx.m * value, 0);
    }

    inline fix64 operator /(const fix64 &fx, int32_t value)
    {
        T3D_ASSERT(value <= fix64::MAX_INT_VALUE 
            && value >= fix64::MIN_INT_VALUE, "fix64");

        if (value == 0)
        {
            return (fx.lt_0() ? fix64::MINUSINF : fix64::INF);
        }

        return fix64(fx.m / value, 0);
    }

    inline fix64 operator /(int32_t value, const fix64 &fx)
    {
        T3D_ASSERT(fx.ne_0(), "fix64");

        if (fx.eq_0())
        {
            T3D_ASSERT(value == 0, "fix64");
            return (value < 0 ? fix64::MINUSINF : fix64::INF);
        }

        return fix64((((int64_t)value << fix64::DECIMAL_BITS) / fx.m), 0);
    }

    inline fix64 &fix64::operator +=(int32_t value)
    {
        return *this = *this + value;
    }

    inline fix64 &fix64::operator -=(int32_t value)
    {
        return *this = *this - value;
    }

    inline fix64 &fix64::operator *=(int32_t value)
    {
        return *this = *this * value;
    }

    inline fix64 &fix64::operator /=(int32_t value)
    {
        return *this = *this / value;
    }

    inline bool operator ==(const fix64 &fx, int32_t value)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return (fx.m == (i64 << fix64::DECIMAL_BITS));
    }

    inline bool operator ==(int32_t value, const fix64 &fx)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return (fx.m == (i64 << fix64::DECIMAL_BITS));
    }

    inline bool operator !=(const fix64 &fx, int32_t value)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return (fx.m != (i64 << fix64::DECIMAL_BITS));
    }

    inline bool operator !=(int32_t value, const fix64 &fx)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return (fx.m != (i64 << fix64::DECIMAL_BITS));
    }

    inline bool operator >=(const fix64 &fx, int32_t value)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return (fx.m >= (i64 << fix64::DECIMAL_BITS));
    }

    inline bool operator >=(int32_t value, const fix64 &fx)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return ((i64 << fix64::DECIMAL_BITS) >= fx.m);
    }

    inline bool operator <=(const fix64 &fx, int32_t value)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return (fx.m <= (i64 << fix64::DECIMAL_BITS));
    }

    inline bool operator <=(int32_t value, const fix64 &fx)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return ((i64 << fix64::DECIMAL_BITS) <= fx.m);
    }

    inline bool operator >(const fix64 &fx, int32_t value)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return (fx.m > (i64 << fix64::DECIMAL_BITS));
    }

    inline bool operator >(int32_t value, const fix64 &fx)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return ((i64 << fix64::DECIMAL_BITS) > fx.m);
    }

    inline bool operator <(const fix64 &fx, int32_t value)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return (fx.m < (i64 << fix64::DECIMAL_BITS));
    }

    inline bool operator <(int32_t value, const fix64 &fx)
    {
        int64_t i64 = (int64_t)value;
        T3D_ASSERT(i64 <= fix64::MAX_INT_VALUE && i64 >= fix64::MIN_INT_VALUE, "fix64");
        return ((i64 << fix64::DECIMAL_BITS) < fx.m);
    }

    inline fix64::operator int32_t() const
    {
        return (int32_t)(m >> DECIMAL_BITS);
    }

    //--------------------------------------------------------------------------

    inline fix64 &fix64::operator =(float32_t value)
    {
        T3D_ASSERT(value <= MAX_INT_VALUE && value >= MIN_INT_VALUE, "fix64");
        m = (int64_t)(value * (1 << DECIMAL_BITS));
        return *this;
    }

    inline fix64 operator +(const fix64 &fx, float32_t value)
    {
        return fx + fix64(value);
    }

    inline fix64 operator +(float32_t value, const fix64 &fx)
    {
        return fx + fix64(value);
    }

    inline fix64 operator -(const fix64 &fx, float32_t value)
    {
        return fx - fix64(value);
    }

    inline fix64 operator -(float32_t value, const fix64 &fx)
    {
        return fix64(value) - fx;
    }

    inline fix64 operator *(const fix64 &fx, float32_t value)
    {
        return fx * fix64(value);
    }

    inline fix64 operator *(float32_t value, const fix64 &fx)
    {
        return fx * fix64(value);
    }

    inline fix64 operator /(const fix64 &fx, float32_t value)
    {
        return fx / fix64(value);
    }

    inline fix64 operator /(float32_t value, const fix64 &fx)
    {
        return fix64(value) / fx;
    }

    inline fix64 &fix64::operator +=(float32_t value)
    {
        return *this = *this + value;
    }

    inline fix64 &fix64::operator -=(float32_t value)
    {
        return *this = *this - value;
    }

    inline fix64 &fix64::operator *=(float32_t value)
    {
        return *this = *this * value;
    }

    inline fix64 &fix64::operator /=(float32_t value)
    {
        return *this = *this / value;
    }

    inline bool operator ==(const fix64 &fx, float32_t value)
    {
        return (fx == fix64(value));
    }

    inline bool operator ==(float32_t value, const fix64 &fx)
    {
        return (fx == fix64(value));
    }

    inline bool operator !=(const fix64 &fx, float32_t value)
    {
        return (fx != fix64(value));
    }

    inline bool operator !=(float32_t value, const fix64 &fx)
    {
        return (fx != fix64(value));
    }

    inline bool operator >=(const fix64 &fx, float32_t value)
    {
        return (fx >= fix64(value));
    }

    inline bool operator >=(float32_t value, const fix64 &fx)
    {
        return (fix64(value) >= fx);
    }

    inline bool operator <=(const fix64 &fx, float32_t value)
    {
        return (fx <= fix64(value));
    }

    inline bool operator <=(float32_t value, const fix64 &fx)
    {
        return (fix64(value) <= fx);
    }

    inline bool operator >(const fix64 &fx, float32_t value)
    {
        return (fx > fix64(value));
    }

    inline bool operator >(float32_t value, const fix64 &fx)
    {
        return (fix64(value) > fx);
    }

    inline bool operator <(const fix64 &fx, float32_t value)
    {
        return (fx < fix64(value));
    }

    inline bool operator <(float32_t value, const fix64 &fx)
    {
        return (fix64(value) < fx);
    }

    inline fix64::operator float32_t() const
    {
        return (float32_t)m / 16777216.0f;
    }

    //--------------------------------------------------------------------------

    inline fix64 operator <<(const fix64 &a, int32_t b)
    {
        T3D_ASSERT(b >= 0, "fix64");
        return fix64((a.m << b), 0);
    }

    inline fix64 operator >> (const fix64 &a, int32_t b)
    {
        T3D_ASSERT(b >= 0, "fix64");
        return fix64((a.m >> b), 0);
    }

    inline fix64 &fix64::operator <<=(int32_t b)
    {
        return *this = *this << b;
    }

    inline fix64 &fix64::operator >>=(int32_t b)
    {
        return *this = *this >> b;
    }

    //--------------------------------------------------------------------------

    inline fix64::operator int64_t() const
    {
        return (m << DECIMAL_BITS);
    }

    //--------------------------------------------------------------------------

    inline fix64 &fix64::operator =(const fix32 &fx)
    {
        float32_t value = (float32_t)fx;
        m = (int64_t)(value * (1 << DECIMAL_BITS));
        return *this;
    }

    inline fix64::operator fix32() const
    {
        float32_t value = (float32_t)m / 16777216.0f;
        return fix32(value);
    }

    //--------------------------------------------------------------------------

    inline std::ostream &operator <<(std::ostream &os, const fix64 &val)
    {
        os << float32_t(val);
        return os;
    }

    //--------------------------------------------------------------------------

    inline std::istream &operator >>(std::istream &is, fix64 &val)
    {
        float32_t fval;
        is >> fval;
        val = fval;
        return is;
    }
    
    //--------------------------------------------------------------------------
}
