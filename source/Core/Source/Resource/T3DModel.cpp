

#include "Resource/T3DModel.h"
#include "DataStruct/T3DMeshData.h"


namespace Tiny3D
{
    ////////////////////////////////////////////////////////////////////////////

    ModelPtr Model::create(const String &name)
    {
        ModelPtr model = new Model(name);
        model->release();
        return model;
    }

    Model::Model(const String &name)
        : Resource(name)
    {

    }

    Model::~Model()
    {

    }

    Resource::Type Model::getType() const
    {
        return E_TYPE_MODEL;
    }

    bool Model::load()
    {
        MeshDataPtr meshdata = smart_pointer_cast<MeshData>(mMeshData);

        return true;
    }

    void Model::unload()
    {
        Resource::unload();
    }

    ResourcePtr Model::clone() const
    {
        ModelPtr model = create(mName);
        return model;
    }
}
