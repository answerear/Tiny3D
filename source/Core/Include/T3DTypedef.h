

#ifndef __T3D_TYPEDEF_H__
#define __T3D_TYPEDEF_H__


#include "Misc/T3DSmartPtr.h"


namespace Tiny3D
{
    typedef SmartPtr<Object, SmartBasePtr> ObjectPtr;

    T3D_DECLARE_SMART_PTR(Resource, Object);
    T3D_DECLARE_SMART_PTR(Dylib, Resource);
    T3D_DECLARE_SMART_PTR(Material, Resource);
    T3D_DECLARE_SMART_PTR(Texture, Resource);

    T3D_DECLARE_SMART_PTR(SGNode, Object);
    T3D_DECLARE_SMART_PTR(SGTransformNode, SGNode);
    T3D_DECLARE_SMART_PTR(SGVisual, SGNode);
    T3D_DECLARE_SMART_PTR(SGCamera, SGVisual);
    T3D_DECLARE_SMART_PTR(SGMesh, SGVisual);
    T3D_DECLARE_SMART_PTR(SGRenderable, SGNode);
    T3D_DECLARE_SMART_PTR(SGBox, SGRenderable);
    T3D_DECLARE_SMART_PTR(SGSphere, SGRenderable);

    T3D_DECLARE_SMART_PTR(Bound, Object);
    T3D_DECLARE_SMART_PTR(SphereBound, Bound);
    T3D_DECLARE_SMART_PTR(AabbBound, Bound);
    T3D_DECLARE_SMART_PTR(ObbBound, Bound);
    T3D_DECLARE_SMART_PTR(FrustumBound, Bound);

    T3D_DECLARE_SMART_PTR(Viewport, Object);
    T3D_DECLARE_SMART_PTR(RenderTarget, Object);
    T3D_DECLARE_SMART_PTR(RenderWindow, Object);
    T3D_DECLARE_SMART_PTR(Renderer, Object);

    T3D_DECLARE_SMART_PTR(HardwareBuffer, Object);
    T3D_DECLARE_SMART_PTR(HardwareVertexBuffer, Object);
    T3D_DECLARE_SMART_PTR(HardwareIndexBuffer, Object);

    T3D_DECLARE_SMART_PTR(VertexElement, Object);
    T3D_DECLARE_SMART_PTR(VertexDeclaration, Object);
    T3D_DECLARE_SMART_PTR(HardwareBufferManagerBase, Object);
    T3D_DECLARE_SMART_PTR(HardwareBufferManager, HardwareBufferManagerBase);

    T3D_DECLARE_SMART_PTR(VertexData, Object);
    T3D_DECLARE_SMART_PTR(IndexData, Object);

    T3D_DECLARE_SMART_PTR(RenderGroup, Object);
    T3D_DECLARE_SMART_PTR(RenderQueue, Object);

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

    typedef std::list<SGNodePtr>        SGChildren;
    typedef SGChildren::iterator        SGChildrenItr;
    typedef SGChildren::const_iterator  SGChildrenConstItr;

    typedef VariantMap                  RenderWindowCreateParamEx;

    typedef std::list<Renderer*>            RendererList;
    typedef RendererList::iterator          RendererListItr;
    typedef RendererList::const_iterator    RendererListConstItr;
}

#endif
