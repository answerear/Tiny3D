

#ifndef __T3D_TYPEDEF_H__
#define __T3D_TYPEDEF_H__


#include "Misc/T3DSmartPtr.h"


namespace Tiny3D
{
    T3D_DECLARE_SMART_PTR(Object);

    T3D_DECLARE_SMART_PTR(Resource);
    T3D_DECLARE_SMART_PTR(Dylib);
    T3D_DECLARE_SMART_PTR(Material);
    T3D_DECLARE_SMART_PTR(Texture);
    T3D_DECLARE_SMART_PTR(Archive);
    T3D_DECLARE_SMART_PTR(FileSystemArchive);
    T3D_DECLARE_SMART_PTR(ZipArchive);

    T3D_DECLARE_SMART_PTR(SGNode);
    T3D_DECLARE_SMART_PTR(SGTransformNode);
    T3D_DECLARE_SMART_PTR(SGVisual);
    T3D_DECLARE_SMART_PTR(SGCamera);
    T3D_DECLARE_SMART_PTR(SGMesh);
    T3D_DECLARE_SMART_PTR(SGLight);
    T3D_DECLARE_SMART_PTR(SGRenderable);
    T3D_DECLARE_SMART_PTR(SGBox);
    T3D_DECLARE_SMART_PTR(SGSphere);

    T3D_DECLARE_SMART_PTR(Bound);
    T3D_DECLARE_SMART_PTR(SphereBound);
    T3D_DECLARE_SMART_PTR(AabbBound);
    T3D_DECLARE_SMART_PTR(ObbBound);
    T3D_DECLARE_SMART_PTR(FrustumBound);

    T3D_DECLARE_SMART_PTR(Viewport);
    T3D_DECLARE_SMART_PTR(RenderTarget);
    T3D_DECLARE_SMART_PTR(RenderWindow);
    T3D_DECLARE_SMART_PTR(Renderer);

    T3D_DECLARE_SMART_PTR(HardwareBuffer);
    T3D_DECLARE_SMART_PTR(HardwareVertexBuffer);
    T3D_DECLARE_SMART_PTR(HardwareIndexBuffer);

    T3D_DECLARE_SMART_PTR(VertexElement);
    T3D_DECLARE_SMART_PTR(VertexDeclaration);
    T3D_DECLARE_SMART_PTR(HardwareBufferManagerBase);
    T3D_DECLARE_SMART_PTR(HardwareBufferManager);

    T3D_DECLARE_SMART_PTR(VertexData);
    T3D_DECLARE_SMART_PTR(IndexData);

    T3D_DECLARE_SMART_PTR(RenderGroup);
    T3D_DECLARE_SMART_PTR(RenderQueue);


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
