

#ifndef __T3D_APPLICATION_LISTENER_H__
#define __T3D_APPLICATION_LISTENER_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
	class ApplicationListener
	{
		T3D_DECLARE_INTERFACE(ApplicationListener);

	public:
		virtual bool applicationDidFinishLaunching() = 0;

		virtual void applicationDidEnterBackground() = 0;

		virtual void applicationWillEnterForeground() = 0;
	};
}


#endif	/*__T3D_APPLICATION_LISTENER_H__*/

