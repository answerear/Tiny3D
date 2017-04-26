

#ifndef __T3D_SG_INDICATOR_H__
#define __T3D_SG_INDICATOR_H__


#include "SG/Visual/T3DSGVisual.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGIndicator : public SGVisual
    {
    public:
        static SGIndicatorPtr create(Real fAxisX, Real fAxisY, Real fAxisZ, uint32_t uID = E_NID_AUTOMATIC);

        virtual ~SGIndicator();
        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

    protected:
        virtual bool init(Real fAxisX, Real fAxisY, Real fAxisZ);

        SGIndicator(uint32_t uID = E_NID_AUTOMATIC);

        virtual void addChild(const NodePtr &node) override;
    };
}

#endif  /*__T3D_SG_INDICATOR_H__*/

