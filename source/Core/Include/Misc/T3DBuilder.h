

#ifndef __T3D_BUILDER_H__
#define __T3D_BUILDER_H__


#include "T3DPrerequisites.h"
#include "T3DSingleton.h"
#include "T3DCommon.h"


namespace Tiny3D
{
	class RenderWindow;
	class Renderer;

	typedef std::list<Renderer*> RendererList;

	/**
	 * @brief The starting point class of the Tiny3D system.
	 * @remarks
	 *		The Tiny3D::Builder class represents a starting point for the application. From here,
	 *		the application can gain access to fundamentals of the system, listing the renderer 
	 *		available, management of saved configurations. An instance of Builder must be created 
	 *		before any other Tiny3D operations are called. Once an instance has been created, 
	 *		the same instance is accessible throughout the life of that object by using 
	 *		Builder::getInstance() (as a reference) or Builder::getInstancePtr() (as a pointer).
	 *		And then you should call Builder::run() for starting Tiny3D.
	 */
	class T3D_ENGINE_API Builder : public Singleton<Builder>
	{
	public:
		/** Default constructor */
		Builder(const TString &strConfigFileName = "Tiny3D.cfg");

		/** Destructor */
		virtual ~Builder();

		/**
		 * @brief Enumerate all available name of renderer in list.
		 * @param [out] rRendererList : list of all available renderer instances.
		 * @return void
		 */
		void enumerateRendererList(RendererList &rRendererList);

		/**
		 * @brief Set active renderer.
		 * @param [in] pRenderer : pointer to an available renderer instance.
		 * @return void
		 * @remarks 
		 *		The renderer must be one of all available renderer. You should call 
		 *		Builder::enumerateRendererList() to retrieve all available renderer in list.
		 */
		void setActiveRenderer(Renderer *pRenderer);

		/**
		 * @brief Create render window.
		 * @param [in] rkCreateParams : This param is key-value dictionary. See remarks for details.
		 * @return pointer to an instance for RenderWindow if create succeeded.
		 * @remarks 
		 *	all keys list below
		 *		# window_handle [key] : Handle of window. It can be used to associate handle
		 *			to existing window with RenderWindow.
		 *		# window_title [key] : Title of render window. 
		 *		# window_width [key] : Width of render window. 
		 *		# window_height [key] : Height of render window.
		 *		# color_depth [key] : Color depth of render window.
		 *		# full_screen [key] : 0 represents windowing, another value represents full screen. 
		 *			If the value of window_handle isn't NULL, this value will be ignored.
		 */
		RenderWindow *createRenderWindow(const NameValuePairList &rkCreateParams);

		/** 
		 * @brief Run Tiny3D
		 * @param void
		 * @return void
		 */
		bool run();
	};

	#define T3D_BUILDER			Builder::getInstance()
	#define T3D_BUILDER_PTR		Builder::getInstancePtr()
}


#endif	/*__T3D_BUILDER_H__*/

