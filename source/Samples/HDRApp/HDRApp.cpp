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

#include "HDRApp.h"
#include "HDRShaderSources.h"
#include "HDRBloomTonemapBehaviour.h"
#include "StreetCameraBehaviour.h"
#include "NeonFlickerBehaviour.h"
#include "Kernel/T3DPixelFormat.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHICapabilities.h"

#include <cmath>
#include <cstring>


using namespace Tiny3D;

HDRApp theApp;

namespace
{
    const char *SUB_MESH_NAME = "#0";

    // Street layout, left-handed : +X to the right of the street, +Y up, +Z down the street
    const Real kRoadHalfWidth = 4.0f;
    const Real kFacadeX = 6.0f;
    const Real kStreetStart = -12.0f;
    const Real kStreetEnd = 70.0f;
    const Real kSidewalkHeight = 0.15f;

    const ColorRGB kSodiumColor(1.0f, 0.66f, 0.34f);

    struct Vertex
    {
        Vector3 position;
        Vector3 normal;
        Vector2 uv;
    };

    uint32_t hash32(uint32_t x)
    {
        x ^= x >> 16;
        x *= 0x7feb352dU;
        x ^= x >> 15;
        x *= 0x846ca68bU;
        x ^= x >> 16;
        return x;
    }

    Real hashUnit(uint32_t x)
    {
        return static_cast<Real>(hash32(x) & 0xFFFFFF) / static_cast<Real>(0xFFFFFF);
    }

    uint8_t toByte(Real v)
    {
        return static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, v)));
    }

    uint16_t floatToHalf(float value)
    {
        uint32_t bits = 0;
        memcpy(&bits, &value, sizeof(bits));
        const uint32_t sign = (bits >> 16) & 0x8000U;
        const int32_t exponent = static_cast<int32_t>((bits >> 23) & 0xFF) - 127 + 15;
        const uint32_t mantissa = bits & 0x7FFFFFU;
        if (exponent <= 0)
        {
            return static_cast<uint16_t>(sign);
        }
        if (exponent >= 31)
        {
            return static_cast<uint16_t>(sign | 0x7C00U);
        }
        return static_cast<uint16_t>(sign | (static_cast<uint32_t>(exponent) << 10) | (mantissa >> 13));
    }

    Real absDot(const Vector3 &axis, const Vector3 &halfSize)
    {
        return std::fabs(axis.x()) * halfSize.x() + std::fabs(axis.y()) * halfSize.y() + std::fabs(axis.z()) * halfSize.z();
    }

    /// Collects quads and boxes in world space into one mesh per material
    class MeshBuilder
    {
    public:
        /// tile > 0 maps uv from world position (one texture repeat per tile metres), otherwise the face gets 0..1
        void addFace(const Vector3 &center, const Vector3 &normal, const Vector3 &right, const Vector3 &up, Real halfRight, Real halfUp, Real tile)
        {
            const Vector3 corners[4] =
            {
                center - right * halfRight + up * halfUp,   // top left
                center + right * halfRight + up * halfUp,   // top right
                center - right * halfRight - up * halfUp,   // bottom left
                center + right * halfRight - up * halfUp,   // bottom right
            };
            const Vector2 unitUV[4] = { Vector2(0, 0), Vector2(1, 0), Vector2(0, 1), Vector2(1, 1) };

            const uint16_t base = static_cast<uint16_t>(mVertices.size());
            for (int32_t i = 0; i < 4; ++i)
            {
                Vertex v;
                v.position = corners[i];
                v.normal = normal;
                v.uv = (tile > 0.0f)
                    ? Vector2(corners[i].dot(right) / tile, -corners[i].dot(up) / tile)
                    : unitUV[i];
                mVertices.push_back(v);
                grow(corners[i]);
            }

            // Clockwise seen from the front, same winding as the other samples
            const uint16_t indices[6] = { 0, 1, 2, 2, 1, 3 };
            for (uint16_t idx : indices)
            {
                mIndices.push_back(static_cast<uint16_t>(base + idx));
            }
        }

        void addBox(const Vector3 &minPos, const Vector3 &maxPos, Real tile)
        {
            const Vector3 center = (minPos + maxPos) * 0.5f;
            const Vector3 half = (maxPos - minPos) * 0.5f;

            struct Face { Vector3 n, r, u; };
            static const Face kFaces[6] =
            {
                { Vector3( 0, 0,-1), Vector3( 1, 0, 0), Vector3(0, 1, 0) },
                { Vector3( 0, 0, 1), Vector3(-1, 0, 0), Vector3(0, 1, 0) },
                { Vector3( 1, 0, 0), Vector3( 0, 0, 1), Vector3(0, 1, 0) },
                { Vector3(-1, 0, 0), Vector3( 0, 0,-1), Vector3(0, 1, 0) },
                { Vector3( 0, 1, 0), Vector3( 1, 0, 0), Vector3(0, 0, 1) },
                { Vector3( 0,-1, 0), Vector3( 1, 0, 0), Vector3(0, 0,-1) },
            };

            for (const Face &face : kFaces)
            {
                addFace(center + face.n * absDot(face.n, half), face.n, face.r, face.u,
                    absDot(face.r, half), absDot(face.u, half), tile);
            }
        }

        MeshPtr build(const String &name, const Tiny3D::UUID &materialUUID) const
        {
            T3D_ASSERT(mVertices.size() <= 0xFFFF);

            VertexAttributes attributes(3);
            attributes[0] = VertexAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
            attributes[1] = VertexAttribute(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_NORMAL, 0);
            attributes[2] = VertexAttribute(0, sizeof(Vector3) * 2, VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, 0);

            Buffer vertexBuffer;
            vertexBuffer.DataSize = sizeof(Vertex) * mVertices.size();
            vertexBuffer.Data = T3D_POD_NEW_ARRAY(uint8_t, vertexBuffer.DataSize);
            memcpy(vertexBuffer.Data, mVertices.data(), vertexBuffer.DataSize);
            Vertices vertexBuffers(1);
            vertexBuffers[0] = vertexBuffer;

            VertexStrides strides(1, sizeof(Vertex));
            VertexOffsets offsets(1, 0);

            Buffer indexBuffer;
            indexBuffer.DataSize = sizeof(uint16_t) * mIndices.size();
            indexBuffer.Data = T3D_POD_NEW_ARRAY(uint8_t, indexBuffer.DataSize);
            memcpy(indexBuffer.Data, mIndices.data(), indexBuffer.DataSize);

            String subName = SUB_MESH_NAME;
            SubMeshPtr submesh = SubMesh::create(subName, materialUUID, PrimitiveType::kTriangleList, std::move(indexBuffer), true);
            SubMeshes subMeshes;
            subMeshes.emplace(subName, submesh);

            return T3D_MESH_MGR.createMesh(name, std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes));
        }

        const Vector3 &getMin() const { return mMin; }
        const Vector3 &getMax() const { return mMax; }

    private:
        void grow(const Vector3 &p)
        {
            if (mVertices.size() == 1)
            {
                mMin = mMax = p;
                return;
            }
            mMin = Vector3(std::min(mMin.x(), p.x()), std::min(mMin.y(), p.y()), std::min(mMin.z(), p.z()));
            mMax = Vector3(std::max(mMax.x(), p.x()), std::max(mMax.y(), p.y()), std::max(mMax.z(), p.z()));
        }

        TArray<Vertex> mVertices;
        TArray<uint16_t> mIndices;
        Vector3 mMin {Vector3::ZERO};
        Vector3 mMax {Vector3::ZERO};
    };

    /// A run of buildings along one side of the street
    struct Block
    {
        Real zStart;
        Real length;
        Real height;
    };

    const Block kRightBlocks[] =
    {
        { -12.0f, 10.0f,  9.0f }, { -2.0f,  9.0f, 12.5f }, {  7.0f, 11.0f,  7.5f }, { 18.0f,  8.0f, 14.0f },
        {  26.0f, 12.0f,  8.5f }, { 38.0f,  9.0f, 11.0f }, { 47.0f, 11.0f,  9.5f }, { 58.0f, 12.0f, 13.0f },
    };

    const Block kLeftBlocks[] =
    {
        { -12.0f,  9.0f,  8.0f }, { -3.0f, 12.0f, 13.0f }, {  9.0f,  8.0f,  7.0f }, { 17.0f, 11.0f, 10.0f },
        {  28.0f,  9.0f, 12.5f }, { 37.0f, 10.0f,  8.0f }, { 47.0f, 11.0f, 11.5f }, { 58.0f, 12.0f,  9.0f },
    };

    struct LampPost
    {
        Real side;  // +1 right, -1 left
        Real z;
    };

    const LampPost kLamps[] =
    {
        {  1.0f,  8.0f }, { -1.0f, 20.0f }, {  1.0f, 32.0f }, { -1.0f, 44.0f },
    };

    struct Segment
    {
        Real x0, y0, x1, y1;
    };

    void addPolyline(TArray<Segment> &segments, const TArray<Vector2> &points, bool closed)
    {
        const size_t count = points.size();
        for (size_t i = 0; i + 1 < count + (closed ? 1 : 0); ++i)
        {
            const Vector2 &a = points[i];
            const Vector2 &b = points[(i + 1) % count];
            segments.push_back({ a.x(), a.y(), b.x(), b.y() });
        }
    }

    void addCircle(TArray<Segment> &segments, Real cx, Real cy, Real radius)
    {
        TArray<Vector2> points;
        const int32_t kSteps = 24;
        for (int32_t i = 0; i < kSteps; ++i)
        {
            const Real a = Math::TWO_PI * static_cast<Real>(i) / static_cast<Real>(kSteps);
            points.push_back(Vector2(cx + radius * std::cos(a), cy + radius * std::sin(a)));
        }
        addPolyline(segments, points, true);
    }

    Real distanceToSegment(Real px, Real py, const Segment &s)
    {
        const Real dx = s.x1 - s.x0;
        const Real dy = s.y1 - s.y0;
        const Real len2 = dx * dx + dy * dy;
        Real t = (len2 > 0.0f) ? ((px - s.x0) * dx + (py - s.y0) * dy) / len2 : 0.0f;
        t = std::max(0.0f, std::min(1.0f, t));
        const Real ex = px - (s.x0 + t * dx);
        const Real ey = py - (s.y0 + t * dy);
        return std::sqrt(ex * ex + ey * ey);
    }
}

