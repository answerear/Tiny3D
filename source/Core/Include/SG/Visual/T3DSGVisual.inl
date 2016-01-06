

namespace Tiny3D
{
    inline bool SGVisual::isVisible() const
    {
        return mIsVisible;
    }

    inline void SGVisual::setBoundEnabled(bool enable)
    {
        if (mBound != nullptr)
        {
            mBound->setEnable(enable);
        }
    }

    inline bool SGVisual::isBoundEnabled() const
    {
        bool isEnable = false;
        if (mBound != nullptr)
        {
            isEnable = mBound->isEnable();
        }
        return isEnable;
    }

    inline bool SGVisual::inFrustum() const
    {
        return mIsInFrustum;
    }

    inline Bound *SGVisual::getBound()
    {
        return mBound;
    }

    inline const Transform &SGVisual::getWorldTransform() const
    {
        return mWorldTransform;
    }
}
