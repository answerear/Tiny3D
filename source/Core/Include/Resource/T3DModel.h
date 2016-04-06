

#ifndef __T3D_MODEL_H__
#define __T3D_MODEL_H__


#include "Resource/T3DResource.h"
#include "Math/T3DMatrix4.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Model : public Resource
    {
    public:
        static ModelPtr create(const String &name);

        virtual ~Model();

        virtual Type getType() const override;

    protected:
        Model(const String &name);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;
    };
}

#endif  /*__T3D_MODEL_H__*/