//------------------------------------------------------------------------------

HDRApp::HDRApp()
{
}

HDRApp::~HDRApp()
{
}

TResult HDRApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    ScenePtr scene = T3D_SCENE_MGR.createScene("HDRStreet");
    scene->init();
    T3D_SCENE_MGR.setCurrentScene(scene);

    // Faint blue night sky fill, so brick and mortar still read in the shadows between lamps
    AmbientLightPtr ambient = scene->getRootGameObject()->addComponent<AmbientLight>();
    ambient->setColor(ColorRGB(0.45f, 0.55f, 0.85f));
    ambient->setIntensity(0.35f);

    GameObjectPtr go = GameObject::create("HDRStreet");
    Transform3DPtr root = go->addComponent<Transform3D>();
    scene->getRootTransform()->addChild(root);

    mLitShader = buildLitShader();
    mEmissiveShader = buildEmissiveShader();
    if (mLitShader == nullptr || mEmissiveShader == nullptr)
    {
        T3D_LOG_ERROR(LOG_TAG_APP, "HDRApp : no embedded shaders for this renderer.");
        return T3D_ERR_NOT_IMPLEMENT;
    }

    buildCamera(root);
    buildLights(root);
    buildStreet(root);
    buildSigns(root);

    return T3D_OK;
}

void HDRApp::applicationWillTerminate()
{
    mMeshes.clear();
    mMaterials.clear();
    mTextures.clear();
    mLitShader = nullptr;
    mEmissiveShader = nullptr;
}

//------------------------------------------------------------------------------

