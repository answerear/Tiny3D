

#ifndef __T3D_SINGLETON_H__
#define __T3D_SINGLETON_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatform.h"

#include <vector>
#define T3D_INIT_SINGLETON(t) template<> t *T3DSingleton<t>::m_pInstance = NULL


template <typename T>
class T3DSingleton
{
	T3D_DISABLE_COPY(T3DSingleton);

public:
	typedef typename T	value_type;
	typedef typename T*	pointer;
	typedef typename T&	reference;
	
	static pointer create()
	{
		if (nullptr == m_pInstance)
		{
			return new T();
		}
		
		return m_pInstance;
	}

protected:
	T3DSingleton()
	{
		m_pInstance = static_cast<pointer>(this);
	}

public:
	virtual ~T3DSingleton()
	{
		m_pInstance = NULL;
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


#endif	/*__T3D_SINGLETON_H__*/
