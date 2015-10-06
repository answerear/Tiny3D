
#ifndef __T3D_RENDER_WINDOW_H__
#define __T3D_RENDER_WINDOW_H__


#include "T3DPrerequisites.h"
#include "T3DCommon.h"


namespace Tiny3D
{
	class T3D_ENGINE_API RenderWindow
	{
	public:
		RenderWindow();
		virtual ~RenderWindow();

		virtual void create(const NameValuePairList &rkCreateParams) = 0;
		virtual void destroy() = 0;
	};
}


#endif	/*__T3D_RENDER_WINDOW_H__*/