void HDRApp::buildCamera(Transform3D *parent)
{
    RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
    RenderTargetPtr rt = RenderTarget::create(rw);

    GameObjectPtr go = GameObject::create("Camera");
    Transform3DPtr xform = go->addComponent<Transform3D>();
    parent->addChild(xform);

    CameraPtr camera = go->addComponent<Camera>();
    camera->setOrder(0);
    Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    camera->setViewport(vp);
    camera->setClearColor(ColorRGB(0.035f, 0.05f, 0.11f));
    // Before setRenderTarget, so the window buffer is created as floating point right away
    camera->setAllowHDR(true);
    camera->setRenderTarget(rt);

    camera->setProjectionType(Camera::Projection::kPerspective);
    camera->setFovY(Radian(Math::PI / 3.0f));
    Real as = Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
    camera->setAspectRatio(as);
    camera->setNearPlaneDistance(0.1f);
    camera->setFarPlaneDistance(200.0f);
    camera->lookAt(Vector3(0.0f, 1.7f, -4.0f), Vector3(0.0f, 2.0f, 10.0f), Vector3::UP);

    RenderTexturePtr colorRT = camera->getRenderTexture();
    if (colorRT != nullptr)
    {
        const PixelFormat format = colorRT->getPixelFormat();
        if (PixelFormatUtil::isFloatFormat(format))
        {
            T3D_LOG_INFO(LOG_TAG_APP, "HDRApp : camera renders into %s, values above 1.0 survive until tone mapping.", PixelFormatUtil::getName(format));
        }
        else
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "HDRApp : camera fell back to %s, lamp and neon cores will clip to white.", PixelFormatUtil::getName(format));
        }
    }

    go->addComponent<FrustumBound>();

    auto path = go->addComponent<StreetCameraBehaviour>();
    path->setSegmentSeconds(4.5f);
    path->addWaypoint(Vector3( 0.0f, 1.7f, -5.0f), Vector3( 0.0f, 2.2f, 10.0f));
    path->addWaypoint(Vector3( 0.6f, 1.7f,  5.0f), Vector3( 5.0f, 3.0f, 15.0f));
    path->addWaypoint(Vector3( 3.0f, 1.6f, 10.5f), Vector3( 6.0f, 2.2f, 13.0f));
    path->addWaypoint(Vector3( 1.0f, 1.8f, 19.0f), Vector3(-5.5f, 4.0f, 26.0f));
    path->addWaypoint(Vector3(-1.5f, 1.7f, 31.0f), Vector3( 4.5f, 3.0f, 40.0f));
    path->addWaypoint(Vector3(-2.5f, 2.0f, 42.0f), Vector3(-5.5f, 3.0f, 50.0f));
    path->addWaypoint(Vector3( 0.0f, 3.0f, 54.0f), Vector3( 0.0f, 2.0f, 66.0f));
    path->addWaypoint(Vector3( 0.0f, 7.0f, 64.0f), Vector3( 0.0f, 1.0f, 36.0f));
    path->addWaypoint(Vector3( 0.0f, 5.0f, 34.0f), Vector3( 0.0f, 1.5f,  6.0f));
    path->addWaypoint(Vector3( 0.0f, 3.0f,  8.0f), Vector3( 0.0f, 1.5f, -8.0f));

    auto effect = go->addComponent<HDRBloomTonemapBehaviour>();
    effect->setExposure(1.6f);
    effect->setThreshold(1.0f);
    effect->setBloomIntensity(0.7f);
}

//------------------------------------------------------------------------------

void HDRApp::buildLights(Transform3D *parent)
{
    // Lamps point straight down : the spot direction is the local +Z axis
    Quaternion down;
    down.fromAngleAxis(Radian(Math::PI * 0.5f), Vector3::UNIT_X);

    int32_t index = 0;
    for (const LampPost &lamp : kLamps)
    {
        GameObjectPtr go = GameObject::create("LampLight#" + std::to_string(index++));
        Transform3DPtr node = go->addComponent<Transform3D>();
        node->setPosition(Vector3(lamp.side * 3.55f, 4.55f, lamp.z));
        node->setOrientation(down);
        parent->addChild(node);

        SpotLightPtr light = go->addComponent<SpotLight>();
        light->setColor(kSodiumColor);
        light->setDiffuseIntensity(5.0f);
        light->setSpecularIntensity(3.0f);
        light->setAttenuationConstant(1.0f);
        light->setAttenuationLinear(0.09f);
        light->setAttenuationQuadratic(0.06f);
        light->setCutoffAngle(Degree(115.0f));
        light->setInnerCutoffAngle(Degree(50.0f));
    }
}

//------------------------------------------------------------------------------

