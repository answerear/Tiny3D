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

#ifndef __T3D_XML_MODEL_SERIALIZER_H__
#define __T3D_XML_MODEL_SERIALIZER_H__


#include "T3DModelSerializer.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DRenderer.h"
#include "Misc/T3DModelData.h"


namespace tinyxml2
{
    class XMLElement;
}


namespace Tiny3D
{
    class XMLModelSerializer : public ModelSerializer
    {
    public:
        XMLModelSerializer();
        virtual ~XMLModelSerializer();

        virtual bool load(MemoryDataStream &stream, ModelDataPtr model) override;
        virtual bool save(MemoryDataStream &stream, ModelDataPtr model) override;

    protected:
        size_t parseVertexValue(const String &text, size_t &start, const VertexElement &attribute, void *value);
        size_t parseIndexValue(const String &text, size_t &start, bool is16bits, void *value);

        bool parseMatrixValue(const String &text, Matrix4 &mat);

        VertexElement::Semantic parseVertexSemantic(const String &name);
        VertexElement::Type parseVertexType(VertexElement::Semantic semantic, const String &name, size_t valueCount);
        Renderer::PrimitiveType parsePrimitiveType(const String &name);

        bool parseMesh(tinyxml2::XMLElement *pMeshElement, MeshDataPtr mesh);
        bool parseVertexBuffers(tinyxml2::XMLElement *pBuffersElement, MeshDataPtr mesh);
        bool parseVertexBuffer(tinyxml2::XMLElement *pBufferElement, VertexBufferPtr buffer, size_t index);

        bool parseSubMeshes(tinyxml2::XMLElement *pMeshElement, MeshDataPtr mesh);
        bool parseSubMesh(tinyxml2::XMLElement *pSubMeshElement, MeshDataPtr mesh);

        bool parseHierarchy(tinyxml2::XMLElement *pHierarchyElement);
        bool parseNode(tinyxml2::XMLElement *pNodeElement, uint16_t parent);

        bool parseSkeleton(tinyxml2::XMLElement *pSkelElement);
        bool parseSkeleton(tinyxml2::XMLElement *pBoneElement, uint16_t parentBone);
        bool searchBone(const String &name, BoneDataPtr &bone, uint16_t &index);

//         bool parseSkins(tinyxml2::XMLElement *pSkinElement);
//         bool parseSkin(tinyxml2::XMLElement *pSkinElement);

        bool parseAnimation(tinyxml2::XMLElement *pAnimElement);
        bool parseAction(tinyxml2::XMLElement *pActionElement);
        bool parseKeyframe(tinyxml2::XMLElement *pKeyframeElement, const String &actionType, const String &boneName, size_t frameCount, const ObjectPtr &actionData);

        int32_t parseActionType(const String &type);

    protected:
        ModelDataPtr    mModelData;
    };
}


#endif  /*__T3D_XML_MODEL_SERIALIZER_H__*/
