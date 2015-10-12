

#ifndef __T3D_ENGINE_MACRO_H__
#define __T3D_ENGINE_MACRO_H__


#include "T3DMacro.h"


#if defined T3DCORE_EXPORT
    #define T3D_ENGINE_API        T3D_EXPORT_API
#else
    #define T3D_ENGINE_API        T3D_IMPORT_API
#endif


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
    
    class Builder;

    class RenderWindow;
    class Renderer;
    class Resource;
    class ResourceManager;
    class Dylib;
    class DylibManager;
}


#endif    /*__T3D_ENGINE_MACRO_H__*/