void HDRApp::buildStreet(Transform3D *parent)
{
    Texture2DPtr redBrick = buildBrickTexture("texBrickRed", 128, 52, 40, 11);
    Texture2DPtr brownBrick = buildBrickTexture("texBrickBrown", 100, 70, 55, 23);
    Texture2DPtr asphalt = buildAsphaltTexture();
    Texture2DPtr pavement = buildPavementTexture();
    Texture2DPtr metal = buildSolidTexture("texMetal", 58, 60, 64);
    Texture2DPtr paint = buildSolidTexture("texLanePaint", 190, 188, 175);
    Texture2DPtr white = buildSolidTexture("texWhite", 255, 255, 255);
    Texture2DPtr window = buildWindowTexture();

    MaterialPtr redBrickMat = buildLitMaterial("matBrickRed", redBrick, 0.15f);
    MaterialPtr brownBrickMat = buildLitMaterial("matBrickBrown", brownBrick, 0.15f);
    MaterialPtr asphaltMat = buildLitMaterial("matAsphalt", asphalt, 0.8f);
    MaterialPtr pavementMat = buildLitMaterial("matPavement", pavement, 0.45f);
    MaterialPtr metalMat = buildLitMaterial("matMetal", metal, 0.55f);
    MaterialPtr paintMat = buildLitMaterial("matLanePaint", paint, 0.5f);
    // Sodium bulbs sit several times above white, lit windows only just above it.
    // A saturated amber keeps the bulb core yellow after ACES instead of flat white.
    MaterialPtr bulbMat = buildEmissiveMaterial("matBulb", white, ColorRGBA(1.0f, 0.55f, 0.18f, 4.0f));
    MaterialPtr windowMat = buildEmissiveMaterial("matWindow", window, ColorRGBA(1.0f, 0.8f, 0.55f, 1.4f));

    MeshBuilder redBricks, brownBricks, road, sidewalk, lanes, metalParts, bulbs, windows;

    // Road, sidewalks running under the facades, centre line dashes
    road.addBox(Vector3(-kRoadHalfWidth, -0.1f, kStreetStart), Vector3(kRoadHalfWidth, 0.0f, kStreetEnd), 4.0f);
    sidewalk.addBox(Vector3(kRoadHalfWidth, -0.1f, kStreetStart), Vector3(kFacadeX + 2.0f, kSidewalkHeight, kStreetEnd), 2.0f);
    sidewalk.addBox(Vector3(-kFacadeX - 2.0f, -0.1f, kStreetStart), Vector3(-kRoadHalfWidth, kSidewalkHeight, kStreetEnd), 2.0f);
    for (Real z = kStreetStart + 1.0f; z < kStreetEnd - 2.0f; z += 4.5f)
    {
        lanes.addBox(Vector3(-0.07f, 0.0f, z), Vector3(0.07f, 0.006f, z + 2.2f), 1.0f);
    }

    // Buildings with a cornice ledge, alternating brick per block; lit windows on the upper floors
    const Real kBrickTile = 1.6f;
    uint32_t windowSeed = 0;
    auto addSide = [&](const Block *blocks, size_t count, Real side)
    {
        for (size_t i = 0; i < count; ++i)
        {
            const Block &b = blocks[i];
            MeshBuilder &bricks = ((i + (side > 0 ? 0 : 1)) % 2 == 0) ? redBricks : brownBricks;
            const Real inner = side * kFacadeX;
            const Real outer = side * (kFacadeX + 8.0f);
            bricks.addBox(Vector3(std::min(inner, outer), kSidewalkHeight, b.zStart),
                Vector3(std::max(inner, outer), b.height, b.zStart + b.length), kBrickTile);

            const Real ledgeInner = side * (kFacadeX - 0.3f);
            bricks.addBox(Vector3(std::min(ledgeInner, inner), b.height - 0.4f, b.zStart),
                Vector3(std::max(ledgeInner, inner), b.height - 0.1f, b.zStart + b.length), kBrickTile);

            // Facade faces the street : normal points to -side
            const Vector3 normal(-side, 0.0f, 0.0f);
            const Vector3 right(0.0f, 0.0f, -side);
            for (Real y = 6.0f; y + 1.6f < b.height - 0.5f; y += 3.0f)
            {
                for (Real z = b.zStart + 1.5f; z + 1.0f < b.zStart + b.length; z += 2.6f)
                {
                    if (hash32(++windowSeed * 7919U) % 3 != 0)
                    {
                        continue;
                    }
                    windows.addFace(Vector3(side * (kFacadeX - 0.01f), y, z), normal, right, Vector3::UP, 0.5f, 0.7f, 0.0f);
                }
            }
        }
    };
    addSide(kRightBlocks, sizeof(kRightBlocks) / sizeof(kRightBlocks[0]), 1.0f);
    addSide(kLeftBlocks, sizeof(kLeftBlocks) / sizeof(kLeftBlocks[0]), -1.0f);

    // Close both ends of the street
    brownBricks.addBox(Vector3(-kFacadeX - 8.0f, kSidewalkHeight, kStreetEnd), Vector3(kFacadeX + 8.0f, 14.0f, kStreetEnd + 6.0f), kBrickTile);
    redBricks.addBox(Vector3(-kFacadeX - 8.0f, kSidewalkHeight, kStreetStart - 6.0f), Vector3(kFacadeX + 8.0f, 10.0f, kStreetStart), kBrickTile);

    // Lamp posts : pole, arm over the road, head, glowing bulb underneath
    for (const LampPost &lamp : kLamps)
    {
        const Real s = lamp.side;
        const Real poleX = s * 4.6f;
        const Real headX = s * 3.55f;
        metalParts.addBox(Vector3(poleX - 0.07f, kSidewalkHeight, lamp.z - 0.07f), Vector3(poleX + 0.07f, 4.9f, lamp.z + 0.07f), 1.0f);
        metalParts.addBox(Vector3(std::min(poleX, headX), 4.82f, lamp.z - 0.05f), Vector3(std::max(poleX, headX), 4.9f, lamp.z + 0.05f), 1.0f);
        metalParts.addBox(Vector3(headX - 0.3f, 4.7f, lamp.z - 0.18f), Vector3(headX + 0.3f, 4.86f, lamp.z + 0.18f), 1.0f);
        bulbs.addBox(Vector3(headX - 0.22f, 4.65f, lamp.z - 0.12f), Vector3(headX + 0.22f, 4.7f, lamp.z + 0.12f), 0.0f);
    }

    mMeshes.push_back(redBricks.build("meshBrickRed", redBrickMat->getUUID()));
    addMeshObject(parent, "BricksRed", mMeshes.back());
    mMeshes.push_back(brownBricks.build("meshBrickBrown", brownBrickMat->getUUID()));
    addMeshObject(parent, "BricksBrown", mMeshes.back());
    mMeshes.push_back(road.build("meshRoad", asphaltMat->getUUID()));
    addMeshObject(parent, "Road", mMeshes.back());
    mMeshes.push_back(sidewalk.build("meshSidewalk", pavementMat->getUUID()));
    addMeshObject(parent, "Sidewalk", mMeshes.back());
    mMeshes.push_back(lanes.build("meshLanes", paintMat->getUUID()));
    addMeshObject(parent, "LaneMarks", mMeshes.back());
    mMeshes.push_back(metalParts.build("meshLampPosts", metalMat->getUUID()));
    addMeshObject(parent, "LampPosts", mMeshes.back());
    mMeshes.push_back(bulbs.build("meshBulbs", bulbMat->getUUID()));
    addMeshObject(parent, "LampBulbs", mMeshes.back());
    mMeshes.push_back(windows.build("meshWindows", windowMat->getUUID()));
    addMeshObject(parent, "LitWindows", mMeshes.back());
}

//------------------------------------------------------------------------------

