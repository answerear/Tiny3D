
#include "T3DXMLModelSerializer.h"
#include "Support/tinyxml2/tinyxml2.h"
#include "DataStruct/T3DModelData.h"


namespace Tiny3D
{
    using namespace tinyxml2;

    XMLModelSerializer::XMLModelSerializer()
    {

    }

    XMLModelSerializer::~XMLModelSerializer()
    {

    }

    bool XMLModelSerializer::load(MemoryDataStream &stream, ModelDataPtr model)
    {
        bool ret = false;
        uint8_t *buffer = nullptr;
        size_t bufSize = 0;
        stream.getBuffer(buffer, bufSize);

        if (bufSize > 0 && buffer != nullptr)
        {
            XMLDocument *pDoc = new XMLDocument();

            if (pDoc != nullptr && pDoc->Parse((const char *)buffer, bufSize) == XML_SUCCESS)
            {
                ret = true;

                mModelData = model;

                XMLDeclaration *pDeclaration = pDoc->ToDeclaration();
                XMLElement *pVersionElement = pDoc->FirstChildElement(T3D_XML_TAG_VERSION);

                String version = pVersionElement->GetText();

                XMLElement *pSceneElement = pDoc->FirstChildElement(T3D_XML_TAG_SCENE);
                XMLElement *pModelElement = pSceneElement->FirstChildElement(T3D_XML_TAG_MODEL);
                XMLElement *pMeshElement = pModelElement->FirstChildElement(T3D_XML_TAG_MESH);

                int32_t meshCount = pModelElement->IntAttribute(T3D_XML_ATTRIB_COUNT);
                mModelData->mIsVertexShared = pModelElement->BoolAttribute(T3D_XML_ATTRIB_SHARED);

                mModelData->mMeshes.resize(meshCount);

                // 网格数据
                size_t i = 0;
                while (pMeshElement != nullptr)
                {
                    MeshDataPtr mesh = MeshData::create();
                    ret = ret && parseMesh(pMeshElement, mesh);

                    if (!ret)
                        break;

                    mModelData->mMeshes[i] = mesh;
                    pMeshElement = pMeshElement->NextSiblingElement(T3D_XML_TAG_MESH);
                    ++i;
                }

                // 蒙皮偏移变换数据
                XMLElement *pSkinElement = pModelElement->FirstChildElement(T3D_XML_TAG_SKIN);
                ret = ret && parseSkins(pSkinElement);

                // 骨骼层次和骨骼本地变换数据
                XMLElement *pSkelElement = pModelElement->FirstChildElement(T3D_XML_TAG_SKELETON);
                ret = ret && parseSkeleton(pSkelElement);

                // 关键帧动画数据
                XMLElement *pAnimElement = pModelElement->FirstChildElement(T3D_XML_TAG_ANIMATION);
                ret = ret && parseAnimation(pAnimElement);
            }

            delete pDoc;
        }

        return ret;
    }


