

#include "Misc/T3DViewport.h"
#include "Render/T3DRenderTarget.h"
#include "SG/Visual/T3DSGCamera.h"


namespace Tiny3D
{
    Viewport::Viewport(SGCamera *camera, RenderTarget *target, 
        Real left, Real top, Real width, Real height, int32_t nZOrder)
        : mCamera(camera)
        , mTarget(target)
        , mLeft(left)
        , mTop(top)
        , mWidth(width)
        , mHeight(height)
        , mActualLeft(0)
        , mActualTop(0)
        , mActualWidth(0)
        , mActualHeight(0)
        , mBkgndColor(0, 0, 0)
    {
        updateDimemsions();
    }

    Viewport::~Viewport()
    {

    }

    void Viewport::update()
    {
        mCamera->updateTransform();
        mCamera->renderScene(this);
    }

    void Viewport::setDimensions(Real left, Real top, Real width, Real height)
    {
        mLeft = left;
        mTop = top;
        mWidth = width;
        mHeight = height;

        updateDimemsions();
    }

    void Viewport::updateDimemsions()
    {
        int32_t nWidth = mTarget->getWidth();
        int32_t nHeight = mTarget->getHeight();

        mActualLeft = int32_t(nWidth * mLeft);
        mActualTop = int32_t(nHeight * mTop);
        mActualWidth = int32_t(nWidth * mWidth);
        mActualHeight = int32_t(nHeight * mHeight);
    }
}
