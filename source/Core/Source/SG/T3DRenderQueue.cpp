

#include "SG/T3DRenderQueue.h"
#include "Resource/T3DMaterial.h"
#include "SG/Renderable/T3DSGRenderable.h"
#include "SG/Renderable/T3DSGLight.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    RenderGroup::RenderGroup()
    {

    }

    RenderGroup::~RenderGroup()
    {

    }

    void RenderGroup::addRenderable(const MaterialPtr &material, const SGRenderablePtr &renderable)
    {
        RenderablesItr itr = mRenderables.find(material);

        if (itr != mRenderables.end())
        {
            /// 有相同材质的，放到相同材质渲染对象列表里
            RenderableList &renderables = itr->second;
            renderables.push_back(renderable);
        }
        else
        {
            /// 没有相同材质的，新加一个材质渲染对象列表
            RenderableList renderables(1, renderable);
            mRenderables.insert(RenderablesValue(material, renderables));
        }
    }

    void RenderGroup::clear()
    {
        mRenderables.clear();
    }

    void RenderGroup::render(uint32_t groupID, const RendererPtr &renderer)
    {
        Renderer::RenderMode renderMode;
        if (RenderQueue::E_GRPID_INDICATOR == groupID)
        {
            renderMode = renderer->getRenderMode();
            renderer->setRenderMode(Renderer::E_RM_WIREFRAME);
        }

        if (RenderQueue::E_GRPID_LIGHT != groupID)
        {
            RenderablesItr itr = mRenderables.begin();

            while (itr != mRenderables.end())
            {
                MaterialPtr material = itr->first;
                renderer->setMaterial(material);

                RenderableList &renderables = itr->second;

                RenderableListItr i = renderables.begin();

                while (i != renderables.end())
                {
                    SGRenderablePtr &renderable = *i;
                    const Matrix4 &m = renderable->getWorldMatrix();
                    renderer->setWorldTransform(m);

                    VertexDataPtr vertexData =renderable->getVertexData();
                    IndexDataPtr indexData = renderable->getIndexData();

                    Renderer::PrimitiveType priType = renderable->getPrimitiveType();
                    bool useIndices = renderable->isIndicesUsed();

                    size_t primitiveCount = calcPrimitiveCount(priType, 
                        useIndices ? indexData->getIndexBuffer()->getIndexCount() : 0, 
                        vertexData->getVertexBuffer(0)->getVertexCount(),
                        useIndices);

                    if (useIndices)
                    {
                        renderer->drawIndexList(priType, vertexData, indexData, 0, primitiveCount);
                    }
                    else
                    {
                        renderer->drawVertexList(priType, vertexData, 0, primitiveCount);
                    }

                    ++i;
                }

                ++itr;
            }
        }
        else
        {
            size_t index = 0;
            RenderablesItr itr = mRenderables.begin();

            while (itr != mRenderables.end())
            {
                RenderableList &renderables = itr->second;

                RenderableListItr i = renderables.begin();

                while (i != renderables.end())
                {
                    SGLightPtr light = smart_pointer_cast<SGLight>(*i);
                    renderer->addDynamicLight(index, light);
                    ++index;
                    ++i;
                }
                
                ++itr;
            }
        }

        if (RenderQueue::E_GRPID_INDICATOR == groupID)
        {
            renderer->setRenderMode(renderMode);
        }
    }

    size_t RenderGroup::calcPrimitiveCount(Renderer::PrimitiveType priType, size_t indexCount, size_t vertexCount, bool useIndex)
    {
        size_t primCount = 0;
        switch (priType)
        {
        case Renderer::E_PT_POINT_LIST:
            primCount = (useIndex ? indexCount : vertexCount);
            break;

        case Renderer::E_PT_LINE_LIST:
            primCount = (useIndex ? indexCount : vertexCount) / 2;
            break;

        case Renderer::E_PT_LINE_STRIP:
            primCount = (useIndex ? indexCount : vertexCount) - 1;
            break;

        case Renderer::E_PT_TRIANGLE_LIST:
            primCount = (useIndex ? indexCount : vertexCount) / 3;
            break;

        case Renderer::E_PT_TRIANGLE_STRIP:
            primCount = (useIndex ? indexCount : vertexCount) - 2;
            break;

        case Renderer::E_PT_TRIANGLE_FAN:
            primCount = (useIndex ? indexCount : vertexCount) - 2;
            break;
        }

        return primCount;
    }

    ////////////////////////////////////////////////////////////////////////////

    RenderQueuePtr RenderQueue::create()
    {
        RenderQueue *rq = new RenderQueue();
        RenderQueuePtr queue(rq);
        rq->release();
        return queue;
    }

    RenderQueue::RenderQueue()
    {

    }

    RenderQueue::~RenderQueue()
    {

    }

    void RenderQueue::addRenderable(GroupID groupID, const SGRenderablePtr &renderable)
    {
        RenderableGroupItr itr = mGroups.find(groupID);

        if (itr != mGroups.end())
        {
            /// 已经有这组渲染对象
            RenderGroupPtr &group = itr->second;
            group->addRenderable(renderable->getMaterial(), renderable);
        }
        else
        {
            /// 不存在这组渲染对象
            RenderGroup *group = new RenderGroup();
            RenderGroupPtr ptr(group);
            group->release();

            group->addRenderable(renderable->getMaterial(), renderable);
            mGroups.insert(RenderableGroupValue(groupID, group));
        }
    }

    void RenderQueue::clear()
    {
        mGroups.clear();
    }

    void RenderQueue::render(const RendererPtr &renderer)
    {
        RenderableGroupItr itr = mGroups.begin();

        while (itr != mGroups.end())
        {
            itr->second->render(itr->first, renderer);
            ++itr;
        }
    }
}
