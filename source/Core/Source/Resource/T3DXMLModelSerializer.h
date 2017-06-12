
#ifndef __T3D_XML_MODEL_SERIALIZER_H__
#define __T3D_XML_MODEL_SERIALIZER_H__


#include "T3DModelSerializer.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DRenderer.h"
#include "DataStruct/T3DModelData.h"


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
        bool parseNode(tinyxml2::XMLElement *pNodeElement, NodeDataPtr node);

        bool parseSkeleton(tinyxml2::XMLElement *pSkelElement);
        bool parseSkeleton(tinyxml2::XMLElement *pBoneElement, uint16_t parentBone);
//         bool searchBone(const String &name, BoneDataPtr &bone, uint16_t &index);

        bool parseAnimation(tinyxml2::XMLElement *pAnimElement);
        bool parseAction(tinyxml2::XMLElement *pActionElement);
        bool parseKeyframe(tinyxml2::XMLElement *pKeyframeElement, const String &actionType, const String &boneName, size_t frameCount, const ObjectPtr &actionData);

        int32_t parseActionType(const String &type);

    protected:
        ModelDataPtr    mModelData;
    };
}


#endif  /*__T3D_XML_MODEL_SERIALIZER_H__*/
