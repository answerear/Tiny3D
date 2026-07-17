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


#include "T3DBuiltinCapsule.h"
#include "T3DBuiltinGuidUtil.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    namespace
    {
        const char *SUB_MESH_NAME = "#0";
        const char *MESH_NAME = "capsule";

        // Circumferential segments (around Y), hemisphere stacks, cylinder stacks.
        const size_t MAX_SEGMENTS = 50;
        const size_t MAX_HEMI_STACKS = 16;
        const size_t MAX_CYL_STACKS = 8;

        // Top hemi (pole..equator) + cylinder (excl. top equator, incl. bottom equator) + bottom hemi (excl. equator, incl. pole).
        const size_t MAX_ROWS = MAX_HEMI_STACKS + MAX_CYL_STACKS + MAX_HEMI_STACKS + 1;
        const size_t MAX_COLS = MAX_SEGMENTS + 1;
        const size_t MAX_VERTICES = MAX_ROWS * MAX_COLS;
        const size_t MAX_TRIANGLES = (MAX_ROWS - 1) * MAX_SEGMENTS * 2;
        const size_t MAX_INDICES = MAX_TRIANGLES * 3;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinCapsule::build(const String &path)
    {
        // Unity-style capsule: radius 0.5, total height 2 (including hemispheres), Y-up, centered at origin.
        const Real radius = Real(0.5);
        const Real totalHeight = Real(2.0);
        const Real cylinderHeight = totalHeight - Real(2.0) * radius;
        const Real cylinderHalfH = cylinderHeight * REAL_HALF;

        struct CapsuleVertex
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

        CapsuleVertex *vertices = T3D_POD_NEW_ARRAY(CapsuleVertex, MAX_VERTICES);
        uint16_t *indices = T3D_POD_NEW_ARRAY(uint16_t, MAX_INDICES);

        const Vector3 topCenter(0.0f, cylinderHalfH, 0.0f);
        const Vector3 bottomCenter(0.0f, -cylinderHalfH, 0.0f);

        Radian alphaStep(Real(2.0) * Math::PI / Real(MAX_SEGMENTS));
        int32_t idx = 0;

        auto emitHemiRing = [&](Real y, Real radial, const Vector3 &hemiCenter, Real v)
        {
            Radian alpha(0.0);
            for (size_t s = 0; s <= MAX_SEGMENTS; ++s, ++idx)
            {
                Real cosAlpha = Math::cos(alpha);
                Real sinAlpha = Math::sin(alpha);

                vertices[idx].position[0] = radial * cosAlpha;
                vertices[idx].position[1] = y;
                vertices[idx].position[2] = radial * sinAlpha;

                vertices[idx].normal = vertices[idx].position - hemiCenter;
                vertices[idx].normal.normalize();

                vertices[idx].uv = Vector2(Real(s) / Real(MAX_SEGMENTS), v);

                alpha += alphaStep;
            }
        };

        // Top hemisphere: pole (beta = +PI/2) -> equator (beta = 0), center at topCenter.
        for (size_t i = 0; i <= MAX_HEMI_STACKS; ++i)
        {
            Real t = Real(i) / Real(MAX_HEMI_STACKS);
            Radian beta(Math::PI * REAL_HALF * (REAL_ONE - t));
            Real sinBeta = Math::sin(beta);
            Real cosBeta = Math::cos(beta);
            Real y = topCenter.y() + radius * sinBeta;
            Real radial = radius * cosBeta;
            Real v = Real(i) / Real(MAX_ROWS - 1);
            emitHemiRing(y, radial, topCenter, v);
        }

        // Cylinder: exclude top equator (already emitted), include bottom equator.
        for (size_t i = 1; i <= MAX_CYL_STACKS; ++i)
        {
            Real t = Real(i) / Real(MAX_CYL_STACKS);
            Real y = cylinderHalfH - t * cylinderHeight;
            Real v = Real(MAX_HEMI_STACKS + i) / Real(MAX_ROWS - 1);

            Radian alpha(0.0);
            for (size_t s = 0; s <= MAX_SEGMENTS; ++s, ++idx)
            {
                Real cosAlpha = Math::cos(alpha);
                Real sinAlpha = Math::sin(alpha);

                vertices[idx].position[0] = radius * cosAlpha;
                vertices[idx].position[1] = y;
                vertices[idx].position[2] = radius * sinAlpha;

                vertices[idx].normal = Vector3(cosAlpha, Real(0.0), sinAlpha);

                vertices[idx].uv = Vector2(Real(s) / Real(MAX_SEGMENTS), v);

                alpha += alphaStep;
            }
        }

        // Bottom hemisphere: exclude equator (already emitted), include pole (beta = -PI/2).
        for (size_t i = 1; i <= MAX_HEMI_STACKS; ++i)
        {
            Real t = Real(i) / Real(MAX_HEMI_STACKS);
            Radian beta(-Math::PI * REAL_HALF * t);
            Real sinBeta = Math::sin(beta);
            Real cosBeta = Math::cos(beta);
            Real y = bottomCenter.y() + radius * sinBeta;
            Real radial = radius * cosBeta;
            Real v = Real(MAX_HEMI_STACKS + MAX_CYL_STACKS + i) / Real(MAX_ROWS - 1);
            emitHemiRing(y, radial, bottomCenter, v);
        }

        T3D_ASSERT(static_cast<size_t>(idx) == MAX_VERTICES);

        // Indices (winding matches BuiltinSphere).
        uint16_t base = 0;
        idx = 0;
        for (size_t r = 0; r < MAX_ROWS - 1; ++r)
        {
            for (size_t s = 0; s < MAX_SEGMENTS; ++s)
            {
                indices[idx++] = base;
                indices[idx++] = base + 1;
                indices[idx++] = static_cast<uint16_t>(base + MAX_COLS);

                indices[idx++] = base + 1;
                indices[idx++] = static_cast<uint16_t>(base + MAX_COLS + 1);
                indices[idx++] = static_cast<uint16_t>(base + MAX_COLS);

                base++;
            }

            base++;
        }

        T3D_ASSERT(static_cast<size_t>(idx) == MAX_INDICES);

        // construct mesh resource
        Buffer vertexBuffer;
        vertexBuffer.Data = (uint8_t *)vertices;
        vertexBuffer.DataSize = sizeof(CapsuleVertex) * MAX_VERTICES;
        Vertices vertexBuffers(1);
        vertexBuffers[0] = vertexBuffer;

        VertexStrides strides(1, sizeof(CapsuleVertex));
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
        UUID capsuleUUID = BuiltinGuidUtil::readExistingMetaUUID(path, String(MESH_NAME) + "." + Resource::EXT_MESH + ".meta");
        mMesh = T3D_MESH_MGR.createMesh(MESH_NAME, std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes),
            Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE, "", capsuleUUID);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinCapsule::save(const String &path)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mMesh == nullptr)
            {
                BGEN_LOG_WARNING("Capsule mesh is null, skip saving capsule mesh !");
                break;
            }

            // Builtin capsule mesh file
            String filename = String(MESH_NAME) + "." + Resource::EXT_MESH;
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            T3D_ASSERT(archive != nullptr);

            ret = T3D_MESH_MGR.saveMesh(archive, filename, mMesh);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to save mesh %s ! ERROR [%d]", filename.c_str(), ret);
                break;
            }

            // Builtin capsule mesh meta file
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
                BGEN_LOG_ERROR("Failed to generate meta file (%s) for capsule mesh ! ERROR [%d]", filename.c_str(), ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
}