void HDRApp::buildSigns(Transform3D *parent)
{
    struct SignDesc
    {
        const char *name;
        ColorRGB color;
        int32_t shape;
        uint32_t texWidth, texHeight;
        Vector3 center;     // centre of the glowing face
        Vector3 normal;
        Vector3 right;
        Real halfWidth, halfHeight;
        bool blade;         // sticks out of the wall, glows on both sides
        Vector3 lightPos;
        bool flicker;
    };

    const SignDesc kSigns[] =
    {
        { "NeonMagenta", ColorRGB(1.0f, 0.12f, 0.65f), 0, 256, 64,
          Vector3(kFacadeX - 0.115f, 3.6f, 14.5f), Vector3(-1, 0, 0), Vector3(0, 0, -1), 1.6f, 0.42f, false,
          Vector3(kFacadeX - 0.8f, 3.6f, 14.5f), false },
        { "NeonCyan", ColorRGB(0.1f, 0.85f, 1.0f), 1, 64, 144,
          Vector3(-kFacadeX + 0.55f, 4.1f, 25.96f), Vector3(0, 0, -1), Vector3(1, 0, 0), 0.42f, 0.95f, true,
          Vector3(-kFacadeX + 1.5f, 4.1f, 26.0f), true },
        { "NeonOrange", ColorRGB(1.0f, 0.32f, 0.06f), 2, 256, 80,
          Vector3(kFacadeX - 0.115f, 3.0f, 40.0f), Vector3(-1, 0, 0), Vector3(0, 0, -1), 1.3f, 0.4f, false,
          Vector3(kFacadeX - 0.8f, 3.0f, 40.0f), false },
        { "NeonGreen", ColorRGB(0.25f, 1.0f, 0.3f), 0, 256, 64,
          Vector3(-kFacadeX + 0.115f, 3.3f, 50.0f), Vector3(1, 0, 0), Vector3(0, 0, 1), 1.2f, 0.32f, false,
          Vector3(-kFacadeX + 0.8f, 3.3f, 50.0f), false },
    };

    Texture2DPtr metal = buildSolidTexture("texSignBoard", 22, 22, 26);
    MaterialPtr boardMat = buildLitMaterial("matSignBoard", metal, 0.3f);
    MeshBuilder boards;

    for (const SignDesc &desc : kSigns)
    {
        Real emissionScale = 1.0f;
        Texture2DPtr texture = buildNeonTexture(String("tex") + desc.name, desc.texWidth, desc.texHeight, desc.shape, emissionScale);
        const ColorRGBA emission(desc.color.red(), desc.color.green(), desc.color.blue(), emissionScale);
        MaterialPtr material = buildEmissiveMaterial(String("mat") + desc.name, texture, emission);

        MeshBuilder glow;
        glow.addFace(desc.center, desc.normal, desc.right, Vector3::UP, desc.halfWidth, desc.halfHeight, 0.0f);

        const Vector3 boardHalf = desc.right * (desc.halfWidth + 0.08f) + Vector3::UP * (desc.halfHeight + 0.08f);
        if (desc.blade)
        {
            // Board between the two faces, bracket back to the wall
            const Vector3 back = desc.center - desc.normal * 0.08f;
            glow.addFace(back, -desc.normal, -desc.right, Vector3::UP, desc.halfWidth, desc.halfHeight, 0.0f);
            const Vector3 boardCenter = desc.center - desc.normal * 0.04f;
            const Vector3 thickness = desc.normal * 0.03f;
            const Vector3 a = boardCenter - boardHalf - thickness;
            const Vector3 b = boardCenter + boardHalf + thickness;
            boards.addBox(Vector3(std::min(a.x(), b.x()), std::min(a.y(), b.y()), std::min(a.z(), b.z())),
                Vector3(std::max(a.x(), b.x()), std::max(a.y(), b.y()), std::max(a.z(), b.z())), 1.0f);
        }
        else
        {
            // Flat board on the wall behind the glowing face
            const Vector3 boardCenter = desc.center - desc.normal * 0.06f;
            const Vector3 thickness = desc.normal * 0.05f;
            const Vector3 a = boardCenter - boardHalf - thickness;
            const Vector3 b = boardCenter + boardHalf + thickness;
            boards.addBox(Vector3(std::min(a.x(), b.x()), std::min(a.y(), b.y()), std::min(a.z(), b.z())),
                Vector3(std::max(a.x(), b.x()), std::max(a.y(), b.y()), std::max(a.z(), b.z())), 1.0f);
        }

        mMeshes.push_back(glow.build(String("mesh") + desc.name, material->getUUID()));
        GameObjectPtr signGO = addMeshObject(parent, desc.name, mMeshes.back());

        GameObjectPtr lightGO = GameObject::create(String(desc.name) + "Light");
        Transform3DPtr node = lightGO->addComponent<Transform3D>();
        node->setPosition(desc.lightPos);
        parent->addChild(node);
        PointLightPtr light = lightGO->addComponent<PointLight>();
        const Real kNeonLight = 2.5f;
        light->setColor(desc.color);
        light->setDiffuseIntensity(kNeonLight);
        light->setSpecularIntensity(kNeonLight);
        light->setAttenuationConstant(1.0f);
        light->setAttenuationLinear(0.35f);
        light->setAttenuationQuadratic(0.45f);

        if (desc.flicker)
        {
            GeometryPtr geometry = signGO->getComponent<Geometry>();
            auto flicker = signGO->addComponent<NeonFlickerBehaviour>();
            flicker->setTargets(geometry->getMaterial(), emission, light, kNeonLight);
        }
    }

    mMeshes.push_back(boards.build("meshSignBoards", boardMat->getUUID()));
    addMeshObject(parent, "SignBoards", mMeshes.back());
}

//------------------------------------------------------------------------------

GameObjectPtr HDRApp::addMeshObject(Transform3D *parent, const String &name, MeshPtr mesh)
{
    GameObjectPtr go = GameObject::create(name);
    Transform3DPtr xform = go->addComponent<Transform3D>();
    parent->addChild(xform);

    SubMesh *submesh = mesh->getSubMesh(SUB_MESH_NAME);
    GeometryPtr geometry = go->addComponent<Geometry>();
    geometry->setMeshObject(mesh, submesh);

    // Mesh vertices are already in world space, so the mesh extent is the bound
    const VertexAttribute *attr = mesh->findVertexAttributeBySemantic(VertexAttribute::Semantic::E_VAS_POSITION, 0);
    const Buffer &vertices = mesh->getVertices()[attr->getSlot()];
    const size_t stride = mesh->getVertexStride(attr->getSlot());
    const size_t count = vertices.DataSize / stride;
    Vector3 minPos, maxPos;
    for (size_t i = 0; i < count; ++i)
    {
        Vector3 p;
        memcpy(&p, vertices.Data + i * stride, sizeof(Vector3));
        if (i == 0)
        {
            minPos = maxPos = p;
            continue;
        }
        minPos = Vector3(std::min(minPos.x(), p.x()), std::min(minPos.y(), p.y()), std::min(minPos.z(), p.z()));
        maxPos = Vector3(std::max(maxPos.x(), p.x()), std::max(maxPos.y(), p.y()), std::max(maxPos.z(), p.z()));
    }
    AabbBoundPtr bound = go->addComponent<AabbBound>();
    bound->setParams(minPos.x(), maxPos.x(), minPos.y(), maxPos.y(), minPos.z(), maxPos.z());
    return go;
}