    size_t XMLModelSerializer::parseVertexValue(const String &text, size_t &start, const VertexElement &attribute, void *value)
    {
        size_t step = 0;

        switch (attribute.getSemantic())
        {
        case VertexElement::E_VES_POSITION:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT3:
                    {
                        float pos[3];
                        pos[0] = getValue<float>(text, start);
                        pos[1] = getValue<float>(text, start);
                        pos[2] = getValue<float>(text, start);
                        step = sizeof(pos);
                        memcpy(value, pos, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE3:
                    {
                        double pos[3];
                        pos[0] = getValue<double>(text, start);
                        pos[1] = getValue<double>(text, start);
                        pos[2] = getValue<double>(text, start);
                        step = sizeof(pos);
                        memcpy(value, pos, step);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_BLENDWEIGHT:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT4:
                    {
                        float weight[4];
                        weight[0] = getValue<float>(text, start);
                        weight[1] = getValue<float>(text, start);
                        weight[2] = getValue<float>(text, start);
                        weight[3] = getValue<float>(text, start);
                        step = sizeof(weight);
                        memcpy(value, weight, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE4:
                    {
                        double weight[4];
                        weight[0] = getValue<double>(text, start);
                        weight[1] = getValue<double>(text, start);
                        weight[2] = getValue<double>(text, start);
                        weight[3] = getValue<double>(text, start);
                        step = sizeof(weight);
                        memcpy(value, weight, step);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_BLENDINDICES:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_INT4:
                    {
                        int32_t indices[4];
                        indices[0] = getValue<int32_t>(text, start);
                        indices[1] = getValue<int32_t>(text, start);
                        indices[2] = getValue<int32_t>(text, start);
                        indices[3] = getValue<int32_t>(text, start);
                        memcpy(value, indices, sizeof(indices));
                        step += sizeof(indices);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_NORMAL:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT3:
                    {
                        float normal[3];
                        normal[0] = getValue<float>(text, start);
                        normal[1] = getValue<float>(text, start);
                        normal[2] = getValue<float>(text, start);
                        step = sizeof(normal);
                        memcpy(value, normal, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE3:
                    {
                        double normal[3];
                        normal[0] = getValue<double>(text, start);
                        normal[1] = getValue<double>(text, start);
                        normal[2] = getValue<double>(text, start);
                        step = sizeof(normal);
                        memcpy(value, normal, step);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_DIFFUSE:
        case VertexElement::E_VES_SPECULAR:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_COLOR:
                    {
                        float color[4];
                        color[0] = getValue<float>(text, start);
                        color[1] = getValue<float>(text, start);
                        color[2] = getValue<float>(text, start);
                        color[3] = getValue<float>(text, start);
                        uint8_t *c = (uint8_t *)value;
                        c[3] = (uint8_t)(color[0] * 255);
                        c[2] = (uint8_t)(color[1] * 255);
                        c[1] = (uint8_t)(color[2] * 255);
                        c[0] = (uint8_t)(color[3] * 255);
                        step = sizeof(uint32_t);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_TEXCOORD:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT2:
                    {
                        float texcoord[2];
                        texcoord[0] = getValue<float>(text, start);
                        texcoord[1] = getValue<float>(text, start);
                        step = sizeof(texcoord);
                        memcpy(value, texcoord, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE2:
                    {
                        double texcoord[2];
                        texcoord[0] = getValue<double>(text, start);
                        texcoord[1] = getValue<double>(text, start);
                        step = sizeof(texcoord);
                        memcpy(value, texcoord, step);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_TANGENT:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT3:
                    {
                        float tangent[3];
                        tangent[0] = getValue<float>(text, start);
                        tangent[1] = getValue<float>(text, start);
                        tangent[2] = getValue<float>(text, start);
                        step = sizeof(tangent);
                        memcpy(value, tangent, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE3:
                    {
                        double tangent[3];
                        tangent[0] = getValue<double>(text, start);
                        tangent[1] = getValue<double>(text, start);
                        tangent[2] = getValue<double>(text, start);
                        step = sizeof(tangent);
                        memcpy(value, tangent, step);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_BINORMAL:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT3:
                    {
                        float binormal[3];
                        binormal[0] = getValue<float>(text, start);
                        binormal[1] = getValue<float>(text, start);
                        binormal[2] = getValue<float>(text, start);
                        step = sizeof(binormal);
                        memcpy(value, binormal, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE3:
                    {
                        double binormal[3];
                        binormal[0] = getValue<double>(text, start);
                        binormal[1] = getValue<double>(text, start);
                        binormal[2] = getValue<double>(text, start);
                        step = sizeof(binormal);
                        memcpy(value, binormal, step);
                    }
                    break;
                }
            }
            break;
        }

        return step;
    }

    size_t XMLModelSerializer::parseIndexValue(const String &text, size_t &start, bool is16bits, void *value)
    {
        size_t step = 0;

        if (is16bits)
        {
            uint16_t index = getValue<uint16_t>(text, start);
            step = sizeof(uint16_t);
            memcpy(value, &index, step);
        }
        else
        {
            uint32_t index = getValue<uint32_t>(text, start);
            step = sizeof(uint32_t);
            memcpy(value, &index, step);
        }

        return step;
    }

    Renderer::PrimitiveType XMLModelSerializer::parsePrimitiveType(const String &name)
    {
        Renderer::PrimitiveType primitiveType = Renderer::E_PT_TRIANGLE_LIST;

        if (name == T3D_PRITYPE_TRIANGLE_LIST)
        {
            primitiveType = Renderer::E_PT_TRIANGLE_LIST;
        }
        else
        {
            // 暂时不支持其他图元类型
            T3D_ASSERT(0);
        }

        return primitiveType;
    }

    VertexElement::Semantic XMLModelSerializer::parseVertexSemantic(const String &name)
    {
        VertexElement::Semantic semantic = VertexElement::E_VES_POSITION;

        if (name == T3D_VERTEX_SEMANTIC_POSITION)
        {
            semantic = VertexElement::E_VES_POSITION;
        }
        else if (name == T3D_VERTEX_SEMANTIC_TEXCOORD)
        {
            semantic = VertexElement::E_VES_TEXCOORD;
        }
        else if (name == T3D_VERTEX_SEMANTIC_NORMAL)
        {
            semantic = VertexElement::E_VES_NORMAL;
        }
        else if (name == T3D_VERTEX_SEMANTIC_TANGENT)
        {
            semantic = VertexElement::E_VES_TANGENT;
        }
        else if (name == T3D_VERTEX_SEMANTIC_BINORMAL)
        {
            semantic = VertexElement::E_VES_BINORMAL;
        }
        else if (name == T3D_VERTEX_SEMANTIC_COLOR)
        {
            semantic = VertexElement::E_VES_DIFFUSE;
        }
        else if (name == T3D_VERTEX_SEMANTIC_BLEND_WEIGHT)
        {
            semantic = VertexElement::E_VES_BLENDWEIGHT;
        }
        else if (name == T3D_VERTEX_SEMANTIC_BLEND_INDEX)
        {
            semantic = VertexElement::E_VES_BLENDINDICES;
        }

        return semantic;
    }

    VertexElement::Type XMLModelSerializer::parseVertexType(VertexElement::Semantic semantic, const String &name, size_t valueCount)
    {
        bool bColorValue = false;

        bColorValue = (semantic == VertexElement::E_VES_DIFFUSE || semantic == VertexElement::E_VES_SPECULAR);

        VertexElement::Type type = VertexElement::E_VET_FLOAT3;

        if (bColorValue)
        {
            type = VertexElement::E_VET_COLOR;
        }
        else
        {
            if (name == T3D_VALUE_TYPE_FLOAT)
            {
                switch (valueCount)
                {
                case 1:
                    {
                        type = VertexElement::E_VET_FLOAT1;
                    }
                    break;
                case 2:
                    {
                        type = VertexElement::E_VET_FLOAT2;
                    }
                    break;
                case 3:
                    {
                        type = VertexElement::E_VET_FLOAT3;
                    }
                    break;
                case 4:
                    {
                        type = VertexElement::E_VET_FLOAT4;
                    }
                    break;
                }
            }
            else if (name == T3D_VALUE_TYPE_DOUBLE)
            {
                switch (valueCount)
                {
                case 1:
                    {
                        type = VertexElement::E_VET_DOUBLE1;
                    }
                    break;
                case 2:
                    {
                        type = VertexElement::E_VET_DOUBLE2;
                    }
                    break;
                case 3:
                    {
                        type = VertexElement::E_VET_DOUBLE3;
                    }
                    break;
                case 4:
                    {
                        type = VertexElement::E_VET_DOUBLE4;
                    }
                    break;
                }
            }
            else if (name == T3D_VALUE_TYPE_INT)
            {
                switch (valueCount)
                {
                case 1:
                    {
                        type = VertexElement::E_VET_INT1;
                    }
                    break;
                case 2:
                    {
                        type = VertexElement::E_VET_INT2;
                    }
                    break;
                case 3:
                    {
                        type = VertexElement::E_VET_INT3;
                    }
                    break;
                case 4:
                    {
                        type = VertexElement::E_VET_INT4;
                    }
                    break;
                }
            }
            else if (name == T3D_VALUE_TYPE_LONG)
            {

            }

        }

        return type;
    }

    bool XMLModelSerializer::parseMatrixValue(const String &text, Matrix4 &mat)
    {
        size_t start = 0;

        start = getStartPos(text, start);

        size_t i = 0, j = 0;

        for (i = 0; i < 4; ++i)
        {
            for (j = 0; j < 4; ++j)
            {
                float val = getValue<float>(text, start);
                mat[i][j] = val;
            }
        }

        return true;
    }

    bool XMLModelSerializer::parseMesh(tinyxml2::XMLElement *pMeshElement, MeshDataPtr mesh)
    {
        XMLElement *pBuffersElement = pMeshElement->FirstChildElement(T3D_XML_TAG_VERTEX_BUFFERS);

        bool ret = parseVertexBuffers(pBuffersElement, mesh);
        ret = ret && parseSubMeshes(pMeshElement, mesh);

        return true;
    }

    bool XMLModelSerializer::parseVertexBuffers(tinyxml2::XMLElement *pBuffersElement, MeshDataPtr mesh)
    {
        size_t count = pBuffersElement->IntAttribute(T3D_XML_ATTRIB_COUNT);

        XMLElement *pBufferElement = pBuffersElement->FirstChildElement(T3D_XML_TAG_VERTEX_BUFFER);

        size_t i = 0;
        bool ret = (pBufferElement != nullptr);

        while (pBufferElement != nullptr)
        {
            VertexBufferPtr buffer = VertexBuffer::create();
            ret = ret && parseVertexBuffer(pBufferElement, buffer, i);
            mesh->mBuffers.push_back(buffer);
            pBufferElement = pBufferElement->NextSiblingElement(T3D_XML_TAG_VERTEX_BUFFER);
            ++i;
        }

        return true;
    }

    bool XMLModelSerializer::parseVertexBuffer(tinyxml2::XMLElement *pBufferElement, VertexBufferPtr buffer, size_t index)
    {
        XMLElement *pAttribsElement = pBufferElement->FirstChildElement(T3D_XML_TAG_ATTRIBUTES);

        // 解析顶点属性列表大小
        size_t count = pAttribsElement->IntAttribute(T3D_XML_ATTRIB_COUNT);

        if (count == 0)
        {
            T3D_LOG_ERROR("The size of vertex attributes is zero !!!");
            return false;
        }

        // 解析顶点属性
        XMLElement *pAttribElement = pAttribsElement->FirstChildElement(T3D_XML_TAG_ATTRIBUTE);
        size_t i = 0;
        size_t offset = 0;

        while (pAttribElement != nullptr && i < count)
        {
            VertexElement::Semantic vertexSemantic = parseVertexSemantic(pAttribElement->Attribute(T3D_XML_ATTRIB_ID));
            size_t valueCount = pAttribElement->IntAttribute(T3D_XML_ATTRIB_SIZE);
            VertexElement::Type vertexType = parseVertexType(vertexSemantic, pAttribElement->Attribute(T3D_XML_ATTRIB_TYPE), valueCount);
            buffer->mAttributes.push_back(VertexElement(index, offset, vertexType, vertexSemantic));
            const VertexElement &element = buffer->mAttributes.back(); 
            offset += element.getSize();
            i++;
            pAttribElement = pAttribElement->NextSiblingElement(T3D_XML_TAG_ATTRIBUTE);
        }

        // 解析顶点数据
        XMLElement *pVerticesElement = pBufferElement->FirstChildElement(T3D_XML_TAG_DATA);
        size_t vertexCount = pVerticesElement->IntAttribute(T3D_XML_ATTRIB_COUNT);

        if (count == 0)
        {
            T3D_LOG_ERROR("Size of vertex data is zero !!!");
            return false;
        }

        size_t vertexSize = offset;
        size_t valueCount = vertexCount * vertexSize;
        buffer->mVertices.resize(valueCount);
        buffer->mVertexSize = vertexSize;

        String text = pVerticesElement->GetText();
        size_t start = 0;
        start = getStartPos(text, start);
        i = 0;

        do
        {
            auto itr = buffer->mAttributes.begin();

            while (itr != buffer->mAttributes.end())
            {
                auto attribute = *itr;
                if (attribute.getStream() == index)
                {
                    size_t step = parseVertexValue(text, start, attribute, &buffer->mVertices[i]);
                    i += step;
                }

                ++itr;
            }
        } while (i < valueCount);

        return true;
    }

    bool XMLModelSerializer::parseSubMeshes(tinyxml2::XMLElement *pMeshElement, MeshDataPtr mesh)
    {
        XMLElement *pSubMeshesElement = pMeshElement->FirstChildElement(T3D_XML_TAG_SUBMESHES);
        int32_t count = pSubMeshesElement->IntAttribute(T3D_XML_ATTRIB_COUNT);

        XMLElement *pSubMeshElement = pSubMeshesElement->FirstChildElement(T3D_XML_TAG_SUBMESH);
        while (pSubMeshElement != nullptr)
        {
            parseSubMesh(pSubMeshElement, mesh);
            pSubMeshElement = pSubMeshElement->NextSiblingElement(T3D_XML_TAG_SUBMESH);
        }

        return true;
    }

    bool XMLModelSerializer::parseSubMesh(tinyxml2::XMLElement *pSubMeshElement, MeshDataPtr mesh)
    {
        Renderer::PrimitiveType primitiveType = parsePrimitiveType(pSubMeshElement->Attribute(T3D_XML_ATTRIB_PRIMITIVE));
        int32_t primitiveCount = pSubMeshElement->IntAttribute(T3D_XML_ATTRIB_COUNT);
        String materialName = pSubMeshElement->Attribute(T3D_XML_TAG_MATERIAL) + String(".") + T3D_TXT_MATERIAL_FILE_EXT;

        XMLElement *pIndicesElement = pSubMeshElement->FirstChildElement(T3D_XML_TAG_INDICES);
        int32_t indexCount = pIndicesElement->IntAttribute(T3D_XML_ATTRIB_COUNT);
        bool is16bits = pIndicesElement->BoolAttribute(T3D_XML_ATTRIB_16BITS);

        size_t indexSize = indexCount * (is16bits ? sizeof(uint16_t) : sizeof(uint32_t));

        SubMeshDataPtr submesh = SubMeshData::create(materialName, primitiveType, is16bits, indexCount);

        String text = pIndicesElement->GetText();
        size_t start = 0;
        size_t i = 0;

        do 
        {
            size_t step = parseIndexValue(text, start, is16bits, &submesh->mIndices[i]);
            i += step;
        } while (i < indexSize);

        mesh->mSubMeshes.push_back(submesh);

        return true;
    }

    bool XMLModelSerializer::parseSkins(tinyxml2::XMLElement *pSkinElement)
    {
        bool ret = true;
        String name = pSkinElement->Attribute(T3D_XML_ATTRIB_ID);
        size_t boneCount = pSkinElement->IntAttribute(T3D_XML_ATTRIB_COUNT);

        mModelData->mBones.resize(boneCount);

        size_t i = 0;
        XMLElement *pBoneElement = pSkinElement->FirstChildElement(T3D_XML_TAG_BONE);
        while (pBoneElement != nullptr)
        {
            ret = ret && parseSkin(pBoneElement, i);
            pBoneElement = pBoneElement->NextSiblingElement(T3D_XML_TAG_BONE);
            ++i;
        }

        return ret;
    }

    bool XMLModelSerializer::parseSkin(tinyxml2::XMLElement *pBoneElement, size_t index)
    {
        bool ret = true;
        String name = pBoneElement->Attribute(T3D_XML_ATTRIB_ID);

        BoneDataPtr bone = BoneData::create(name);
        mModelData->mBones[index] = bone;

        XMLElement *pTransformElement = pBoneElement->FirstChildElement(T3D_XML_TAG_TRANSFORM);

        if (pTransformElement != nullptr)
        {
            String text = pTransformElement->GetText();
            parseMatrixValue(text, bone->mOffsetMatrix);
        }

        return ret;
    }

    bool XMLModelSerializer::parseSkeleton(tinyxml2::XMLElement *pSkelElement)
    {
        if (pSkelElement == nullptr)
            return true;

        XMLElement *pBoneElement = pSkelElement->FirstChildElement(T3D_XML_TAG_BONE);
        bool ret = parseSkeleton(pBoneElement, 0xFFFF);

        return ret;
    }

    bool XMLModelSerializer::parseSkeleton(tinyxml2::XMLElement *pBoneElement, uint16_t parentBone)
    {
        bool ret = true;
        String name = pBoneElement->Attribute(T3D_XML_ATTRIB_ID);

        XMLElement *pTransformElement = pBoneElement->FirstChildElement(T3D_XML_TAG_TRANSFORM);

        if (pTransformElement != nullptr)
        {
            String text = pTransformElement->GetText();
            
            BoneDataPtr bone;
            uint16_t index = 0;
            if (searchBone(name, bone, index))
            {
                bone->mParentBone = parentBone;
                parseMatrixValue(text, bone->mLocalMatrix);

                XMLElement *pChildElement = pBoneElement->FirstChildElement(T3D_XML_TAG_BONE);
                while (pChildElement != nullptr)
                {
                    ret = ret && parseSkeleton(pChildElement, index);
                    pChildElement = pChildElement->NextSiblingElement(T3D_XML_TAG_BONE);
                }
            }
            else
            {
                ret = false;
            }
        }
        else
        {
            ret = false;
        }

        return ret;
    }

    bool XMLModelSerializer::searchBone(const String &name, BoneDataPtr &bone, uint16_t &index)
    {
        bool found = false;

        uint16_t i = 0;
        for (i = 0; i < mModelData->mBones.size(); ++i)
        {
            auto data = mModelData->mBones[i];
            if (name == data->mName)
            {
                bone = data;
                index = i;
                found = true;
                break;
            }
        }

        return found;
    }

    bool XMLModelSerializer::parseAnimation(tinyxml2::XMLElement *pAnimElement)
    {
        if (pAnimElement == nullptr)
            return true;

        bool ret = true;
        size_t actionCount = pAnimElement->IntAttribute(T3D_XML_ATTRIB_COUNT);

        XMLElement *pActionElement = pAnimElement->FirstChildElement(T3D_XML_TAG_ACTION);
        while (pActionElement != nullptr)
        {
            ret = ret && parseAction(pActionElement);
            pActionElement = pActionElement->NextSiblingElement(T3D_XML_TAG_ACTION);
        }

        return ret;
    }

    bool XMLModelSerializer::parseAction(tinyxml2::XMLElement *pActionElement)
    {
        bool ret = true;
        String actionName = pActionElement->Attribute(T3D_XML_ATTRIB_ID);
        float duration = pActionElement->FloatAttribute(T3D_XML_ATTRIB_DURATION);
        int32_t length = (int32_t)(duration * 1000);
        ActionDataPtr actionData = ActionData::create(actionName, length);
        auto result = mModelData->mAnimations.insert(ModelData::AnimationValue(actionName, actionData));

        XMLElement *pKeyframeElement = pActionElement->FirstChildElement(T3D_XML_TAG_KEYFRAME);

        while (pKeyframeElement != nullptr)
        {
            String actionType = pKeyframeElement->Attribute(T3D_XML_ATTRIB_TYPE);
            size_t frameCount = pKeyframeElement->IntAttribute(T3D_XML_ATTRIB_COUNT);
            String boneName = pKeyframeElement->Attribute(T3D_XML_ATTRIB_BONE);

            ret = ret && parseKeyframe(pKeyframeElement, actionType, boneName, frameCount, result.first->second);

            pKeyframeElement = pKeyframeElement->NextSiblingElement(T3D_XML_TAG_KEYFRAME);
        }

        return ret;
    }

    bool XMLModelSerializer::parseKeyframe(tinyxml2::XMLElement *pKeyframeElement, const String &actionType, const String &boneName, size_t frameCount, const ObjectPtr &actionData)
    {
        bool ret = true;

        KeyFrameData::Type frameType = (KeyFrameData::Type)parseActionType(actionType);
        XMLElement *pFrameElement = pKeyframeElement->FirstChildElement(T3D_XML_TAG_FRAME);

        ActionDataPtr action = smart_pointer_cast<ActionData>(actionData);

        switch (frameType)
        {
        case KeyFrameData::E_TYPE_TRANSLATION:
            {
                auto result = action->mBonesTranslation.insert(ActionData::BonesValue(boneName, ActionData::KeyFrames()));

                while (pFrameElement != nullptr)
                {
                    float timestamp = pFrameElement->FloatAttribute(T3D_XML_ATTRIB_TIME);
                    String text = pFrameElement->GetText();
                    size_t start = 0;
                    start = getStartPos(text, start);
                    float tx = getValue<float>(text, start);
                    float ty = getValue<float>(text, start);
                    float tz = getValue<float>(text, start);
                    int64_t ts = (int64_t)((double)timestamp * 1000.0);

                    KeyFrameDataTPtr keyframe = KeyFrameDataT::create(ts, Vector3(tx, ty, tz));
                    result.first->second.push_back(keyframe);

                    ret = true;
                    pFrameElement = pFrameElement->NextSiblingElement(T3D_XML_TAG_FRAME);
                }
            }
            break;
        case KeyFrameData::E_TYPE_ROTATION:
            {
                auto result = action->mBonesRotation.insert(ActionData::BonesValue(boneName, ActionData::KeyFrames()));

                while (pFrameElement != nullptr)
                {
                    float timestamp = pFrameElement->FloatAttribute(T3D_XML_ATTRIB_TIME);
                    String text = pFrameElement->GetText();
                    size_t start = 0;
                    start = getStartPos(text, start);
                    float qx = getValue<float>(text, start);
                    float qy = getValue<float>(text, start);
                    float qz = getValue<float>(text, start);
                    float qw = getValue<float>(text, start);
                    int64_t ts = (int64_t)((double)timestamp * 1000.0);

                    KeyFrameDataRPtr keyframe = KeyFrameDataR::create(ts, Quaternion(qw, qx, qy, qz));
                    result.first->second.push_back(keyframe);

                    ret = true;
                    pFrameElement = pFrameElement->NextSiblingElement(T3D_XML_TAG_FRAME);
                }
            }
            break;
        case KeyFrameData::E_TYPE_SCALING:
            {
                auto result = action->mBonesScaling.insert(ActionData::BonesValue(boneName, ActionData::KeyFrames()));

                while (pFrameElement != nullptr)
                {
                    float timestamp = pFrameElement->FloatAttribute(T3D_XML_ATTRIB_TIME);
                    String text = pFrameElement->GetText();
                    size_t start = 0;
                    start = getStartPos(text, start);
                    float sx = getValue<float>(text, start);
                    float sy = getValue<float>(text, start);
                    float sz = getValue<float>(text, start);
                    int64_t ts = (int64_t)((double)timestamp * 1000.0);

                    KeyFrameDataSPtr keyframe = KeyFrameDataS::create(ts, Vector3(sx, sy, sz));
                    result.first->second.push_back(keyframe);

                    ret = true;
                    pFrameElement = pKeyframeElement->NextSiblingElement(T3D_XML_TAG_FRAME);
                }
            }
            break;
        default:
            ret = false;
            break;
        }

        return ret;
    }

    int32_t XMLModelSerializer::parseActionType(const String &type)
    {
        KeyFrameData::Type actionType = KeyFrameData::E_TYPE_UNKNOWN;

        if (T3D_ACTION_TYPE_TRANSLATION == type)
        {
            actionType = KeyFrameData::E_TYPE_TRANSLATION;
        }
        else if (T3D_ACTION_TYPE_ROTATION == type)
        {
            actionType = KeyFrameData::E_TYPE_ROTATION;
        }
        else if (T3D_ACTION_TYPE_SCALING == type)
        {
            actionType = KeyFrameData::E_TYPE_SCALING;
        }

        return actionType;
    }

    bool XMLModelSerializer::save(MemoryDataStream &stream, ModelDataPtr model)
    {
        return true;
    }
}
