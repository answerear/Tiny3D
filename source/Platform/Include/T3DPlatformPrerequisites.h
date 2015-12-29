

#ifndef __T3D_PLATFORM_MACRO_H__
#define __T3D_PLATFORM_MACRO_H__


#if defined T3DPLATFORM_EXPORT
    #define T3D_PLATFORM_API    T3D_EXPORT_API
#else
    #define T3D_PLATFORM_API    T3D_IMPORT_API
#endif


#endif  /*__T3D_PLATFORM_MACRO_H__*/