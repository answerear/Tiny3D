

#ifndef __T3D_SINGLETON_H__
#define __T3D_SINGLETON_H__


#include "T3DType.h"
#include "T3DMacro.h"

#include <vector>


#define T3D_INIT_SINGLETON(T) T3DSingleton<T>::pointer T3DSingleton<T>::m_pInstance = nullptr


template <typename T>
class T3DSingleton
{
	T3D_DISABLE_COPY(T3DSingleton);

public:
	typedef typename T	value_type;
	typedef typename T*	pointer;
	typedef typename T&	reference;

	T3DSingleton()
	{
		m_pInstance = static_cast<pointer>(this);
	}

	virtual ~T3DSingleton()
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


#endif	/*__T3D_SINGLETON_H__*/
