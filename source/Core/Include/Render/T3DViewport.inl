

namespace Tiny3D
{
    inline SGCamera *Viewport::getCamera() const
    {
        return mCamera;
    }

    inline void Viewport::setCamera(SGCamera *camera)
    {
        mCamera = camera;
    }

    inline RenderTarget *Viewport::getRenderTarget() const
    {
        return mTarget;
    }

    inline Real Viewport::getLeft() const
    {
        return mLeft;
    }

    inline Real Viewport::getTop() const
    {
        return mTop;
    }

    inline Real Viewport::getWidth() const
    {
        return mWidth;
    }

    inline Real Viewport::getHeight() const
    {
        return mHeight;
    }

    inline int32_t Viewport::getActualLeft() const
    {
        return mActualLeft;
    }

    inline int32_t Viewport::getActualTop() const
    {
        return mActualTop;
    }

    inline int32_t Viewport::getActualWidth() const
    {
        return mActualWidth;
    }

    inline int32_t Viewport::getActualHeight() const
    {
        return mActualHeight;
    }

    inline void Viewport::setBackgroundColor(const Color4 &rkColor)
    {
        mBkgndColor = rkColor;
    }

    inline const Color4 &Viewport::getBackgroundColor() const
    {
        return mBkgndColor;
    }
}
