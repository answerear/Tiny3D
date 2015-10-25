

#ifndef __T3D_D3D9PREREQUISITES_H__
#define __T3D_D3D9PREREQUISITES_H__


#include "T3DMacro.h"
#include <windows.h>


#if defined T3DD3D9RENDERER_EXPORT
    #define T3D_D3D9RENDERER_API        T3D_EXPORT_API
#else
    #define T3D_D3D9RENDERER_API        T3D_IMPORT_API
#endif


#endif  /*__T3D_D3D9PREREQUISITES_H__*/

