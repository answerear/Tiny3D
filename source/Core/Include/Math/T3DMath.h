

#ifndef __T3D_MATH_H__
#define __T3D_MATH_H__


#include "T3DEngineMacro.h"
#include <math.h>


namespace Tiny3D
{
	class Degree;
	
	////////////////////////////////////////////////////////////////////////////////////////////////

	class T3D_ENGINE_API Radian
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

	#include "T3DRadian.inl"

	////////////////////////////////////////////////////////////////////////////////////////////////

	class T3D_ENGINE_API Degree
	{
	public:
		/// Create a degree with float value.
		explicit Degree(Real fDegree = 0.0);
		/// Create a degree with radian.
		Degree(const Radian &radian);
		/// Copy constructor.
		Degree(const Degree &other);

		/// Assignment
		Degree &operator =(Real fDegree);
		Degree &operator =(const Degree &other);
		Degree &operator =(const Radian &radian);

		const Degree &operator +() const;
		Degree operator -() const;

		/// Comparison (equal to).
		bool operator ==(const Degree &other) const;
		/// Comparison (not equal to).
		bool operator !=(const Degree &other) const;
		/// 
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

		Real valueDegrees() const;
		Real valueRadians() const;

	private:
		Real	m_fDegree;
	};

	#include "T3DDegree.inl"

	////////////////////////////////////////////////////////////////////////////////////////////////

	class T3D_ENGINE_API Math
	{
	public:
		/// Square.
		static Real Sqr(Real fValue);
		/// Square root.
		static Real Sqrt(Real fValue);

		/// Sine in degree.
		static Real Sin(const Degree &degrees);
		/// Sine in radian.
		static Real Sin(const Radian &radians);
		/// Cosine in degree.
		static Real Cos(const Degree &degrees);
		/// Cosine in radian.
		static Real Cos(const Radian &radians);
		/// Tangent in degree.
		static Real Tan(const Degree &degrees);
		/// Tangent in radians.
		static Real Tan(const Radian &radians);

		/// Arc-sine.
		static Radian ASin(Real fValue);
		/// Arc-cosine.
		static Radian ACos(Real fValue);
		/// Arc-tangent.
		static Radian ATan(Real fValue);
		/// Arc-tangent of y/x
		static Radian ATan2(Real fY, Real fX);

		static Real DegreesToRadians(Real degrees);
		static Real RadiansToDegrees(Real radians);

	public:
		static const Real POS_INFINITY;
		static const Real NEG_INFINITY;
		static const Real PI;
		static const Real TWO_PI;
		static const Real HALF_PI;
		static const Real DEGREES_TO_RADIANS;
		static const Real RADIANS_TO_DEGREES;
	};
}


#include "T3DMath.inl"


#endif	/*__T3D_MATH_H__*/
