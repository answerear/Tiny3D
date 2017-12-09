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
	 * @brief 32λ��������
	 * @remarks 32λ�������������ø�20λ��Ϊ�������֣���12λ������С������
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
		 * @brief Ĭ�Ϲ��캯��
		 */
        fix32();

		/**
		 * @brief �����ȸ����������Ĺ��캯��
		 */
        fix32(float32_t value);

		/**
		 * @brief ˫���ȸ����������Ĺ��캯��
		 */
		fix32(float64_t value);

		/**
		 * @brief ��32λ�������춨������32λ���䣺20.12
		 * @param [in] value : ������
	     */
		fix32(int32_t value);

		/**
		 * @brief ��64λ�������춨������64λ���䣺40.24
		 * @param [in] value : ������
		 */
		fix32(int64_t value);

		/**
		 * @brief ��32λ����ֱ�Ӹ�ֵ������������
		 */
		fix32(int32_t value, int32_t r);

		/**
		 * @brief �������캯��
		 */
		fix32(const fix32 &value);

		/**
		 * @brief ��������
		 */
		~fix32();

		/**
		 * @brief ��ȡ������������ֵ
		 */
		int32_t mantissa() const;

		/**
		 * @brief ��ȡ������������ֵ 
		 */
		int32_t &mantissa();

		fix32 operator +() const;
		fix32 operator -() const;

		// fix32��fix32֮������������
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

		// fix32��int32_t֮������������
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

		// fix32��float32_t֮������������
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

		// λ�����������
		friend fix32 operator <<(const fix32 &a, int32_t b);
		friend fix32 operator >>(const fix32 &a, int32_t b);

		fix32 &operator <<=(int32_t b);
		fix32 &operator >>=(int32_t b);

		// ����һЩ�򵥸����Ƚϴ�С�ӿ�
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

		// ת��64λ����
		operator int64_t() const;

	private:
		int32_t	m;
    };
}

#include "T3DFix32.inl"


#endif  /*__T3D_FIX32_H__*/