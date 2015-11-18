

#ifndef __T3D_D3D9_RENDERER_H__
#define __T3D_D3D9_RENDERER_H__


#include "T3DD3D9Prerequisites.h"
#include "Render/T3DRenderer.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>


namespace Tiny3D
{
    class D3D9Renderer : public Renderer
    {
    public:
        D3D9Renderer(HINSTANCE hInstance);
        virtual ~D3D9Renderer();

        virtual String getName() const;

        virtual RenderWindow *createRenderWindow(
            const RenderWindowCreateParam &rkCreateParam, 
            const RenderWindowCreateParamEx &rkCreateParamEx);

        virtual void startRendering();

        virtual bool initialize();
        virtual void uninitialize();

        virtual bool beginRender();
        virtual bool endRender();

        virtual bool queryCapability(Capability cap);
        virtual void enableCapability(Capability cap, bool enabled = true);

        virtual void setTransform(TransformState state, const Matrix4 &mat);

        virtual const Matrix4 &getTransform(TransformState state) const;

        virtual void setMaterial();

        virtual void drawVertexList(PrimitiveType primitiveType, 
            const VertexList &vertices, uint32_t startIdx, 
            uint32_t primitiveCount);

        virtual void drawIndexList(PrimitiveType primitiveType, 
            const VertexList &vertices, const IndexList *indicies, 
            uint32_t startIdx, uint32_t pritimitiveCount);

        LPDIRECT3D9 getD3D()  { return mD3D; }

    protected:
        HINSTANCE   mInstance;

        LPDIRECT3D9         mD3D;
    };
}


#endif  /*__T3D_D3D9_RENDERER_H__*/
