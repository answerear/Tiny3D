/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

namespace Tiny3D
{
    inline fix32::fix32() : m(0)
    {

    }

    inline fix32::fix32(float32_t value)
    {
		T3D_ASSERT(value <= 524287.0f &&value >= -524288.0f);
		m = (int32_t)(value * 4096);
    }

    inline fix32::fix32(float64_t value)
    {
		T3D_ASSERT(value <= 524287.0 &&value >= -524288.0);
		m = (int32_t)(value * 4096);
    }

	inline fix32::fix32(int32_t value)
	{
		T3D_ASSERT(value <= 524287 && value >= -524288);
		m = (value << 12);
	}

	inline fix32::fix32(int64_t value)
	{
		T3D_ASSERT(value <= 524287 && value >= -524288);
		m = (int32_t)(value << 12);
	}

	inline fix32::fix32(int32_t value, int32_t r)
	{
		m = value;
	}

	inline fix32::fix32(const fix32 &value)
	{
		m = value.m;
	}

    inline fix32::~fix32()
    {

    }

	inline int32_t fix32::mantissa() const
	{
		return m;
	}

	inline int32_t &fix32::mantissa()
	{
		return m;
	}

	inline fix32 &fix32::operator =(const fix32 &value)
	{
		m = value.m;
		return *this;
	}
}