//------------------------------------------------------------------------------

ShaderPtr HDRApp::buildLitShader()
{
    ShaderVariantPtr forwardVS = createHDRShaderVariant(HDRShader::kForwardVS, SHADER_STAGE::kVertex);
    ShaderVariantPtr forwardPS = createHDRShaderVariant(HDRShader::kForwardPS, SHADER_STAGE::kPixel);
    ShaderVariantPtr shadowVS = createHDRShaderVariant(HDRShader::kShadowVS, SHADER_STAGE::kVertex);
    if (forwardVS == nullptr || forwardPS == nullptr || shadowVS == nullptr)
    {
        return nullptr;
    }

    PassPtr forwardPass = Pass::create("ForwardBase");
    forwardPass->addShaderVariant(forwardVS->getShaderKeyword(), forwardVS);
    forwardPass->addShaderVariant(forwardPS->getShaderKeyword(), forwardPS);
    forwardPass->addTag(ShaderLab::kBuiltinTagLightMode, ShaderLab::kBuiltinLightModeForwardBaseStr);
    RenderStatePtr forwardState = RenderState::create();
    forwardState->setBlendDesc(BlendDesc());
    forwardState->setDepthStencilDesc(DepthStencilDesc());
    forwardState->setRasterizerDesc(RasterizerDesc());
    forwardPass->setRenderState(forwardState);

    PassPtr shadowPass = Pass::create("ShadowCaster");
    shadowPass->addShaderVariant(shadowVS->getShaderKeyword(), shadowVS);
    shadowPass->addTag(ShaderLab::kBuiltinTagLightMode, ShaderLab::kBuiltinLightModeShadowCasterStr);
    RenderStatePtr shadowState = RenderState::create();
    shadowState->setBlendDesc(BlendDesc());
    shadowState->setDepthStencilDesc(DepthStencilDesc());
    RasterizerDesc shadowRaster;
    shadowRaster.DepthBias = 50.0f;
    shadowRaster.SlopeScaledDepthBias = 2.5f;
    shadowState->setRasterizerDesc(shadowRaster);
    shadowPass->setRenderState(shadowState);

    TechniquePtr tech = Technique::create("HDR-Lit-Technique");
    tech->addPass(forwardPass);
    tech->addPass(shadowPass);
    tech->addTag(ShaderLab::kBuiltinTagQueue, ShaderLab::kBuiltinQueueGeometryStr);

    ShaderPtr shader = T3D_SHADER_MGR.createShader("HDR-Lit-Shader");
    shader->addTechnique(tech);
    return shader;
}

ShaderPtr HDRApp::buildEmissiveShader()
{
    ShaderVariantPtr vs = createHDRShaderVariant(HDRShader::kEmissiveVS, SHADER_STAGE::kVertex);
    ShaderVariantPtr ps = createHDRShaderVariant(HDRShader::kEmissivePS, SHADER_STAGE::kPixel);
    if (vs == nullptr || ps == nullptr)
    {
        return nullptr;
    }

    PassPtr pass = Pass::create("ForwardBase");
    pass->addShaderVariant(vs->getShaderKeyword(), vs);
    pass->addShaderVariant(ps->getShaderKeyword(), ps);
    pass->addTag(ShaderLab::kBuiltinTagLightMode, ShaderLab::kBuiltinLightModeForwardBaseStr);
    RenderStatePtr state = RenderState::create();
    state->setBlendDesc(BlendDesc());
    state->setDepthStencilDesc(DepthStencilDesc());
    state->setRasterizerDesc(RasterizerDesc());
    pass->setRenderState(state);

    TechniquePtr tech = Technique::create("HDR-Emissive-Technique");
    tech->addPass(pass);
    tech->addTag(ShaderLab::kBuiltinTagQueue, ShaderLab::kBuiltinQueueGeometryStr);

    ShaderPtr shader = T3D_SHADER_MGR.createShader("HDR-Emissive-Shader");
    shader->addTechnique(tech);
    return shader;
}

//------------------------------------------------------------------------------

