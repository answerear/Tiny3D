

#include "SG/Renderable/T3DSGBox.h"
#include "SG/T3DRenderQueue.h"


namespace Tiny3D
{
    const uint32_t SGBox::MAX_FACE_NUM = 6;
    const uint32_t SGBox::MAX_VERTICES_NUM = 8;
    const uint32_t SGBox::INDICES_PER_FACE = 6;

    SGBoxPtr SGBox::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGBox *box = new SGBox(unID);
        SGBoxPtr ptr(box);
        box->release();
        return ptr;
    }

    SGBox::SGBox(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGRenderable(unID)
    {
    }

    SGBox::~SGBox()
    {

    }

    SGNode::Type SGBox::getNodeType() const
    {
        return E_NT_BOX;
    }

    void SGBox::setMinX(Real x)
    {
        
    }

    SGNodePtr SGBox::clone() const
    {
        SGBox *node = new SGBox();
        SGNodePtr ptr(node);
        cloneProperties(node);
        node->release();
        return ptr;
    }

    void SGBox::cloneProperties(SGNode *node) const
    {

    }

    void SGBox::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        queue->addRenderable(RenderQueue::E_GRPID_SOLID, this);
    }
}
