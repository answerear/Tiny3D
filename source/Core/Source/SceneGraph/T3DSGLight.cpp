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

#include "SceneGraph/T3DSGLight.h"
#include "Render/T3DRenderQueue.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    SGLightPtr SGLight::create(uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGLightPtr light = new SGLight(uID);
        light->release();
        return light;
    }

    SGLight::SGLight(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
        , mLightType(E_LT_POINT)
        , mAmbientColor(Color4::WHITE)
        , mDiffuseColor(Color4::WHITE)
        , mSpecularColor(Color4::WHITE)
    {

    }

    SGLight::~SGLight()
    {

    }

    Node::Type SGLight::getNodeType() const
    {
        return E_NT_LIGHT;
    }

    NodePtr SGLight::clone() const
    {
        SGLightPtr light = create();
        cloneProperties(light);
        return light;
    }

    void SGLight::updateTransform()
    {
    }

    void SGLight::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        queue->addRenderable(RenderQueue::E_GRPID_LIGHT, this);
    }

    MaterialPtr SGLight::getMaterial() const
    {
        return nullptr;
    }

    Renderer::PrimitiveType SGLight::getPrimitiveType() const
    {
        return Renderer::E_PT_TRIANGLE_LIST;
    }

    VertexDataPtr SGLight::getVertexData() const
    {
        return nullptr;
    }

    IndexDataPtr SGLight::getIndexData() const
    {
        return nullptr;
    }

    bool SGLight::isIndicesUsed() const
    {
        return false;
    }
}
