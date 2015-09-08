

#ifndef __T3D_DEGREE_H__
#define __T3D_DEGREE_H__


#include "T3DEngineMacro.h"
#include "T3DMath.h"


namespace Tiny3D
{
	class Radian;

	class Degree
	{
	public:
		/// Create a degree with degree.
		Degree(Real fDegree = 0.0);
		/// Create a degree with radian.
		Degree(const Radian &radian);
		/// Copy constructor.
		Degree(const Degree &other);

		/// Assignment
		Degree &operator =(Real fDegree);
		Degree &operator =(const Degree &other);
		Degree &operator =(const Radian &radian);

		/// Comparison (equal to).
		bool operator ==(const Degree &other) const;
		/// Comparison (not equal to).
		bool operator !=(const Degree &other) const;
		bool operator <(const Degree &other) const;
		bool operator <=(const Degree &other) const;
		bool operator >(const Degree &other) const;
		bool operator >=(const Degree &other) const;

		Degree operator +(const Degree &other) const;
		Degree operator -(const Degree &other) const;

		Degree operator +(const Radian &radian) const;
		Degree operator -(const Radian &radian) const;

		Degree &operator +=(const Degree &other);
		Degree &operator -=(const Degree &other);

		Degree &operator +=(const Radian &radian);
		Degree &operator -=(const Radian &radian);

		Degree operator *(Real fScalar) const;
		Degree operator /(Real fScalar) const;

		Degree &operator *=(Real fScalar);
		Degree &operator /=(Real fScalar);

		Real valueDegree() const;
		Real valueRadian() const;

	private:
		Real	m_fDegree;
	};

	#include "T3DDegree.inl"
}


#endif	/*__T3D_DEGREE_H__*/