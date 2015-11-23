

#ifndef __T3D_SG_VISUAL_H__
#define __T3D_SG_VISUAL_H__


#include "SG/T3DSGNode.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGVisual : public SGNode
    {
    protected:
        SGVisual(uint32_t unID = E_NID_AUTOMATIC);

    public:
        virtual ~SGVisual();

        virtual void setVisible(bool visible);
        bool isVisible() const;

        virtual Bound *getBound() = 0;

        void setBoundEnabled(bool enable);
        bool isBoundEnabled() const;

        virtual void cloneProperties(SGNode *node);

    protected:
        virtual void update();
        virtual void updateBound() = 0;

    private:
        bool    mIsVisible;
        bool    mBoundEnabled;
    };
}


#include "T3DSGVisual.inl"


#endif  /*__T3D_SG_VISUAL_H__*/
