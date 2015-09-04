
#ifndef __T3D_MACRO_H__
#define __T3D_MACRO_H__


#include "T3DType.h"
#include <assert.h>


#if defined T3D_OS_WINDOWS		// Win32

#ifdef _DEBUG	// debug
	#define T3D_DEBUG
#else			// release
	#undef T3D_DEBUG
#endif

#pragma warning(disable:4996)
#pragma warning(disable:4251)

#define snprintf	_snprintf
#define vsnprintf	_vsnprintf

#elif defined T3D_OS_IOS || defined T3D_OS_MAC || defined T3D_OS_LINUX	// iOS or Mac OS x or Linux

#ifdef DEBUG	// debug
	#define T3D_DEBUG
#else			// release
	#undef T3D_DEBUG
#endif

#elif defined T3D_OS_ANDROID	// Android

#ifdef NDK_DEBUG	// debug
	#define T3D_DEBUG
#else				// release
	#undef T3D_DEBUG
#endif

#endif


#ifndef T3D_ASSERT
	#ifdef T3D_DEBUG
		#define T3D_ASSERT(x)	assert(x)
	#else
		#define T3D_ASSERT(x)
	#endif
#endif


#define MK_DWORD(h, l)		(((h<<16)&0xFFFF0000)|(0xFFFF&l))
#define LO_WORD(x)			(x&0xFFFF)
#define HI_WORD(x)			((x>>16)&0xFFFF)
#define MK_WORD(h, l)		(((h<<8)&0xFF00)|(0xFF&l))
#define LO_BYTE(x)			(x&0xFF)
#define HI_BYTE(x)			((x&>>8)&0xFF)


#define T3D_DECLARE_INTERFACE(T)	\
	public:		\
		virtual ~T()	{}

#define T3D_DISABLE_COPY(T)	\
	private:	\
		T(const T &);	\
		T &operator =(const T &);

#define T3D_SAFE_DELETE(p)	\
	if (p != nullptr)	\
	{	\
		delete p;	\
		p = nullptr;	\
	}

#define T3D_SAFE_DELETE_ARRAY(p)	\
	if (p != nullptr)	\
	{	\
		delete []p;	\
		p = nullptr;	\
	}


#endif	/*__T3D_MACRO_H__*/
