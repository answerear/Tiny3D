

#ifndef __T3D_ENGINE_MACRO_H__
#define __T3D_ENGINE_MACRO_H__


#include "T3DMacro.h"

#if defined T3DCORE_EXPORT
	#define T3D_ENGINE_API		T3D_EXPORT_API
#else
	#define T3D_ENGINE_API		T3D_IMPORT_API
#endif


#endif	/*__T3D_ENGINE_MACRO_H__*/
