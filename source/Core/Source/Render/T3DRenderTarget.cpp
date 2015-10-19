

#include "Render/T3DRenderTarget.h"
#include "Misc/T3DViewport.h"


namespace Tiny3D
{
    RenderTarget::RenderTarget()
        : mWidth(0)
        , mHeight(0)
        , mColorDepth(0)
    {

    }

    RenderTarget::~RenderTarget()
    {

    }

    void RenderTarget::update()
    {

    }

    Viewport *RenderTarget::addViewport(SGCamera *camera, int32_t nZOrder, 
        Real left, Real top, Real width, Real height)
    {
        Viewport *viewport = nullptr;

        auto itr = mViewportList.find(nZOrder);

        if (itr == mViewportList.end())
        {
            viewport = new Viewport(camera, this, left, top, width, height, nZOrder);
            ViewportPtr v(viewport);
            mViewportList.insert(ViewportValue(nZOrder, v));
        }

        return viewport;
    }

    void RenderTarget::removeViewport(int32_t nZOrder)
    {
        auto itr = mViewportList.find(nZOrder);

        if (itr != mViewportList.end())
        {
            mViewportList.erase(itr);
        }
    }

    void RenderTarget::removeAllViewports()
    {
        mViewportList.clear();
    }
}
