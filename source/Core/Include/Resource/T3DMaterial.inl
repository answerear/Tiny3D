/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
