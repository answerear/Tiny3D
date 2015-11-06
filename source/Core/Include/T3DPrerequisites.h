

#ifndef __T3D_PREREQUISITES_H__
#define __T3D_PREREQUISITES_H__


#include "T3DMacro.h"


#if defined T3DCORE_EXPORT
    #define T3D_ENGINE_API        T3D_EXPORT_API
#else
    #define T3D_ENGINE_API        T3D_IMPORT_API
#endif


#include "T3DPlatform.h"
#include "T3DLogger.h"


namespace Tiny3D
{
    /// Platform
    class TextCodec;
    class Console;
    class DeviceInfo;
    class DataStream;
    class Dir;
    class FileDataStream;
    class MemoryDataStream;
    class DateTime;
    class RunLoop;
    class RunLoopObserver;
    class Timer;
    class TimerObserver;
    class System;

    /// Log
    class Logger;

    /// Core
    class ApplicationListener;
    class RenderTargetListener;
    class FrameListener;

    class Degree;
    class Radian;
    class Math;
    class Matrix2;
    class Matrix3;
    class Matrix4;
    class Quaternion;
    class Vector2;
    class Vector3;
    class Vector4;
    
    class Entrance;
    class Plugin;

    class Color4;

    class Plane;
    class Sphere;
    class Frustum;
    class Obb;
    class Aabb;

    class Bound;
    class SphereBound;
    class AabbBound;
    class ObbBound;
    class Frustum;

    class Viewport;
    class RenderTarget;
    class RenderWindow;
    class Renderer;

    class Resource;
    class ResourceManager;
    class Dylib;
    class DylibManager;

    class SGCamera;

    class Variant;

    typedef std::vector<Variant>            VariantArray;
    typedef VariantArray::iterator          VariantArrayItr;
    typedef VariantArray::const_iterator    VariantArrayConstItr;

    typedef std::list<Variant>              VariantList;
    typedef VariantList::iterator           VariantListItr;
    typedef VariantList::const_iterator     VariantListConstItr;

    typedef std::map<Variant, Variant>      VariantMap;
    typedef VariantMap::iterator            VariantMapItr;
    typedef VariantMap::const_iterator      VariantMapConstItr;

    typedef std::pair<Variant, Variant>     VariantMapValue;

    typedef VariantMap Settings;
}


#endif    /*__T3D_PREREQUISITES_H__*/
