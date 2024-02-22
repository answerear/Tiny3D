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

namespace  Tiny3D
{
    //--------------------------------------------------------------------------

    inline void Material::setInteger(const String &name, int32_t value)
    {
        setValue<int32_t>(name, &value);
    }

    //--------------------------------------------------------------------------

    inline int32_t Material::getInteger(const String &name) const
    {
        return getValue<int32_t>(name);
    }
    
    //--------------------------------------------------------------------------

    inline bool Material::hasInteger(const String &name) const
    {
        return hasValue<int32_t>(name);
    }

    //--------------------------------------------------------------------------

    inline void Material::setFloat(const String &name, float32_t value)
    {
        setValue<float32_t>(name, &value);
    }

    //--------------------------------------------------------------------------

    inline float32_t Material::getFloat(const String &name) const
    {
        return getValue<float32_t>(name);
    }

    //--------------------------------------------------------------------------

    inline bool Material::hasFloat(const String &name) const
    {
        return hasValue<float32_t>(name);
    }

    //--------------------------------------------------------------------------

    inline void Material::setFloatArray(const String &name, const FloatArray &values)
    {
        setValue<float32_t>(name, values.data());
    }

    //--------------------------------------------------------------------------

    inline FloatArray Material::getFloatArray(const String &name) const
    {
        return getValues<float_t>(name);
    }

    //--------------------------------------------------------------------------

    inline void Material::setColor(const String &name, const ColorRGBA &value)
    {
        setValue<ColorRGBA>(name, &value);
    }

    //--------------------------------------------------------------------------

    inline ColorRGBA Material::getColor(const String &name) const
    {
        return getValue<ColorRGBA>(name);
    }

    //--------------------------------------------------------------------------

    inline bool Material::hasColor(const String &name) const
    {
        return hasValue<ColorRGBA>(name);
    }

    //--------------------------------------------------------------------------

    inline void Material::setColorArray(const String &name, const ColorArray &values)
    {
        setValue<uint8_t>(name, values.data());
    }

    //--------------------------------------------------------------------------

    inline ColorArray Material::getColorArray(const String &name) const
    {
        return getValues<ColorRGBA>(name);
    }

    //--------------------------------------------------------------------------

    inline void Material::setVector(const String &name, const Vector4 &value)
    {
        setValue<Vector4>(name, &value);
    }

    //--------------------------------------------------------------------------

    inline Vector4 Material::getVector(const String &name) const
    {
        return getValue<Vector4>(name);
    }

    //--------------------------------------------------------------------------

    inline bool Material::hasVector(const String &name) const
    {
        return hasValue<Vector4>(name);
    }

    //--------------------------------------------------------------------------

    inline void Material::setVectorArray(const String &name, const Vector4Array &values)
    {
        setValue<Vector4>(name, values.data());
    }

    //--------------------------------------------------------------------------

    inline Vector4Array Material::getVectorArray(const String &name) const
    {
        return getValues<Vector4>(name);
    }
    
    //--------------------------------------------------------------------------

    inline void Material::setMatrix(const String &name, const Matrix4 &value)
    {
        setValue<Matrix4>(name, &value);
    }

    //--------------------------------------------------------------------------

    inline Matrix4 Material::getMatrix(const String &name) const
    {
        return getValue<Matrix4>(name);
    }
    
    //--------------------------------------------------------------------------

    inline bool Material::hasMatrix(const String &name) const
    {
        return hasValue<Matrix4>(name);
    }
    
    //--------------------------------------------------------------------------

    inline void Material::setMatrixArray(const String &name, const Matrix4Array &values)
    {
        setValue<Matrix4>(name, values.data());
    }

    //--------------------------------------------------------------------------

    inline Matrix4Array Material::getMatrixArray(const String &name) const
    {
        return getValues<Matrix4>(name);
    }
}
