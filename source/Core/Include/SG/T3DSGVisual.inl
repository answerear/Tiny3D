

namespace Tiny3D
{
    inline bool SGVisual::isVisible() const
    {
        return mIsVisible;
    }

    inline void SGVisual::setBoundEnabled(bool enable)
    {
        mBoundEnabled = enable;
    }

    inline bool SGVisual::isBoundEnabled() const
    {
        return mBoundEnabled;
    }
}
