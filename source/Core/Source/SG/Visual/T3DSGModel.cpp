

#include "SG/Visual/T3DSGModel.h"
#include "SG/Renderable/T3DSGMesh.h"
#include "Resource/T3DModel.h"
#include "Resource/T3DModelManager.h"


namespace Tiny3D
{
    SGModelPtr SGModel::create(const String &modelName, uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGModelPtr model = new SGModel(unID);
        if (model != nullptr && model->init(modelName))
        {
            model->release();
        }
        else
        {
            T3D_SAFE_RELEASE(model);
        }
        return model;
    }

    SGModel::SGModel(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGShape(unID)
    {

    }

    SGModel::~SGModel()
    {

    }

    bool SGModel::init(const String &modelName)
    {
        bool ret = false;
        mModel = T3D_MODEL_MGR.loadModel(modelName);

        if (mModel != nullptr)
        {
            ret = true;

            const Model::GeometryDataList &geometries = mModel->getGeometryDataList();
            size_t count = geometries.size();
            size_t i = 0;
            for (i = 0; i < count; ++i)
            {
                ObjectPtr geometry = geometries[i];
                SGMeshPtr mesh = SGMesh::create(geometry, mModel->isSharedVertex());
                addChild(mesh);
            }
        }

        return ret;
    }

    Node::Type SGModel::getNodeType() const
    {
        return E_NT_MODEL;
    }

    NodePtr SGModel::clone() const
    {
        SGModelPtr model = create(mModel->getName());

        if (model != nullptr)
        {
            SGShape::cloneProperties(model);
        }
        
        return model;
    }


}
