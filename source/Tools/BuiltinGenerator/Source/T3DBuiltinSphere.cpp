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


#include "T3DBuiltinSphere.h"
#include "T3DBuiltinGuidUtil.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    namespace
    {
        const char *SUB_MESH_NAME = "#0";
        const char *MESH_NAME = "sphere";

        const size_t MAX_STACKS = 50;
        const size_t MAX_SLICES = 50;
        const size_t MAX_VERTICES = (MAX_STACKS + 1) * (MAX_SLICES + 1);
        const size_t MAX_TRIANGLES = MAX_STACKS * MAX_SLICES * 2;
        const size_t MAX_INDICES = MAX_TRIANGLES * 3;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinSphere::build(const String &path)
    {
        // Diameter = 1, center at model origin.
        const Real radius = Real(0.5);

        struct SphereVertex
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

        SphereVertex *vertices = T3D_POD_NEW_ARRAY(SphereVertex, MAX_VERTICES);
        uint16_t *indices = T3D_POD_NEW_ARRAY(uint16_t, MAX_INDICES);

        // Setup sphere vertices (Globe::setupSphere algorithm).
        Radian alphaStep(Real(2.0) * Math::PI / MAX_STACKS);
        Radian betaStep(Math::PI / MAX_SLICES);
        Radian alpha(0.0);
        Radian beta;

        int32_t i = 0, j = 0, idx = 0;

        for (i = 0; i <= static_cast<int32_t>(MAX_STACKS); ++i)
        {
            Real sinAlpha = Math::sin(alpha);
            Real cosAlpha = Math::cos(alpha);
            beta = -Math::PI * REAL_HALF;

            for (j = 0; j <= static_cast<int32_t>(MAX_SLICES); ++j, ++idx)
            {
                Real sinBeta = Math::sin(beta);
                Real cosBeta = Math::cos(beta);

                vertices[idx].position[0] = radius * cosBeta * cosAlpha;
                vertices[idx].position[1] = radius * sinBeta;
                vertices[idx].position[2] = radius * cosBeta * sinAlpha;

                vertices[idx].normal = vertices[idx].position;
                vertices[idx].normal.normalize();

                vertices[idx].uv = Vector2(
                    Real(i) / Real(MAX_STACKS),
                    Real(j) / Real(MAX_SLICES));

                beta += betaStep;
            }

            alpha += alphaStep;
        }

        // Setup sphere indices.
        uint16_t base = 0;
        idx = 0;

        for (i = 0; i < static_cast<int32_t>(MAX_STACKS); ++i)
        {
            for (j = 0; j < static_cast<int32_t>(MAX_SLICES); ++j)
            {
                indices[idx++] = base;
                indices[idx++] = base + 1;
                indices[idx++] = static_cast<uint16_t>(base + MAX_SLICES + 1);

                indices[idx++] = base + 1;
                indices[idx++] = static_cast<uint16_t>(base + MAX_SLICES + 2);
                indices[idx++] = static_cast<uint16_t>(base + MAX_SLICES + 1);

                base++;
            }

            base++;
        }

        // construct mesh resource
        Buffer vertexBuffer;
        vertexBuffer.Data = (uint8_t *)vertices;
        vertexBuffer.DataSize = sizeof(SphereVertex) * MAX_VERTICES;
        Vertices vertexBuffers(1);
        vertexBuffers[0] = vertexBuffer;

        VertexStrides strides(1, sizeof(SphereVertex));
        VertexOffsets offsets(1, 0);

        Buffer indexBuffer;
        indexBuffer.Data = (uint8_t *)indices;
        indexBuffer.DataSize = sizeof(uint16_t) * MAX_INDICES;

        String name = SUB_MESH_NAME;
        MaterialPtr material = buildMaterial();
        SubMeshPtr submesh = SubMesh::create(name, material->getUUID(), PrimitiveType::kTriangleList, std::move(indexBuffer), true);
        SubMeshes subMeshes;
        subMeshes.emplace(name, submesh);

        // 复用已有资源的 guid，避免重生成导致引用失效：创建时即传入旧 guid
        UUID sphereUUID = BuiltinGuidUtil::readExistingMetaUUID(path, String(MESH_NAME) + "." + Resource::EXT_MESH + ".meta");
        mMesh = T3D_MESH_MGR.createMesh(MESH_NAME, std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes),
            Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE, "", sphereUUID);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinSphere::save(const String &path)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mMesh == nullptr)
            {
                BGEN_LOG_WARNING("Sphere mesh is null, skip saving sphere mesh !");
                break;
            }

            // Builtin sphere mesh file
            String filename = String(MESH_NAME) + "." + Resource::EXT_MESH;
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            T3D_ASSERT(archive != nullptr);

            ret = T3D_MESH_MGR.saveMesh(archive, filename, mMesh);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to save mesh %s ! ERROR [%d]", filename.c_str(), ret);
                break;
            }

            // Builtin sphere mesh meta file
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
                BGEN_LOG_ERROR("Failed to generate meta file (%s) for sphere mesh ! ERROR [%d]", filename.c_str(), ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
}
