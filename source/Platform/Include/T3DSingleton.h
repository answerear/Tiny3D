

#ifndef __T3D_SINGLETON_H__
#define __T3D_SINGLETON_H__


#include "T3DType.h"
#include "T3DMacro.h"

#include <vector>


#if defined (T3D_OS_WINDOWS)
	#pragma warning(disable:4661)
#endif


#define T3D_INIT_SINGLETON(T) Tiny3D::Singleton<T>::pointer Tiny3D::Singleton<T>::m_pInstance = nullptr


namespace Tiny3D
{
	template <typename T>
	class Singleton
	{
		T3D_DISABLE_COPY(Singleton);

	public:
		typedef typename T	value_type;
		typedef typename T*	pointer;
		typedef typename T&	reference;

		Singleton()
		{
			m_pInstance = static_cast<pointer>(this);
		}

		virtual ~Singleton()
		{
			m_pInstance = nullptr;
		}

		static reference getInstance()
		{
			return *m_pInstance;
		}

		static pointer getInstancePtr()
		{
			return m_pInstance;
		}

	protected:
		static pointer m_pInstance;
	};
}


#endif	/*__T3D_SINGLETON_H__*/
