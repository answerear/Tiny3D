

namespace Tiny3D
{
    inline const TString &RenderTarget::getName() const
    {
        return mName;
    }

    inline void RenderTarget::getMetrics(int32_t &nWidth, int32_t &nHeight, 
        int32_t &nColorDepth) const
    {
        nWidth = mWidth;
        nHeight = mHeight;
        nColorDepth = mColorDepth;
    }

    inline int32_t RenderTarget::getWidth() const
    {
        return mWidth;
    }

    inline int32_t RenderTarget::getHeight() const
    {
        return mHeight;
    }

    inline int32_t RenderTarget::getColorDepth() const
    {
        return mColorDepth;
    }

    inline uint32_t RenderTarget::getNumViewports() const
    {
        return (uint32_t)mViewportList.size();
    }
}
