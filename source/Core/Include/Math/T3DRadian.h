

#ifndef __T3D_RADIAN_H__
#define __T3D_RADIAN_H__


#include "T3DEngineMacro.h"
#include "T3DMath.h"


namespace Tiny3D
{
	class Degree;

	class Radian
	{
	public:
		/// Create a radian object with float value.
		explicit Radian(Real fRadian = 0.0);
		/// Create a degree object with float value.
		Radian(const Degree &degree);
		/// Copy constructor.
		Radian(const Radian &other);

		/// Assignment
		Radian &operator =(Real fRadian);
		Radian &operator =(const Radian &other);
		Radian &operator =(const Degree &degree);

		const Radian &operator +() const;
		Radian operator -() const;

		/// Comparison (equal to).
		bool operator ==(const Radian &other) const;
		/// Comparison (not equal to).
		bool operator !=(const Radian &other) const;
		/// 
		bool operator <(const Radian &other) const;
		bool operator <=(const Radian &other) const;
		bool operator >(const Radian &other) const;
		bool operator >=(const Radian &other) const;

		Radian operator +(const Radian &other) const;
		Radian operator -(const Radian &other) const;

		Radian operator +(const Degree &degree) const;
		Radian operator -(const Degree &degree) const;

		Radian &operator +=(const Radian &other);
		Radian &operator -=(const Radian &other);

		Radian &operator +=(const Degree &degree);
		Radian &operator -=(const Degree &degree);

		Radian operator *(Real fScalar) const;
		Radian operator /(Real fScalar) const;

		Radian &operator *=(Real fScalar);
		Radian &operator /=(Real fScalar);

		Real valueDegrees() const;
		Real valueRadians() const;

	private:
		Real	m_fRadian;
	};
}


#include "T3DRadian.inl"


#endif	/*__T3D_RADIAN_H__*/
