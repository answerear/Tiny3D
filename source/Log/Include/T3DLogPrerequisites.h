

#ifndef __T3D_LOGGER_MACRO_H__
#define __T3D_LOGGER_MACRO_H__


#if defined T3DLOG_EXPORT
#define T3D_LOG_API T3D_EXPORT_API
#else
#define T3D_LOG_API T3D_IMPORT_API
#endif


#include <T3DPlatform.h>



#endif  /*__T3D_LOGGER_MACRO_H__*/
