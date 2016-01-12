

#ifndef __T3D_PREREQUISITES_H__
#define __T3D_PREREQUISITES_H__


#include "T3DMacro.h"


#if defined T3DCORE_EXPORT
    #define T3D_ENGINE_API        T3D_EXPORT_API
#else
    #define T3D_ENGINE_API        T3D_IMPORT_API
#endif


#include "T3DPlatform.h"
#include "T3DLog.h"
#include "Misc/T3DSmartPointer.h"


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

    class Transform;
    
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
    class FrustumBound;

    class Viewport;
    class RenderTarget;
    class RenderWindow;
    class Renderer;

    class HardwareBuffer;
    class HardwareVertexBuffer;
    class HardwareIndexBuffer;

    class VertexElement;
    class VertexDeclaration;

    class HardwareBufferManagerBase;
    class HardwareBufferManager;

    class Resource;
    class ResourceManager;
    class Dylib;
    class DylibManager;
    class Material;
    class MaterialManager;
    class Texture;
    class TextureManager;

    class Object;

    class SceneManager;
    class SGNode;
    class SGTransformNode;
    class SGVisual;
    class SGCamera;
    class SGMesh;
    class SGRenderable;
    class SGSphere;
    class SGBox;

    class VertexData;
    class IndexData;

    class RenderGroup;
    class RenderQueue;

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

    typedef SmartPtr<Object>                ObjectPtr;
    typedef SmartPtr<SGNode>                SGNodePtr;
    typedef SmartPtr<SGTransformNode>       SGTransformPtr;
    typedef SmartPtr<SGVisual>              SGVisualPtr;
    typedef SmartPtr<SGCamera>              SGCameraPtr;
    typedef SmartPtr<SGMesh>                SGMeshPtr;
    typedef SmartPtr<SGRenderable>          SGRenderablePtr;
    typedef SmartPtr<SGBox>                 SGBoxPtr;

    typedef SmartPtr<Viewport>              ViewportPtr;

    typedef SmartPtr<Renderer>              RendererPtr;
    typedef SmartPtr<RenderTarget>          RenderTargetPtr;
    typedef SmartPtr<RenderWindow>          RenderWindowPtr;

    typedef SmartPtr<HardwareBuffer>        HardwareBufferPtr;
    typedef SmartPtr<HardwareVertexBuffer>  HardwareVertexBufferPtr;
    typedef SmartPtr<HardwareIndexBuffer>   HardwareIndexBufferPtr;

    typedef SmartPtr<VertexElement>         VertexElementPtr;
    typedef SmartPtr<VertexDeclaration>     VertexDeclarationPtr;

    typedef SmartPtr<HardwareBufferManagerBase> HardwareBufferManagerBasePtr;

    typedef SmartPtr<VertexData>            VertexDataPtr;
    typedef SmartPtr<IndexData>             IndexDataPtr;

    typedef SmartPtr<Material>              MaterialPtr;
    typedef SmartPtr<Texture>               TexturePtr;

    typedef SmartPtr<RenderGroup>           RenderGroupPtr;
    typedef SmartPtr<RenderQueue>           RenderQueuePtr;

    typedef SmartPtr<Bound>                 BoundPtr;

    typedef std::list<SGNodePtr>            SGChildren;
    typedef SGChildren::iterator            SGChildrenItr;
    typedef SGChildren::const_iterator      SGChildrenConstItr;
}


#endif    /*__T3D_PREREQUISITES_H__*/
