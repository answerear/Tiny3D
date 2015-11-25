

#ifndef __T3D_TYPE_H__
#define __T3D_TYPE_H__


#include <string>
#include <memory>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>

// #define __T3D_HIGH_PERCISION_FLOAT__


typedef signed char         char_t;
typedef unsigned char       uchar_t;
typedef signed short        short_t;
typedef unsigned short      ushort_t;
typedef signed int          int_t;
typedef unsigned int        uint_t;
typedef signed long         long_t;
typedef unsigned long       ulong_t;

typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;

typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;


typedef std::string         String;
typedef std::wstring        TWString;

typedef void*               THandle;

#if defined (__T3D_HIGH_PERCISION_FLOAT__)
typedef double  Real;
#else
typedef float   Real;
#endif


#if defined T3D_OS_WINDOWS
#define T3D_EXPORT_API      __declspec(dllexport)
#define T3D_IMPORT_API      __declspec(dllimport)
#else
#define T3D_EXPORT_API
#define T3D_IMPORT_API
#endif


#endif  /*__T3D_TYPE_H__*/