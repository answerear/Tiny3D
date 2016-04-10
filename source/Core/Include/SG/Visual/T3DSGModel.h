
#ifndef __T3D_SG_MODEL_H__
#define __T3D_SG_MODEL_H__


#include "SG/Visual/T3DSGShape.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGModel : public SGShape
    {
    public:
        static SGModelPtr create(const String &modelName, uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGModel();

        virtual Type getNodeType() const override;

        virtual NodePtr clone() const override;

    protected:
        SGModel(uint32_t unID = E_NID_AUTOMATIC);

        virtual bool init(const String &modelName);

    protected:
        ModelPtr    mModel;
    };
}


#endif  /*__T3D_SG_MODEL_H__*/
