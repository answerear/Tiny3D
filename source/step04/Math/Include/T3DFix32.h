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

#ifndef __T3D_FIX32_H__
#define __T3D_FIX32_H__

#include "T3DMathPrerequisites.h"

namespace Tiny3D
{
	/**
	 * @brief 32位定点数类
	 * @remarks 32位定点数构成是用高20位作为整数部分，低12位用来做小数部分
	 */
    class T3D_MATH_API fix32
    {
    public:
		static const fix32 ZERO;		// fix32(0.0f)
		static const fix32 HALF;		// fix32(0.5f)
		static const fix32 ONE;			// fix32(1.0f)
		static const fix32 MINUS_ONE;	// fix32(-1.0f)
		static const fix32 INF;			// fix32(Infinity)
		static const fix32 MINUSINF;	// fix32(-Infinity)

		/**
		 * @brief 默认构造函数
		 */
        fix32();

		/**
		 * @brief 单精度浮点数参数的构造函数
		 */
        fix32(float32_t value);

		/**
		 * @brief 双精度浮点数参数的构造函数
		 */
		fix32(float64_t value);

		/**
		 * @brief 用32位整数构造定点数，32位分配：20.12
		 * @param [in] value : 整型数
	     */
		fix32(int32_t value);

		/**
		 * @brief 用64位整数构造定点数，64位分配：40.24
		 * @param [in] value : 整型数
		 */
		fix32(int64_t value);

		/**
		 * @brief 用32位整数直接赋值定点数来构造
		 */
		fix32(int32_t value, int32_t r);

		/**
		 * @brief 拷贝构造函数
		 */
		fix32(const fix32 &value);

		/**
		 * @brief 析构函数
		 */
		~fix32();

		/**
		 * @brief 获取定点数的整型值
		 */
		int32_t mantissa() const;

		/**
		 * @brief 获取定点数的整型值 
		 */
		int32_t &mantissa();

		fix32 operator +() const;
		fix32 operator -() const;

		// fix32和fix32之间的运算符重载
		fix32 &operator =(const fix32 &value);

		fix32 &operator +=(const fix32 &value);
		fix32 &operator -=(const fix32 &value);
		fix32 &operator *=(const fix32 &value);
		fix32 &operator /=(const fix32 &value);

		friend fix32 operator +(const fix32 &fx, const fix32 &gx);
		friend fix32 operator -(const fix32 &fx, const fix32 &gx);
		friend fix32 operator *(const fix32 &fx, const fix32 &gx);
		friend fix32 operator /(const fix32 &fx, const fix32 &gx);

		friend bool operator ==(const fix32 &fx, const fix32 &gx);
		friend bool operator !=(const fix32 &fx, const fix32 &gx);
		friend bool operator >=(const fix32 &fx, const fix32 &gx);
		friend bool operator <=(const fix32 &fx, const fix32 &gx);
		friend bool operator >(const fix32 &fx, const fix32 &gx);
		friend bool operator <(const fix32 &fx, const fix32 &gx);

		// fix32和int32_t之间的运算符重载
		fix32 &operator =(int32_t value);

		fix32 &operator +=(int32_t value);
		fix32 &operator -=(int32_t value);
		fix32 &operator *=(int32_t value);
		fix32 &operator /=(int32_t value);

		friend fix32 operator +(const fix32 &fx, int32_t value);
		friend fix32 operator +(int32_t value, const fix32 &fx);
		friend fix32 operator -(const fix32 &fx, int32_t value);
		friend fix32 operator -(int32_t value, const fix32 &fx);
		friend fix32 operator *(const fix32 &fx, int32_t value);
		friend fix32 operator *(int32_t value, const fix32 &fx);
		friend fix32 operator /(const fix32 &fx, int32_t value);
		friend fix32 operator /(int32_t value, const fix32 &fx);

		friend bool operator ==(const fix32 &fx, int32_t value);
		friend bool operator ==(int32_t value, const fix32 &fx);
		friend bool operator !=(const fix32 &fx, int32_t value);
		friend bool operator !=(int32_t value, const fix32 &fx);
		friend bool operator >=(const fix32 &fx, int32_t value);
		friend bool operator >=(int32_t value, const fix32 &fx);
		friend bool operator <=(const fix32 &fx, int32_t value);
		friend bool operator <=(int32_t value, const fix32 &fx);
		friend bool operator >(const fix32 &fx, int32_t value);
		friend bool operator >(int32_t value, const fix32 &fx);
		friend bool operator <(const fix32 &fx, int32_t value);
		friend bool operator <(int32_t value, const fix32 &fx);

		operator int32_t() const;

		// fix32和float32_t之间的运算符重载
		fix32 &operator =(float32_t value);

		fix32 &operator +=(float32_t value);
		fix32 &operator -=(float32_t value);
		fix32 &operator *=(float32_t value);
		fix32 &operator /=(float32_t value);

		friend fix32 operator +(const fix32 &fx, float32_t value);
		friend fix32 operator +(float32_t value, const fix32 &fx);
		friend fix32 operator -(const fix32 &fx, float32_t value);
		friend fix32 operator -(float32_t value, const fix32 &fx);
		friend fix32 operator *(const fix32 &fx, float32_t value);
		friend fix32 operator *(float32_t value, const fix32 &fx);
		friend fix32 operator /(const fix32 &fx, float32_t value);
		friend fix32 operator /(float32_t value, const fix32 &fx);

		friend bool operator ==(const fix32 &fx, float32_t value);
		friend bool operator ==(float32_t value, const fix32 &fx);
		friend bool operator !=(const fix32 &fx, float32_t value);
		friend bool operator !=(float32_t value, const fix32 &fx);
		friend bool operator >=(const fix32 &fx, float32_t value);
		friend bool operator >=(float32_t value, const fix32 &fx);
		friend bool operator <=(const fix32 &fx, float32_t value);
		friend bool operator <=(float32_t value, const fix32 &fx);
		friend bool operator >(const fix32 &fx, float32_t value);
		friend bool operator >(float32_t value, const fix32 &fx);
		friend bool operator <(const fix32 &fx, float32_t value);
		friend bool operator <(float32_t value, const fix32 &fx);

		operator float32_t() const;

		// 位移运算符重载
		friend fix32 operator <<(const fix32 &a, int32_t b);
		friend fix32 operator >>(const fix32 &a, int32_t b);

		fix32 &operator <<=(int32_t b);
		fix32 &operator >>=(int32_t b);

		// 其他一些简单辅助比较大小接口
		bool eq_0() const;
		bool ne_0() const;
		bool gt_0() const;
		bool ge_0() const;
		bool lt_0() const;
		bool le_0() const;
		bool eq_1() const;

		bool is_INF() const;
		bool is_MINUSINF() const;

		bool is_integer() const;

		// 转换64位整型
		operator int64_t() const;

	private:
		int32_t	m;
    };
}

#include "T3DFix32.inl"


#endif  /*__T3D_FIX32_H__*/