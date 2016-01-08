

#include "SG/Renderable/T3DSGCube.h"
#include "SG/T3DRenderQueue.h"
#include "DataStruct/T3DVertexList.h"
#include "DataStruct/T3DIndexList.h"


namespace Tiny3D
{
    const uint32_t SGCube::MAX_FACE_NUM = 6;
    const uint32_t SGCube::MAX_VERTICES_NUM = 8;
    const uint32_t SGCube::INDICES_PER_FACE = 6;

    SGNodePtr SGCube::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGCube *cube = new SGCube(unID);
        SGNodePtr ptr(cube);
        cube->release();
        return ptr;
    }

    SGCube::SGCube(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGRenderable(unID)
    {
        VertexList *vertices = new VertexList(VertexList::E_VF_POSITION, MAX_VERTICES_NUM, nullptr);
        mVertices = vertices;

        VertexP *vertex = new VertexP[MAX_VERTICES_NUM];
        int32_t i = 0;
        for (i = 0; i < MAX_VERTICES_NUM; ++i)
        {
            vertex[i].mPosition.x();
        }
        

//         IndexList *indices = new IndexList(IndexList::E_IT_16BITS, , nullptr);
    }

    SGCube::~SGCube()
    {

    }

    SGNode::Type SGCube::getNodeType() const
    {
        return E_NT_CUBE;
    }

    void SGCube::setMinX(Real x)
    {
        
    }

    SGNodePtr SGCube::clone() const
    {
        SGCube *node = new SGCube();
        SGNodePtr ptr(node);
        cloneProperties(node);
        node->release();
        return ptr;
    }

    void SGCube::cloneProperties(SGNode *node) const
    {

    }

    void SGCube::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        queue->addRenderable(RenderQueue::E_GRPID_SOLID, this);
    }
}