MaterialPtr HDRApp::buildLitMaterial(const String &name, Texture2D *texture, Real smoothness)
{
    MaterialPtr material = T3D_MATERIAL_MGR.createMaterial(name, mLitShader);
    StringArray enableKeywords;
    enableKeywords.push_back("");
    StringArray disableKeywords;
    material->switchKeywords(enableKeywords, disableKeywords);
    material->setTexture("texCube", texture->getUUID());

    material->setVector("tiny3d_CameraWorldPos", Vector4::ZERO);
    material->setVector("tiny3d_ObjectSmoothness", Vector4(smoothness, 0, 0, 0));
    material->setVector("tiny3d_ProjectionParams", Vector4(1.0f, 0, 0, 0));
    material->setColor("tiny3d_AmbientLight", ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
    material->setColor("tiny3d_DirLightColor", ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
    material->setVector("tiny3d_DirLightDir", Vector4::ZERO);
    ColorArray colors(4, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
    Vector4Array values(4, Vector4::ZERO);
    material->setColorArray("tiny3d_PointLightColor", colors);
    material->setVectorArray("tiny3d_PointLightPos", values);
    material->setVectorArray("tiny3d_PointLightAttenuation", values);
    material->setColorArray("tiny3d_SpotLightColor", colors);
    material->setVectorArray("tiny3d_SpotLightPos", values);
    material->setVectorArray("tiny3d_SpotLightDir", values);
    material->setVectorArray("tiny3d_SpotLightAttenuation", values);

    mMaterials.push_back(material);
    return material;
}

MaterialPtr HDRApp::buildEmissiveMaterial(const String &name, Texture2D *texture, const ColorRGBA &emission)
{
    MaterialPtr material = T3D_MATERIAL_MGR.createMaterial(name, mEmissiveShader);
    StringArray enableKeywords;
    enableKeywords.push_back("");
    StringArray disableKeywords;
    material->switchKeywords(enableKeywords, disableKeywords);
    material->setTexture("_EmissionTex", texture->getUUID());
    material->setColor("hdr_EmissionColor", emission);

    mMaterials.push_back(material);
    return material;
}

//------------------------------------------------------------------------------

Texture2DPtr HDRApp::buildColorTexture(const String &name, uint32_t width, uint32_t height, const uint8_t *bgra)
{
    // Full box-filtered mip chain, tightly packed level after level; without it brick
    // walls far down the street turn into moire
    uint32_t levels = 1;
    size_t dataSize = static_cast<size_t>(width) * height * 4;
    for (uint32_t w = width, h = height; w > 1 || h > 1; ++levels)
    {
        w = std::max<uint32_t>(1, w / 2);
        h = std::max<uint32_t>(1, h / 2);
        dataSize += static_cast<size_t>(w) * h * 4;
    }

    uint8_t *pixels = T3D_POD_NEW_ARRAY(uint8_t, dataSize);
    memcpy(pixels, bgra, static_cast<size_t>(width) * height * 4);
    const uint8_t *prev = pixels;
    uint8_t *cur = pixels + static_cast<size_t>(width) * height * 4;
    uint32_t prevW = width;
    uint32_t prevH = height;
    for (uint32_t level = 1; level < levels; ++level)
    {
        const uint32_t w = std::max<uint32_t>(1, prevW / 2);
        const uint32_t h = std::max<uint32_t>(1, prevH / 2);
        for (uint32_t y = 0; y < h; ++y)
        {
            for (uint32_t x = 0; x < w; ++x)
            {
                const uint32_t x0 = std::min(x * 2, prevW - 1), x1 = std::min(x * 2 + 1, prevW - 1);
                const uint32_t y0 = std::min(y * 2, prevH - 1), y1 = std::min(y * 2 + 1, prevH - 1);
                for (uint32_t c = 0; c < 4; ++c)
                {
                    const uint32_t sum = prev[(y0 * prevW + x0) * 4 + c] + prev[(y0 * prevW + x1) * 4 + c]
                        + prev[(y1 * prevW + x0) * 4 + c] + prev[(y1 * prevW + x1) * 4 + c];
                    cur[(y * w + x) * 4 + c] = static_cast<uint8_t>((sum + 2) / 4);
                }
            }
        }
        prev = cur;
        cur += static_cast<size_t>(w) * h * 4;
        prevW = w;
        prevH = h;
    }

    Buffer texData;
    texData.Data = pixels;
    texData.DataSize = dataSize;
    Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D(name, width, height, PixelFormat::E_PF_B8G8R8A8, texData, levels);
    SamplerDesc samplerDesc;
    texture->setSamplerDesc(samplerDesc);
    mTextures.push_back(texture);
    return texture;
}

Texture2DPtr HDRApp::buildBrickTexture(const String &name, uint8_t r, uint8_t g, uint8_t b, uint32_t seed)
{
    // 64 x 32 px bricks with 3 px mortar, every other course shifted by half a brick
    const uint32_t kSize = 256;
    const uint32_t kBrickW = 64;
    const uint32_t kBrickH = 32;
    const uint32_t kMortar = 3;
    TArray<uint8_t> bgra(kSize * kSize * 4);
    for (uint32_t y = 0; y < kSize; ++y)
    {
        const uint32_t row = y / kBrickH;
        const uint32_t shift = (row % 2) * (kBrickW / 2);
        for (uint32_t x = 0; x < kSize; ++x)
        {
            const uint32_t bx = (x + shift) % kSize;
            const uint32_t col = bx / kBrickW;
            const bool mortar = (y % kBrickH) < kMortar || (bx % kBrickW) < kMortar;
            const Real grain = (hashUnit(seed * 131071U + y * kSize + x) - 0.5f) * 18.0f;
            Real cr, cg, cb;
            if (mortar)
            {
                cr = cg = cb = 150.0f + grain;
                cb -= 8.0f;
            }
            else
            {
                const Real tone = 0.72f + 0.4f * hashUnit(seed * 977U + row * 37U + col);
                cr = r * tone + grain;
                cg = g * tone + grain;
                cb = b * tone + grain;
            }
            uint8_t *p = &bgra[(y * kSize + x) * 4];
            p[0] = toByte(cb);
            p[1] = toByte(cg);
            p[2] = toByte(cr);
            p[3] = 255;
        }
    }
    return buildColorTexture(name, kSize, kSize, bgra.data());
}

Texture2DPtr HDRApp::buildAsphaltTexture()
{
    const uint32_t kSize = 128;
    TArray<uint8_t> bgra(kSize * kSize * 4);
    for (uint32_t y = 0; y < kSize; ++y)
    {
        for (uint32_t x = 0; x < kSize; ++x)
        {
            const Real v = 44.0f + (hashUnit(y * kSize + x + 5U) - 0.5f) * 22.0f
                + (hashUnit((y / 8) * 64 + (x / 8) + 99991U) - 0.5f) * 10.0f;
            uint8_t *p = &bgra[(y * kSize + x) * 4];
            p[0] = toByte(v + 3.0f);
            p[1] = toByte(v);
            p[2] = toByte(v - 1.0f);
            p[3] = 255;
        }
    }
    return buildColorTexture("texAsphalt", kSize, kSize, bgra.data());
}

Texture2DPtr HDRApp::buildPavementTexture()
{
    // Two 1 m slabs per texture repeat, dark joints between them
    const uint32_t kSize = 128;
    TArray<uint8_t> bgra(kSize * kSize * 4);
    for (uint32_t y = 0; y < kSize; ++y)
    {
        for (uint32_t x = 0; x < kSize; ++x)
        {
            const bool joint = (x % 64) < 2 || (y % 64) < 2;
            const Real slab = 0.9f + 0.2f * hashUnit((y / 64) * 2 + (x / 64) + 31337U);
            const Real v = joint ? 52.0f : (108.0f * slab + (hashUnit(y * kSize + x + 777U) - 0.5f) * 16.0f);
            uint8_t *p = &bgra[(y * kSize + x) * 4];
            p[0] = toByte(v);
            p[1] = toByte(v);
            p[2] = toByte(v + 2.0f);
            p[3] = 255;
        }
    }
    return buildColorTexture("texPavement", kSize, kSize, bgra.data());
}

Texture2DPtr HDRApp::buildSolidTexture(const String &name, uint8_t r, uint8_t g, uint8_t b)
{
    const uint32_t kSize = 4;
    TArray<uint8_t> bgra(kSize * kSize * 4);
    for (uint32_t i = 0; i < kSize * kSize; ++i)
    {
        bgra[i * 4 + 0] = b;
        bgra[i * 4 + 1] = g;
        bgra[i * 4 + 2] = r;
        bgra[i * 4 + 3] = 255;
    }
    return buildColorTexture(name, kSize, kSize, bgra.data());
}

Texture2DPtr HDRApp::buildWindowTexture()
{
    // Warm room light behind a dark frame and cross bars, brighter near the top
    const uint32_t kSize = 64;
    TArray<uint8_t> bgra(kSize * kSize * 4);
    for (uint32_t y = 0; y < kSize; ++y)
    {
        for (uint32_t x = 0; x < kSize; ++x)
        {
            const bool frame = x < 4 || x >= kSize - 4 || y < 4 || y >= kSize - 4;
            const bool bars = (x >= 30 && x < 34) || (y >= 26 && y < 30);
            Real v = 0.0f;
            if (!frame && !bars)
            {
                v = 255.0f * (1.0f - 0.45f * static_cast<Real>(y) / static_cast<Real>(kSize));
            }
            else
            {
                v = 10.0f;
            }
            uint8_t *p = &bgra[(y * kSize + x) * 4];
            p[0] = toByte(v);
            p[1] = toByte(v);
            p[2] = toByte(v);
            p[3] = 255;
        }
    }
    return buildColorTexture("texWindow", kSize, kSize, bgra.data());
}

Texture2DPtr HDRApp::buildNeonTexture(const String &name, uint32_t width, uint32_t height, int32_t shape, Real &emissionScale)
{
    // Tube shapes in pixels : a frame plus a motif per shape
    const Real w = static_cast<Real>(width);
    const Real h = static_cast<Real>(height);
    const Real inset = 6.0f;
    TArray<Segment> segments;
    addPolyline(segments, { Vector2(inset, inset), Vector2(w - inset, inset), Vector2(w - inset, h - inset), Vector2(inset, h - inset) }, true);
    switch (shape)
    {
    case 0:
        {
            TArray<Vector2> zigzag;
            for (int32_t i = 0; i <= 8; ++i)
            {
                zigzag.push_back(Vector2(w * (0.12f + 0.095f * static_cast<Real>(i)), h * ((i % 2) ? 0.3f : 0.7f)));
            }
            addPolyline(segments, zigzag, false);
        }
        break;
    case 1:
        addCircle(segments, w * 0.5f, h * 0.24f, w * 0.26f);
        for (Real v : { 0.5f, 0.64f, 0.78f })
        {
            segments.push_back({ w * 0.25f, h * v, w * 0.75f, h * v });
        }
        break;
    default:
        addCircle(segments, w * 0.2f, h * 0.5f, h * 0.28f);
        segments.push_back({ w * 0.38f, h * 0.38f, w * 0.88f, h * 0.38f });
        segments.push_back({ w * 0.38f, h * 0.62f, w * 0.75f, h * 0.62f });
        break;
    }

    // Bright tube core well above 1.0 plus a soft halo baked into the texture
    const Real kCore = 6.0f;
    TArray<Real> values(width * height);
    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            Real d = 1e9f;
            for (const Segment &s : segments)
            {
                d = std::min(d, distanceToSegment(static_cast<Real>(x) + 0.5f, static_cast<Real>(y) + 0.5f, s));
            }
            values[y * width + x] = 0.015f + kCore * std::exp(-(d * d) / (1.4f * 1.4f)) + 0.5f * std::exp(-(d * d) / (5.0f * 5.0f));
        }
    }

    Texture2DPtr texture;
    const RHICapabilities &caps = T3D_AGENT.getActiveRHIContext()->getCapabilities();
    if (caps.supportsFormat(PixelFormat::E_PF_R16G16B16A16_FLOAT, kFormatSupportTexture | kFormatSupportLinearFilter))
    {
        const uint32_t dataSize = width * height * 4 * sizeof(uint16_t);
        uint16_t *pixels = reinterpret_cast<uint16_t *>(T3D_POD_NEW_ARRAY(uint8_t, dataSize));
        for (uint32_t i = 0; i < width * height; ++i)
        {
            const uint16_t v = floatToHalf(values[i]);
            pixels[i * 4 + 0] = v;
            pixels[i * 4 + 1] = v;
            pixels[i * 4 + 2] = v;
            pixels[i * 4 + 3] = floatToHalf(1.0f);
        }
        Buffer texData;
        texData.Data = reinterpret_cast<uint8_t *>(pixels);
        texData.DataSize = dataSize;
        texture = T3D_TEXTURE_MGR.createTexture2D(name, width, height, PixelFormat::E_PF_R16G16B16A16_FLOAT, texData);
        emissionScale = 1.0f;
    }
    else
    {
        // Same profile squeezed into 8 bits, the material multiplies the peak back in
        T3D_LOG_WARNING(LOG_TAG_APP, "HDRApp : %s textures are not filterable here, neon sign %s uses 8-bit with a scaled emission.",
            PixelFormatUtil::getName(PixelFormat::E_PF_R16G16B16A16_FLOAT), name.c_str());
        const Real peak = kCore + 0.6f;
        const uint32_t dataSize = width * height * 4;
        uint8_t *pixels = T3D_POD_NEW_ARRAY(uint8_t, dataSize);
        for (uint32_t i = 0; i < width * height; ++i)
        {
            const uint8_t v = toByte(values[i] / peak * 255.0f);
            pixels[i * 4 + 0] = v;
            pixels[i * 4 + 1] = v;
            pixels[i * 4 + 2] = v;
            pixels[i * 4 + 3] = 255;
        }
        Buffer texData;
        texData.Data = pixels;
        texData.DataSize = dataSize;
        texture = T3D_TEXTURE_MGR.createTexture2D(name, width, height, PixelFormat::E_PF_B8G8R8A8, texData);
        emissionScale = peak;
    }

    SamplerDesc samplerDesc;
    samplerDesc.MipFilter = FilterOptions::kNone;
    samplerDesc.AddressU = TextureAddressMode::kClamp;
    samplerDesc.AddressV = TextureAddressMode::kClamp;
    texture->setSamplerDesc(samplerDesc);
    mTextures.push_back(texture);
    return texture;
}
