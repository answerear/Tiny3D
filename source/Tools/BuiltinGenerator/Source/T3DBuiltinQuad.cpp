/*******************************************************************************
* MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "T3DBuiltinQuad.h"
#include "T3DBuiltinGuidUtil.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    namespace
    {
        const char *SUB_MESH_NAME = "#0";
        const char *MESH_NAME = "quad";
    }

    //--------------------------------------------------------------------------

    TResult BuiltinQuad::build(const String &path)
    {
        //
        // Quad 在 XY 平面上，z = 0，边长 1，中心在原点，法线 -Z：
        //
        //        Y
        //        ^
        //   v0   |   v2
        //        |
        // X <----+---->
        //        |
        //   v1   |   v3
        //
        // v0 (-0.5,  0.5, 0)  v2 ( 0.5,  0.5, 0)
        // v1 (-0.5, -0.5, 0)  v3 ( 0.5, -0.5, 0)
        //

        struct QuadVertex
        {
            Vector3 position {};
            Vector3 normal {};
            Vector2 uv {};
            float weight[T3D_MAX_BLEND_BONES] {0.0f};
            uint8_t indices[T3D_MAX_BLEND_BONES] {0xFF};
        };

        // vertex attributes
        VertexAttributes attributes(5);

        uint32_t attribIdx = 0;
        // position
        uint32_t attrOffset = 0;
        VertexAttribute attrPos(0, attrOffset, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
        attrOffset += sizeof(Vector3);
        attributes[attribIdx] = attrPos;
        attribIdx++;
        // normal
        VertexAttribute attrNormal(0, attrOffset, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_NORMAL, 0);
        attrOffset += sizeof(Vector3);
        attributes[attribIdx] = attrNormal;
        attribIdx++;
        // uv
        VertexAttribute attrUV(0, attrOffset, VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, 0);
        attrOffset += sizeof(Vector2);
        attributes[attribIdx] = attrUV;
        attribIdx++;
        // blend weights
        VertexAttribute attrWeights(0, attrOffset, VertexAttribute::Type::E_VAT_FLOAT4, VertexAttribute::Semantic::E_VAS_BLENDWEIGHT, 0);
        attrOffset += sizeof(float) * T3D_MAX_BLEND_BONES;
        attributes[attribIdx] = attrWeights;
        attribIdx++;
        // blend indices
        VertexAttribute attrIndices(0, attrOffset, VertexAttribute::Type::E_VAT_UBYTE4, VertexAttribute::Semantic::E_VAS_BLENDINDICES, 0);
        attrOffset += sizeof(uint32_t) * T3D_MAX_BLEND_BONES;
        attributes[attribIdx] = attrIndices;
        attribIdx++;

        const Vector3 center(0.0f, 0.0f, 0.0f);
        const Vector3 extent(0.5f, 0.5f, 0.0f);
        const Vector3 normal = -Vector3::FORWARD;

        const uint32_t kVertexCount = 4;
        const uint32_t kIndexCount = 6;
        QuadVertex *vertices = T3D_POD_NEW_ARRAY(QuadVertex, kVertexCount);
        uint16_t *indices = T3D_POD_NEW_ARRAY(uint16_t, kIndexCount);

        // v0: top-left
        vertices[0].position = Vector3(-extent[0], extent[1], 0.0f);
        vertices[0].normal = normal;
        vertices[0].uv = Vector2(0.0f, 0.0f);

        // v1: bottom-left
        vertices[1].position = Vector3(-extent[0], -extent[1], 0.0f);
        vertices[1].normal = normal;
        vertices[1].uv = Vector2(0.0f, 1.0f);

        // v2: top-right
        vertices[2].position = Vector3(extent[0], extent[1], 0.0f);
        vertices[2].normal = normal;
        vertices[2].uv = Vector2(1.0f, 0.0f);

        // v3: bottom-right
        vertices[3].position = Vector3(extent[0], -extent[1], 0.0f);
        vertices[3].normal = normal;
        vertices[3].uv = Vector2(1.0f, 1.0f);

        // 绕序与 cube front 相同（顺时针为正面）
        indices[0] = 0; indices[1] = 2; indices[2] = 1;
        indices[3] = 1; indices[4] = 2; indices[5] = 3;

        Buffer vertexBuffer;
        vertexBuffer.Data = (uint8_t *)vertices;
        vertexBuffer.DataSize = sizeof(QuadVertex) * kVertexCount;
        Vertices vertexBuffers(1);
        vertexBuffers[0] = vertexBuffer;

        VertexStrides strides(1, sizeof(QuadVertex));
        VertexOffsets offsets(1, 0);

        Buffer indexBuffer;
        indexBuffer.Data = (uint8_t *)indices;
        indexBuffer.DataSize = sizeof(uint16_t) * kIndexCount;

        String name = SUB_MESH_NAME;
        MaterialPtr material = buildMaterial();
        SubMeshPtr submesh = SubMesh::create(name, material->getUUID(), PrimitiveType::kTriangleList, std::move(indexBuffer), true);
        SubMeshes subMeshes;
        subMeshes.emplace(name, submesh);

        UUID quadUUID = BuiltinGuidUtil::readExistingMetaUUID(path, String(MESH_NAME) + "." + Resource::EXT_MESH + ".meta");
        mMesh = T3D_MESH_MGR.createMesh(MESH_NAME, std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes),
            Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE, "", quadUUID);

        const Vector3 aabbMin = center - extent;
        const Vector3 aabbMax = center + extent;
        if (mMesh != nullptr)
        {
            mMesh->setBoundSeed(Bound::Type::AABB, aabbMin, aabbMax, Real(0.0));
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinQuad::save(const String &path)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mMesh == nullptr)
            {
                BGEN_LOG_WARNING("Quad mesh is null, skip saving quad mesh !");
                break;
            }

            String filename = String(MESH_NAME) + "." + Resource::EXT_MESH;
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            T3D_ASSERT(archive != nullptr);

            ret = T3D_MESH_MGR.saveMesh(archive, filename, mMesh);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to save mesh %s ! ERROR [%d]", filename.c_str(), ret);
                break;
            }

            filename = filename + ".meta";
            ret = archive->write(filename,
                [this](DataStream &stream, const String &filename, void *userData)
                {
                    MetaMeshPtr meta = MetaMesh::create(mMesh->getUUID());
                    return T3D_SERIALIZER_MGR.serialize(stream, meta);
                },
                nullptr);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate meta file (%s) for quad mesh ! ERROR [%d]", filename.c_str(), ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
}
