/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "Render/T3DRenderQueue.h"
#include "Render/T3DRenderer.h"
#include "Resource/T3DMaterial.h"
#include "SceneGraph/T3DSGRenderable.h"
#include "SceneGraph/T3DSGCamera.h"
#include "SceneGraph/T3DSGLight.h"


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
        else if (RenderQueue::E_GRPID_OVERLAY == groupID)
        {
            renderer->getViewport()->getCamera()->setProjectionType(SGCamera::E_PT_ORTHOGRAPHIC);
            renderer->getViewport()->getCamera()->getViewMatrix();
            renderer->getViewport()->getCamera()->getProjectionMatrix();
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
            itr->second->clear();
            ++itr;
        }
    }
}
