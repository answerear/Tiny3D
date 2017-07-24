

#include "Render/T3DRenderTarget.h"
#include "Render/T3DViewport.h"


namespace Tiny3D
{
    RenderTargetPtr RenderTarget::create()
    {
        RenderTarget *rt = new RenderTarget();
        RenderTargetPtr ptr(rt);
        rt->release();
        return ptr;
    }

    RenderTarget::RenderTarget()
        : mWidth(0)
        , mHeight(0)
        , mColorDepth(0)
        , mIsActive(true)
    {

    }

    RenderTarget::~RenderTarget()
    {

    }

    void RenderTarget::update()
    {
        auto itr = mViewportList.begin();

        while (itr != mViewportList.end())
        {
            itr->second->update();
            ++itr;
        }
    }

    ViewportPtr RenderTarget::addViewport(SGCamera *camera, int32_t nZOrder, 
        Real left, Real top, Real width, Real height)
    {
        ViewportPtr viewport;

        auto itr = mViewportList.find(nZOrder);

        if (itr == mViewportList.end())
        {
            viewport = Viewport::create(camera, this, left, top, width, height, nZOrder);
            mViewportList.insert(ViewportValue(nZOrder, viewport));
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
