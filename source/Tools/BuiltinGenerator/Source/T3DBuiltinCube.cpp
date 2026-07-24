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


#include "T3DBuiltinCube.h"
#include "T3DBuiltinGuidUtil.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char *SUB_MESH_NAME = "#0";
    const char *MESH_NAME = "cube";
    const char *TEST_MESH_NAME = "test-cube";
    
    //--------------------------------------------------------------------------

    TResult BuiltinCube::build(const String &path)
    {
        // 
        // 正方体顶点定义如下：
        //
        //           v6-------v4
        //          /|       /|
        //         / |      / |
        //        v0-------v2 |
        //        |  v7----|--v5
        //        | /      | /
        //        |/       |/
        //        v1-------v3
        //

        struct BoxVertex
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

        // vertices & indices
        Vector3 offset;
        Vector3 center(0.0f, 0.0f, 0.0f);
        Vector3 extent(0.5f, 0.5f, 0.5f);

        const uint32_t kVertexCount = 24;
        const uint32_t kIndexCount = 36;
        BoxVertex *vertices = T3D_POD_NEW_ARRAY(BoxVertex, kVertexCount);
        uint16_t *indices = T3D_POD_NEW_ARRAY(uint16_t, kIndexCount);

        // front - V0
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[0].position = center + offset;
        vertices[0].normal = Vector3::FORWARD;
        vertices[0].uv = Vector2(0.0f, 0.0f);
        
        // front - V1
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[1].position = center + offset;
        vertices[1].normal = Vector3::FORWARD;
        vertices[1].uv = Vector2(0.0f, 0.5f);
        
        // front - V2
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[2].position = center + offset;
        vertices[2].normal = Vector3::FORWARD;
        vertices[2].uv = Vector2(0.5f, 0.0f);
        
        // front - V3
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[3].position = center + offset;
        vertices[3].normal = Vector3::FORWARD;
        vertices[3].uv = Vector2(0.5f, 0.5f);

        // right - V2
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[4].position = center + offset;
        vertices[4].normal = Vector3::RIGHT;
        vertices[4].uv = Vector2(0.0f, 0.5f);
        
        // right - V3
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[5].position = center + offset;
        vertices[5].normal = Vector3::RIGHT;
        vertices[5].uv = Vector2(0.0f, 1.0f);
        
        // right - V4
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[6].position = center + offset;
        vertices[6].normal = Vector3::RIGHT;
        vertices[6].uv = Vector2(0.5f, 0.5f);
        
        // right - V5
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[7].position = center + offset;
        vertices[7].normal = Vector3::RIGHT;
        vertices[7].uv = Vector2(0.5f, 1.0f);

        // back - V4
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[8].position = center + offset;
        vertices[8].normal = -Vector3::FORWARD;
        vertices[8].uv = Vector2(0.0f, 0.0f);
        
        // back - V5
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[9].position = center + offset;
        vertices[9].normal = -Vector3::FORWARD;
        vertices[9].uv = Vector2(0.0f, 0.5f);

        // back - V6
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[10].position = center + offset;
        vertices[10].uv = Vector2(0.5f, 0.0f);
        
        // back - V7
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[11].position = center + offset;
        vertices[11].normal = -Vector3::FORWARD;
        vertices[11].uv = Vector2(0.5f, 0.5f);
        
        // left - V6
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[12].position = center + offset;
        vertices[12].normal = -Vector3::RIGHT;
        vertices[12].uv = Vector2(0.5f, 0.5f);
        
        // left - V7
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[13].position = center + offset;
        vertices[13].normal = -Vector3::RIGHT;
        vertices[13].uv = Vector2(0.5f, 1.0f);

        // left - V0
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[14].position = center + offset;
        vertices[14].normal = -Vector3::RIGHT;
        vertices[14].uv = Vector2(1.0f, 0.5f);
        
        // left - V1
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[15].position = center + offset;
        vertices[15].normal = -Vector3::RIGHT;
        vertices[15].uv = Vector2(1.0f, 1.0f);

        // top - V0
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[16].position = center + offset;
        vertices[16].normal = Vector3::UP;
        vertices[16].uv = Vector2(0.5f, 0.5f);
        
        // top - V2
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[17].position = center + offset;
        vertices[17].normal = Vector3::UP;
        vertices[17].uv = Vector2(1.0f, 0.5f);

        // top - V4
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[18].position = center + offset;
        vertices[18].normal = Vector3::UP;
        vertices[18].uv = Vector2(1.0f, 0.0f);

        // top - V6
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[19].position = center + offset;
        vertices[19].normal = Vector3::UP;
        vertices[19].uv = Vector2(0.5f, 0.0f);

        // bottom - V1
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[20].position = center + offset;
        vertices[20].normal = -Vector3::UP;
        vertices[20].uv = Vector2(0.5f, 0.5f);

        // bottom - V7
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[21].position = center + offset;
        vertices[21].normal = -Vector3::UP;
        vertices[21].uv = Vector2(0.5f, 1.0f);

        // bottom - V3
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[22].position = center + offset;
        vertices[22].normal = -Vector3::UP;
        vertices[22].uv = Vector2(1.0f, 0.5f);
        
        // bottom - V5
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[23].position = center + offset;
        vertices[23].normal = -Vector3::UP;
        vertices[23].uv = Vector2(1.0f, 1.0f);
        
        // Front face
        indices[0] = 0; indices[1] = 2; indices[2] = 1;
        indices[3] = 1; indices[4] = 2; indices[5] = 3;
        
        // Back
        indices[6] = 4; indices[7] = 6; indices[8] = 5;
        indices[9] = 5; indices[10] = 6; indices[11] = 7;
        
        // Left
        indices[12] = 8; indices[13] = 10; indices[14] = 9;
        indices[15] = 9; indices[16] = 10; indices[17] = 11;
        
        // Right
        indices[18] = 12; indices[19] = 14; indices[20] = 13;
        indices[21] = 13; indices[22] = 14; indices[23] = 15;
        
        // Top
        indices[24] = 16; indices[25] = 19; indices[26] = 18;
        indices[27] = 18; indices[28] = 17; indices[29] = 16;
        
        // Bottom
        indices[30] = 20; indices[31] = 22; indices[32] = 21;
        indices[33] = 21; indices[34] = 22; indices[35] = 23;
        
        // construct mesh resource
        Buffer vertexBuffer;
        vertexBuffer.Data = (uint8_t*)vertices;
        vertexBuffer.DataSize = sizeof(BoxVertex) * kVertexCount;
        Vertices vertexBuffers(1);
        vertexBuffers[0] = vertexBuffer;
        
        VertexStrides strides(1, sizeof(BoxVertex));
        VertexOffsets offsets(1, 0);
        
        Buffer indexBuffer;
        indexBuffer.Data = (uint8_t*)indices;
        indexBuffer.DataSize = sizeof(uint16_t) * kIndexCount;
        
        String name = SUB_MESH_NAME;
        MaterialPtr material = buildMaterial();
        SubMeshPtr submesh = SubMesh::create(name, material->getUUID(), PrimitiveType::kTriangleList, std::move(indexBuffer), true);
        SubMeshes subMeshes;
        subMeshes.emplace(name, submesh);

        auto attributes2 = attributes;
        Buffer vertexBuffer2;
        vertexBuffer2.setData(vertices, sizeof(BoxVertex) * kVertexCount);
        Vertices vertexBuffers2(1);
        vertexBuffers2[0] = vertexBuffer2;

        Buffer indexBuffer2;
        indexBuffer2.setData(indices, sizeof(uint16_t) * kIndexCount);
        
        auto strides2 = strides;
        auto offsets2 = offsets;
        // 复用已有资源的 guid，避免重生成导致引用失效：创建时即传入旧 guid
        UUID cubeUUID = BuiltinGuidUtil::readExistingMetaUUID(path, String(MESH_NAME) + "." + Resource::EXT_MESH + ".meta");
        mMesh = T3D_MESH_MGR.createMesh(MESH_NAME, std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes),
            Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE, "", cubeUUID);

        // 包围体种子：AABB（min/max = center ± extent）
        const Vector3 aabbMin = center - extent;
        const Vector3 aabbMax = center + extent;
        if (mMesh != nullptr)
        {
            mMesh->setBoundSeed(Bound::Type::AABB, aabbMin, aabbMax, Real(0.0));
        }

        material = buildTestMaterial();
        submesh = SubMesh::create(name, material->getUUID(), PrimitiveType::kTriangleList, std::move(indexBuffer2), true);
        SubMeshes subMeshes2;
        subMeshes2.emplace(name, submesh);
        UUID testCubeUUID = BuiltinGuidUtil::readExistingMetaUUID(path, String(TEST_MESH_NAME) + "." + Resource::EXT_MESH + ".meta");
        mTestMesh = T3D_MESH_MGR.createMesh(TEST_MESH_NAME, std::move(attributes2), std::move(vertexBuffers2), std::move(strides2), std::move(offsets2), std::move(subMeshes2),
            Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE, "", testCubeUUID);

        // 包围体种子：AABB（与 cube 相同的几何范围）
        if (mTestMesh != nullptr)
        {
            mTestMesh->setBoundSeed(Bound::Type::AABB, aabbMin, aabbMax, Real(0.0));
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinCube::save(const String &path)
    {
        TResult ret = T3D_OK;

        do
        {
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            T3D_ASSERT(archive != nullptr);

            // Builtin cube mesh file
            if (mMesh != nullptr)
            {
                String filename = String(MESH_NAME) + "." + Resource::EXT_MESH;

                ret = T3D_MESH_MGR.saveMesh(archive, filename, mMesh);
                if (T3D_FAILED(ret))
                {
                    BGEN_LOG_ERROR("Failed to save mesh %s ! ERROR [%d]", filename.c_str(), ret);
                }

                // Builtin cube mesh meta file
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
                    BGEN_LOG_ERROR("Failed to generate meta file (%s) for cube mesh ! ERROR [%d]", filename.c_str(), ret);
                }
            }
            else
            {
                BGEN_LOG_WARNING("Cube mesh is null, skip saving cube mesh !");
            }

            // Builtin cube mesh for testing
            if (mTestMesh != nullptr)
            {
                String filename = String(TEST_MESH_NAME) + "." + Resource::EXT_MESH;

                ret = T3D_MESH_MGR.saveMesh(archive, filename, mTestMesh);
                if (T3D_FAILED(ret))
                {
                    BGEN_LOG_ERROR("Failed to save mesh %s ! ERROR [%d]", filename.c_str(), ret);
                }

                // Builtin cube mesh meta file for testing
                MetaMeshPtr meta = MetaMesh::create(mTestMesh->getUUID());
                filename = filename + ".meta";
                ret = archive->write(filename,
                    [](DataStream &stream, const String &filename, void *userData)
                    {
                        MetaMesh *meta = static_cast<MetaMesh *>(userData);
                        return T3D_SERIALIZER_MGR.serialize(stream, meta);
                    },
                    meta.get());
                if (T3D_FAILED(ret))
                {
                    BGEN_LOG_ERROR("Failed to generate meta file (%s) for testing cube mesh ! ERROR [%d]", filename.c_str(), ret);
                }
            }
            else
            {
                BGEN_LOG_WARNING("Test cube mesh is null, skip saving test cube mesh !");
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
