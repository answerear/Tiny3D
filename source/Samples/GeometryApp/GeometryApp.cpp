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

#include "GeometryApp.h"


using namespace Tiny3D;

const char *SUB_MESH_NAME = "#0";

GeometryApp theApp;


GeometryApp::GeometryApp()
{
}

GeometryApp::~GeometryApp()
{
}

bool GeometryApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    // create scene
    ScenePtr scene = T3D_SCENE_MGR.createScene("TestScene");
    T3D_SCENE_MGR.setCurrentScene(scene);
    
    // root game object
    GameObjectPtr go = GameObject::create("TestScene");
    scene->addRootGameObject(go);
    Transform3DPtr root = go->addComponent<Transform3D>();

    // render window for render target in camera
    RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
    RenderTargetPtr rt = RenderTarget::create(rw);

    // transform node for camera
    go = GameObject::create("Camera");
    Transform3DPtr xform = go->addComponent<Transform3D>();
    root->addChild(xform);
    // camera component
    CameraPtr camera = go->addComponent<Camera>();
    camera->setOrder(0);
    Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    camera->setViewport(vp);
    camera->setClearColor(ColorRGB::GRAY);
    camera->setRenderTarget(rt);
    scene->addCamera(camera);

    // transform node for cube
    go = GameObject::create("Cube");
    xform = go->addComponent<Transform3D>();
    root->addChild(xform);
    // geometry component
    GeometryPtr geometry = go->addComponent<Geometry>();
    MeshPtr mesh = buildMesh();
    geometry->setMeshObject(mesh, mesh->getSubMesh(SUB_MESH_NAME));

    return true;
}

void GeometryApp::applicationWillTerminate() 
{
    
}

MeshPtr GeometryApp::buildMesh()
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
        ColorRGBA diffuse {};
    };
    
    // vertex attributes
    VertexAttribute attrPos(0, 0, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
    VertexAttribute attrColor(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_COLOR, VertexAttribute::Semantic::E_VAS_DIFFUSE, 0);        
    VertexAttributes attributes(2);
    attributes[0] = attrPos;
    attributes[1] = attrColor;

    // vertices & indices
    Vector3 offset;
    ColorRGBA color = ColorRGBA::WHITE;
    Vector3 center(0.0f, 0.0f, 0.0f);
    Vector3 extent(0.5f, 0.5f, 0.5f);
    BoxVertex *vertices = new BoxVertex[8];
    uint16_t *indices = new uint16_t[36];
    
    // V0
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[0].position = center + offset;
    vertices[0].diffuse = color;

    // V1
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[1].position = center + offset;
    vertices[1].diffuse = color;

    // V2
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[2].position = center + offset;
    vertices[2].diffuse = color;

    // V3
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[3].position = center + offset;
    vertices[3].diffuse = color;

    // V4
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[4].position = center + offset;
    vertices[4].diffuse = color;

    // V5
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[5].position = center + offset;
    vertices[5].diffuse = color;

    // V6
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[6].position = center + offset;
    vertices[6].diffuse = color;

    // V7
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[7].position = center + offset;
    vertices[7].diffuse = color;

    // Front face
    indices[0] = 0, indices[1] = 1, indices[2] = 2;
    indices[3] = 1, indices[4] = 3, indices[5] = 2;

    // Back
    indices[6] = 4, indices[7] = 5, indices[8] = 6;
    indices[9] = 5, indices[10] = 7, indices[11] = 6;

    // Left
    indices[12] = 0, indices[13] = 6, indices[14] = 7;
    indices[15] = 0, indices[16] = 7, indices[17] = 1;

    // Right
    indices[18] = 2, indices[19] = 3, indices[20] = 4;
    indices[21] = 3, indices[22] = 5, indices[23] = 4;

    // Top
    indices[24] = 0, indices[25] = 2, indices[26] = 4;
    indices[27] = 0, indices[28] = 4, indices[29] = 6;

    // Bottom
    indices[30] = 1, indices[31] = 7, indices[32] = 3;
    indices[33] = 3, indices[34] = 7, indices[35] = 5;
    

    // construct mesh resource
    Buffer vertexBuffer;
    vertexBuffer.Data = (uint8_t*)vertices;
    vertexBuffer.DataSize = sizeof(vertices);
    Vertices vertexBuffers(1);
    vertexBuffers[0] = vertexBuffer;
    
    VertexStrides strides(1, sizeof(BoxVertex));
    VertexOffsets offsets(1, 0);
    
    Buffer indexBuffer;
    indexBuffer.Data = (uint8_t*)indices;
    indexBuffer.DataSize = sizeof(indices);
    
    String name = SUB_MESH_NAME;
    SubMeshPtr submesh = SubMesh::create(name, PrimitiveType::kTriangleList, indexBuffer, true);
    SubMeshes subMeshes;
    subMeshes.emplace(name, submesh);

    MeshPtr mesh = T3D_MESH_MGR.createMesh("Cube", std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes));
    return mesh;
}


