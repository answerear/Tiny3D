/***************************************************************************************************
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
 **************************************************************************************************/

#include "SceneGraph/T3DSGText2D.h"
#include "SceneGraph/T3DSGCamera.h"
#include "SceneGraph/T3DSGTransform2D.h"

#include "Resource/T3DFont.h"
#include "Resource/T3DFontManager.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DTexture.h"

#include "Bound/T3DFrustumBound.h"

#include "Render/T3DViewport.h"
#include "Render/T3DRenderQueue.h"
#include "Render/T3DVertexData.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareIndexBuffer.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    SGText2DPtr SGText2D::create(const String &text /* = "" */, size_t fontSize /* = 10 */, 
        const Color4 &textColor /* = Color4::WHITE */, const String &fontName /* = "" */, 
        uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGText2DPtr ret = new SGText2D(uID);

        if (ret != nullptr && ret->init(text, fontSize, textColor, fontName))
        {
            ret->release();
        }
        else
        {
            T3D_SAFE_DELETE(ret);
        }

        return ret;
    }

    SGText2D::SGText2D(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
        , mAnchorPos(0.5f, 0.5f)
    {

    }

    SGText2D::~SGText2D()
    {

    }

    bool SGText2D::init(const String &text, size_t fontSize, const Color4 &texColor, const String &fontName)
    {
        bool ret = false;

        do 
        {
            mFont = T3D_FONT_MGR.loadFont(fontSize, fontName);
            if (mFont == nullptr)
            {
                T3D_LOG_ERROR("Load font %s failed !", fontName.c_str());
                break;
            }

            if (!text.empty())
            {
                mText = text;

                if (!updateVertices())
                {
                    break;
                }
            }

            ret = true;
        } while (0);
        
        return ret;
    }

    Node::Type SGText2D::getNodeType() const
    {
        return E_NT_TEXT2D;
    }

    NodePtr SGText2D::clone() const
    {
        return SGText2D::create(mText);
    }

    void SGText2D::setText(const String &text)
    {
        if (text != mText)
        {
            mText = text;
            updateVertices();
        }
    }

    void SGText2D::setTextColor(const Color4 &color)
    {

    }

    const Color4 &SGText2D::getTextColor() const
    {
        return Color4::WHITE;
    }

    void SGText2D::setAnchorPos(const Vector2 &pos)
    {
        mAnchorPos = pos;
    }

    const Vector2 &SGText2D::getAnchorPos() const
    {
        return mAnchorPos;
    }

    void SGText2D::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        queue->addRenderable(RenderQueue::E_GRPID_SOLID, this);
        return;
        if (isVisible())
        {
            // 可见物体，才需要判断是否在视景体内，否则根本不用判断了
            FrustumBoundPtr frustumBound = smart_pointer_cast<FrustumBound>(bound);
            SGCameraPtr camera = smart_pointer_cast<SGCamera>(frustumBound->getNode());
            ViewportPtr viewport = camera->getViewport();
            int32_t width = viewport->getActualWidth();
            int32_t height = viewport->getActualHeight();

            NodePtr parent = getParent();

            while (parent != nullptr && parent->getNodeType() != E_NT_TRANSFORM2D)
                parent = parent->getParent();

            SGTransform2DPtr node = smart_pointer_cast<SGTransform2D>(parent);
            const Transform &transform = node->getLocalToWorldTransform();

            const Vector3 &pos = transform.getTranslate();

            Real anchorX = mSize.width * mAnchorPos.x();
            Real left = pos.x() - anchorX;
            Real right = pos.x() + anchorX;

            Real anchorY = mSize.height * mAnchorPos.y();
            Real top = pos.y() - anchorY;
            Real bottom = pos.y() + anchorY;

            if (left >= 0 && left < width && top >= 0 && top < height
                || right >= 0 && right < width && top >= 0 && top < height
                || left >= 0 && left < width && bottom >= 0 && bottom < height
                || right >= 0 && right < width && bottom >= 0 && bottom < height)
            {
                // 四个角上的点任意一点在范围内，则是表示有至少部分落入视景体中
                queue->addRenderable(RenderQueue::E_GRPID_OVERLAY, this);
            }

            SGNode::frustumCulling(bound, queue);
        }
    }

    MaterialPtr SGText2D::getMaterial() const
    {
        return mMaterial;
    }

    Renderer::PrimitiveType SGText2D::getPrimitiveType() const
    {
        return Renderer::E_PT_TRIANGLE_LIST;
    }

    VertexDataPtr SGText2D::getVertexData() const
    {
        return mVertexData;
    }

    IndexDataPtr SGText2D::getIndexData() const
    {
        return mIndexData;
    }

    bool SGText2D::isIndicesUsed() const
    {
        return true;
    }

    bool SGText2D::updateVertices()
    {
        bool ret = false;

        do 
        {
            mCharSet.clear();

            // 先更新字符集内容
            if (!mFont->updateContent(mText, mMaterial, mCharSet, mSize))
            {
                T3D_LOG_ERROR("Upadte text content failed !");
                break;
            }

            // 构建顶点
            size_t vertexCount = mCharSet.size() * 4;
            std::vector<Vertex> vertices;
            vertices.resize(vertexCount);

            size_t indexCount = mCharSet.size() * 6;
            std::vector<uint16_t> indices;
            indices.resize(indexCount);

            uint32_t color = Color4::WHITE.A8R8G8B8();

            size_t i = 0;
            size_t c = 0;
            Real left = -mAnchorPos.x() * mSize.width;
            Real top =  (Real(1.0f) - mAnchorPos.y()) * mSize.height;
            Real bottom = -mAnchorPos.y() * mSize.height;

            Texture *texture = mMaterial->getTexture(0);
            int32_t texWidth = texture->getTexWidth();
            int32_t texHeight = texture->getTexHeight();

            for (auto itr = mCharSet.begin(); itr != mCharSet.end() && i < vertices.size(); ++itr)
            {
                Font::CharPtr ch = *itr;

                // 计算顶点
                uint16_t index = i;

                // top-left
                Vertex &v0 = vertices[i++];
                v0.position[0] = left;
                v0.position[1] = top;
                v0.position[2] = 0.5;
                v0.diffuse = color;
                v0.texcoord[0] = Real(ch->mArea.left) / Real(texWidth);
                v0.texcoord[1] = Real(ch->mArea.top) / Real(texHeight);

                // bottom-left
                Vertex &v1 = vertices[i++];
                v1.position[0] = left;
                v1.position[1] = bottom;
                v1.position[2] = 0.5;
                v1.diffuse = color;
                v1.texcoord[0] = Real(ch->mArea.left) / Real(texWidth);
                v1.texcoord[1] = Real(ch->mArea.bottom) / Real(texHeight);

                // top-right
                Vertex &v2 = vertices[i++];
                v2.position[0] = left + ch->mArea.width();
                v2.position[1] = top;
                v2.position[2] = 0.5;
                v2.diffuse = color;
                v2.texcoord[0] = Real(ch->mArea.right) / Real(texWidth);
                v2.texcoord[1] = Real(ch->mArea.top) / Real(texHeight);

                // bottom-right
                Vertex &v3 = vertices[i++];
                v3.position[0] = left + ch->mArea.width();
                v3.position[1] = bottom;
                v3.position[2] = 0.5;
                v3.diffuse = color;
                v3.texcoord[0] = Real(ch->mArea.right) / Real(texWidth);
                v3.texcoord[1] = Real(ch->mArea.bottom) / Real(texHeight);

                T3D_LOG_INFO("code [%d]", ch->mCode);
                T3D_LOG_INFO("top-left uv : (%f, %f)", v0.texcoord[0], v0.texcoord[1]);
                T3D_LOG_INFO("bottom-left uv : (%f, %f)", v1.texcoord[0], v1.texcoord[1]);
                T3D_LOG_INFO("top-right uv : (%f, %f)", v2.texcoord[0], v2.texcoord[1]);
                T3D_LOG_INFO("bottom-right uv : (%f, %f)", v3.texcoord[0], v3.texcoord[1]);

                left += ch->mArea.width();

                // 索引
                indices[c++] = index;
                indices[c++] = index + 1;
                indices[c++] = index + 2;
                indices[c++] = index + 1;
                indices[c++] = index + 3;
                indices[c++] = index + 2;
            }

            // 构造顶点硬件缓冲区
            VertexDeclarationPtr vertexDecl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
            size_t offset = 0;
            const VertexElement &posElem = vertexDecl->addElement(0, offset, VertexElement::E_VET_FLOAT3, VertexElement::E_VES_POSITION);
            offset += posElem.getSize();
            const VertexElement &texElem = vertexDecl->addElement(0, offset, VertexElement::E_VET_FLOAT2, VertexElement::E_VES_TEXCOORD);
            offset += texElem.getSize();
            const VertexElement &colorElem = vertexDecl->addElement(0, offset, VertexElement::E_VET_COLOR, VertexElement::E_VES_DIFFUSE);
            offset += colorElem.getSize();

            size_t vertexSize = offset;
            HardwareVertexBufferPtr vertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(vertexSize, vertexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);
            vertexBuffer->writeData(0, vertexSize * vertexCount, &vertices[0]);
            mVertexData = VertexData::create(vertexDecl);
            mVertexData->addVertexBuffer(vertexBuffer);

            // 构造顶点索引缓冲区
            HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(HardwareIndexBuffer::E_IT_16BITS, indexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);
            indexBuffer->writeData(0, indexCount * sizeof(uint16_t), &indices[0]);
            mIndexData = IndexData::create(indexBuffer);

            ret = true;
        } while (0);

        return ret;
    }
}

