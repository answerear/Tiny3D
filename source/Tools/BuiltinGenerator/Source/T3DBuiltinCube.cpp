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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char *SUB_MESH_NAME = "#0";
#if !defined (GENERATE_TEST_MESH)
    const char *MESH_NAME = "cube.tmesh";
#else
    const char *MESH_NAME = "test-cube.tmesh";
#endif
    
    //--------------------------------------------------------------------------

    TResult BuiltinCube::build()
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
            Vector2 uv {};
        };
        
        // vertex attributes
        VertexAttribute attrPos(0, 0, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
        VertexAttribute attrUV(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, 0);
        VertexAttributes attributes(2);
        attributes[0] = attrPos;
        attributes[1] = attrUV;

        // vertices & indices
        Vector3 offset;
        Vector3 center(0.0f, 0.0f, 0.0f);
        Vector3 extent(0.5f, 0.5f, 0.5f);

        const uint32_t kVertexCount = 24;
        const uint32_t kIndexCount = 36;
        BoxVertex *vertices = new BoxVertex[kVertexCount];
        uint16_t *indices = new uint16_t[kIndexCount];

        // front - V0
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[0].position = center + offset;
        vertices[0].uv = Vector2(0.0f, 0.0f);
        
        // front - V1
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[1].position = center + offset;
        vertices[1].uv = Vector2(0.0f, 0.5f);
        
        // front - V2
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[2].position = center + offset;
        vertices[2].uv = Vector2(0.5f, 0.0f);
        
        // front - V3
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[3].position = center + offset;
        vertices[3].uv = Vector2(0.5f, 0.5f);

        // right - V2
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[4].position = center + offset;
        vertices[4].uv = Vector2(0.0f, 0.5f);
        
        // right - V3
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[5].position = center + offset;
        vertices[5].uv = Vector2(0.0f, 1.0f);
        
        // right - V4
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[6].position = center + offset;
        vertices[6].uv = Vector2(0.5f, 0.5f);
        
        // right - V5
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[7].position = center + offset;
        vertices[7].uv = Vector2(0.5f, 1.0f);

        // back - V4
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[8].position = center + offset;
        vertices[8].uv = Vector2(0.0f, 0.0f);
        
        // back - V5
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[9].position = center + offset;
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
        vertices[11].uv = Vector2(0.5f, 0.5f);
        
        // left - V6
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[12].position = center + offset;
        vertices[12].uv = Vector2(0.5f, 0.5f);
        
        // left - V7
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[13].position = center + offset;
        vertices[13].uv = Vector2(0.5f, 1.0f);

        // left - V0
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[14].position = center + offset;
        vertices[14].uv = Vector2(1.0f, 0.5f);
        
        // left - V1
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[15].position = center + offset;
        vertices[15].uv = Vector2(1.0f, 1.0f);

        // top - V0
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[16].position = center + offset;
        vertices[16].uv = Vector2(0.5f, 0.5f);
        
        // top - V2
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[17].position = center + offset;
        vertices[17].uv = Vector2(1.0f, 0.5f);

        // top - V4
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[18].position = center + offset;
        vertices[18].uv = Vector2(1.0f, 0.0f);

        // top - V6
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[19].position = center + offset;
        vertices[19].uv = Vector2(0.5f, 0.0f);

        // bottom - V1
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[20].position = center + offset;
        vertices[20].uv = Vector2(0.5f, 0.5f);

        // bottom - V7
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[21].position = center + offset;
        vertices[21].uv = Vector2(0.5f, 1.0f);

        // bottom - V3
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[22].position = center + offset;
        vertices[22].uv = Vector2(1.0f, 0.5f);
        
        // bottom - V5
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[23].position = center + offset;
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
        SubMeshPtr submesh = SubMesh::create(name, material, PrimitiveType::kTriangleList, indexBuffer, true);
        SubMeshes subMeshes;
        subMeshes.emplace(name, submesh);

        auto attributes2 = attributes;
        auto vertexBuffers2 = vertexBuffers;
        auto strides2 = strides;
        auto offsets2 = offsets;
        auto subMeshes2 = subMeshes;
        mMesh = T3D_MESH_MGR.createMesh(MESH_NAME, std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes));
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinCube::save(const String &path)
    {
        TResult ret = T3D_OK;

        do
        {
            String filename = path + Dir::getNativeSeparator() + MESH_NAME;
            FileDataStream fs;
            if (!fs.open(filename.c_str(), FileDataStream::EOpenMode::E_MODE_TRUNCATE|FileDataStream::EOpenMode::E_MODE_WRITE_ONLY|FileDataStream::EOpenMode::E_MODE_TEXT))
            {
                BGEN_LOG_ERROR("Open file %s failed !", filename.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            T3D_SERIALIZER_MGR.serialize<Mesh>(fs, *mMesh);
            fs.close();
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
