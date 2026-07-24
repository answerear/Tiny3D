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


#include "T3DBuiltinCylinder.h"
#include "T3DBuiltinGuidUtil.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    namespace
    {
        const char *SUB_MESH_NAME = "#0";
        const char *MESH_NAME = "cylinder";

        // Circumferential segments (around Y).
        const size_t MAX_SEGMENTS = 50;
        const size_t MAX_COLS = MAX_SEGMENTS + 1;

        // Side: top ring + bottom ring. Caps: center + edge ring each.
        const size_t MAX_VERTICES = 2 * MAX_COLS + 2 * (1 + MAX_COLS);
        // Side quads + top fan + bottom fan.
        const size_t MAX_TRIANGLES = MAX_SEGMENTS * 2 + MAX_SEGMENTS + MAX_SEGMENTS;
        const size_t MAX_INDICES = MAX_TRIANGLES * 3;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinCylinder::build(const String &path)
    {
        // Cylinder: radius 0.5, total height 2, Y-up, centered at origin.
        const Real radius = Real(0.5);
        const Real halfHeight = Real(1.0);

        struct CylinderVertex
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

        CylinderVertex *vertices = T3D_POD_NEW_ARRAY(CylinderVertex, MAX_VERTICES);
        uint16_t *indices = T3D_POD_NEW_ARRAY(uint16_t, MAX_INDICES);

        Radian alphaStep(Real(2.0) * Math::PI / Real(MAX_SEGMENTS));

        // Vertex section offsets.
        const size_t sideTopStart = 0;
        const size_t sideBotStart = MAX_COLS;
        const size_t topCenter = 2 * MAX_COLS;
        const size_t topRingStart = topCenter + 1;
        const size_t botCenter = topRingStart + MAX_COLS;
        const size_t botRingStart = botCenter + 1;

        // Side rings (row 0 = top y=+halfHeight, row 1 = bottom y=-halfHeight).
        {
            Radian alpha(0.0);
            for (size_t s = 0; s <= MAX_SEGMENTS; ++s)
            {
                Real cosAlpha = Math::cos(alpha);
                Real sinAlpha = Math::sin(alpha);
                Real u = Real(s) / Real(MAX_SEGMENTS);
                Vector3 radial(cosAlpha, Real(0.0), sinAlpha);

                CylinderVertex &top = vertices[sideTopStart + s];
                top.position = Vector3(radius * cosAlpha, halfHeight, radius * sinAlpha);
                top.normal = radial;
                top.uv = Vector2(u, Real(0.0));

                CylinderVertex &bottom = vertices[sideBotStart + s];
                bottom.position = Vector3(radius * cosAlpha, -halfHeight, radius * sinAlpha);
                bottom.normal = radial;
                bottom.uv = Vector2(u, Real(1.0));

                alpha += alphaStep;
            }
        }

        // Top cap (facing +Y).
        vertices[topCenter].position = Vector3(Real(0.0), halfHeight, Real(0.0));
        vertices[topCenter].normal = Vector3::UP;
        vertices[topCenter].uv = Vector2(Real(0.5), Real(0.5));
        // Bottom cap (facing -Y).
        vertices[botCenter].position = Vector3(Real(0.0), -halfHeight, Real(0.0));
        vertices[botCenter].normal = -Vector3::UP;
        vertices[botCenter].uv = Vector2(Real(0.5), Real(0.5));
        {
            Radian alpha(0.0);
            for (size_t s = 0; s <= MAX_SEGMENTS; ++s)
            {
                Real cosAlpha = Math::cos(alpha);
                Real sinAlpha = Math::sin(alpha);

                CylinderVertex &topRing = vertices[topRingStart + s];
                topRing.position = Vector3(radius * cosAlpha, halfHeight, radius * sinAlpha);
                topRing.normal = Vector3::UP;
                topRing.uv = Vector2(Real(0.5) + Real(0.5) * cosAlpha, Real(0.5) + Real(0.5) * sinAlpha);

                CylinderVertex &botRing = vertices[botRingStart + s];
                botRing.position = Vector3(radius * cosAlpha, -halfHeight, radius * sinAlpha);
                botRing.normal = -Vector3::UP;
                botRing.uv = Vector2(Real(0.5) + Real(0.5) * cosAlpha, Real(0.5) + Real(0.5) * sinAlpha);

                alpha += alphaStep;
            }
        }

        // Indices.
        size_t idx = 0;

        // Side wall (winding matches BuiltinCapsule, outward-facing).
        for (size_t s = 0; s < MAX_SEGMENTS; ++s)
        {
            uint16_t base = static_cast<uint16_t>(sideTopStart + s);
            indices[idx++] = base;
            indices[idx++] = base + 1;
            indices[idx++] = static_cast<uint16_t>(base + MAX_COLS);

            indices[idx++] = base + 1;
            indices[idx++] = static_cast<uint16_t>(base + MAX_COLS + 1);
            indices[idx++] = static_cast<uint16_t>(base + MAX_COLS);
        }

        // Top cap fan (center, ring[s+1], ring[s]) -> normal +Y.
        for (size_t s = 0; s < MAX_SEGMENTS; ++s)
        {
            indices[idx++] = static_cast<uint16_t>(topCenter);
            indices[idx++] = static_cast<uint16_t>(topRingStart + s + 1);
            indices[idx++] = static_cast<uint16_t>(topRingStart + s);
        }

        // Bottom cap fan (center, ring[s], ring[s+1]) -> normal -Y.
        for (size_t s = 0; s < MAX_SEGMENTS; ++s)
        {
            indices[idx++] = static_cast<uint16_t>(botCenter);
            indices[idx++] = static_cast<uint16_t>(botRingStart + s);
            indices[idx++] = static_cast<uint16_t>(botRingStart + s + 1);
        }

        T3D_ASSERT(idx == MAX_INDICES);

        // construct mesh resource
        Buffer vertexBuffer;
        vertexBuffer.Data = (uint8_t *)vertices;
        vertexBuffer.DataSize = sizeof(CylinderVertex) * MAX_VERTICES;
        Vertices vertexBuffers(1);
        vertexBuffers[0] = vertexBuffer;

        VertexStrides strides(1, sizeof(CylinderVertex));
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
        UUID cylinderUUID = BuiltinGuidUtil::readExistingMetaUUID(path, String(MESH_NAME) + "." + Resource::EXT_MESH + ".meta");
        mMesh = T3D_MESH_MGR.createMesh(MESH_NAME, std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes),
            Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE, "", cylinderUUID);

        // 包围体种子：Capsule（中轴端点 (0, ±0.5, 0)，半径 0.5，胶囊总高与圆柱一致）
        if (mMesh != nullptr)
        {
            mMesh->setBoundSeed(Bound::Type::CAPSULE, Vector3(Real(0.0), Real(0.5), Real(0.0)),
                Vector3(Real(0.0), Real(-0.5), Real(0.0)), radius);
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinCylinder::save(const String &path)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mMesh == nullptr)
            {
                BGEN_LOG_WARNING("Cylinder mesh is null, skip saving cylinder mesh !");
                break;
            }

            // Builtin cylinder mesh file
            String filename = String(MESH_NAME) + "." + Resource::EXT_MESH;
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            T3D_ASSERT(archive != nullptr);

            ret = T3D_MESH_MGR.saveMesh(archive, filename, mMesh);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to save mesh %s ! ERROR [%d]", filename.c_str(), ret);
                break;
            }

            // Builtin cylinder mesh meta file
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
                BGEN_LOG_ERROR("Failed to generate meta file (%s) for cylinder mesh ! ERROR [%d]", filename.c_str(), ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
}
