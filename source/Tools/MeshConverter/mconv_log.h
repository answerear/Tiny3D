

#ifndef __MCONV_LOG_H__
#define __MCONV_LOG_H__



#include "mconv_prerequisites.h"


namespace mconv
{
#define MCONV_LOG_ERROR(fmt, ...)   \
    T3D_LOG_ERROR(fmt, ##__VA_ARGS__);  \
    printf(fmt, ##__VA_ARGS__); \
    printf("\n");

#define MCONV_LOG_WARNING(fmt, ...) \
    T3D_LOG_WARNING(fmt, ##__VA_ARGS__);    \
    printf(fmt, ##__VA_ARGS__); \
    printf("\n");

#define MCONV_LOG_INFO(fmt, ...)    \
    T3D_LOG_INFO(fmt, ##__VA_ARGS__);   \
    printf(fmt, ##__VA_ARGS__); \
    printf("\n");

#define MCONV_LOG_DEBUG(fmt, ...)   \
    T3D_LOG_DEBUG(fmt, ##__VA_ARGS__);  \
    printf(fmt, ##__VA_ARGS__); \
    printf("\n");

}


#endif  /*__MCONV_LOG_H__*/
