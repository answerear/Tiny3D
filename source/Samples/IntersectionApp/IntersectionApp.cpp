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

#include "IntersectionApp.h"


using namespace Tiny3D;

IntersectionApp theApp;

const size_t TriVerticesCount = 3;

const Vector3 TriangleVertices0[TriVerticesCount] =
{
    Vector3(-REAL_ONE, -REAL_ONE, REAL_ZERO),
    Vector3( REAL_ONE, -REAL_ONE, REAL_ZERO),
    Vector3( REAL_ZERO, REAL_ONE, REAL_ZERO)
};

const Vector3 TriangleVertices1[TriVerticesCount] =
{
    Vector3(Real(3), -REAL_ONE, REAL_ZERO),
    Vector3(Real(5), -REAL_ONE, REAL_ZERO),
    Vector3(Real(4),  REAL_ONE, REAL_ZERO)
};

const size_t BoxVerticesCount = 8;

const Vector3 BoxVertices0[BoxVerticesCount] =
{
    Vector3( REAL_HALF,  REAL_HALF,  REAL_HALF),
    Vector3( REAL_HALF, -REAL_HALF,  REAL_HALF),
    Vector3(-REAL_HALF,  REAL_HALF,  REAL_HALF),
    Vector3(-REAL_HALF, -REAL_HALF,  REAL_HALF),
    Vector3( REAL_HALF,  REAL_HALF, -REAL_HALF),
    Vector3( REAL_HALF, -REAL_HALF, -REAL_HALF),
    Vector3(-REAL_HALF,  REAL_HALF, -REAL_HALF),
    Vector3(-REAL_HALF, -REAL_HALF, -REAL_HALF),
};

const Vector3 BoxVertices1[BoxVerticesCount] = 
{
    Vector3(Real(4),  REAL_HALF,  REAL_HALF),
    Vector3(Real(4), -REAL_HALF,  REAL_HALF),
    Vector3(Real(3),  REAL_HALF,  REAL_HALF),
    Vector3(Real(3), -REAL_HALF,  REAL_HALF),
    Vector3(Real(4),  REAL_HALF, -REAL_HALF),
    Vector3(Real(4), -REAL_HALF, -REAL_HALF),
    Vector3(Real(3),  REAL_HALF, -REAL_HALF),
    Vector3(Real(3), -REAL_HALF, -REAL_HALF),
};

const Vector3 BoxVertices2[BoxVerticesCount] =
{
    Vector3( REAL_ONE,  REAL_HALF,  REAL_HALF),
    Vector3( REAL_ONE, -REAL_HALF,  REAL_HALF),
    Vector3(REAL_ZERO,  REAL_HALF,  REAL_HALF),
    Vector3(REAL_ZERO, -REAL_HALF,  REAL_HALF),
    Vector3( REAL_ONE,  REAL_HALF, -REAL_HALF),
    Vector3( REAL_ONE, -REAL_HALF, -REAL_HALF),
    Vector3(REAL_ZERO,  REAL_HALF, -REAL_HALF),
    Vector3(REAL_ZERO, -REAL_HALF, -REAL_HALF),
};

const Vector3 BoxVertices3[BoxVerticesCount] =
{
    Vector3(Real(60),  REAL_HALF,  REAL_HALF),
    Vector3(Real(60), -REAL_HALF,  REAL_HALF),
    Vector3(Real(50),  REAL_HALF,  REAL_HALF),
    Vector3(Real(50), -REAL_HALF,  REAL_HALF),
    Vector3(Real(60),  REAL_HALF, -REAL_HALF),
    Vector3(Real(60), -REAL_HALF, -REAL_HALF),
    Vector3(Real(50),  REAL_HALF, -REAL_HALF),
    Vector3(Real(50), -REAL_HALF, -REAL_HALF),
};

// 近胶囊：覆盖原点附近盒体区域
const Capsule CapsuleNear(
    Vector3(REAL_ZERO, -REAL_HALF, REAL_ZERO),
    Vector3(REAL_ZERO,  REAL_HALF, REAL_ZERO),
    REAL_HALF);

// 远胶囊：平移到 x≈3.5，用于 Ray / Sphere / AABB / OBB / Triangle / Capsule 反例
const Capsule CapsuleFar(
    Vector3(Real(3.5f), -REAL_HALF, REAL_ZERO),
    Vector3(Real(3.5f),  REAL_HALF, REAL_ZERO),
    REAL_HALF);

// 更远胶囊：对齐 BoxVertices3，用于 Frustum 反例
const Capsule CapsuleFarFrustum(
    Vector3(Real(55), -REAL_HALF, REAL_ZERO),
    Vector3(Real(55),  REAL_HALF, REAL_ZERO),
    REAL_HALF);

static void printBoolIntrResult(const char *name, bool actual, bool expected,
    const String &objA, const String &objB)
{
    const bool pass = (actual == expected);
    if (pass)
    {
        printf("%s: expected=%d actual=%d [PASS]\n",
            name, expected ? 1 : 0, actual ? 1 : 0);
    }
    else
    {
        printf("%s: expected=%d actual=%d [FAIL] | A={%s} | B={%s}\n",
            name, expected ? 1 : 0, actual ? 1 : 0,
            objA.c_str(), objB.c_str());
    }
}

static void printIntIntrResult(const char *name, int32_t actual, int32_t expected,
    const String &objA, const String &objB)
{
    const bool pass = (actual == expected);
    if (pass)
    {
        printf("%s: expected=%d actual=%d [PASS]\n",
            name, expected, actual);
    }
    else
    {
        printf("%s: expected=%d actual=%d [FAIL] | A={%s} | B={%s}\n",
            name, expected, actual,
            objA.c_str(), objB.c_str());
    }
}

IntersectionApp::IntersectionApp()
{
}

IntersectionApp::~IntersectionApp()
{
}


/*******************************************************************************

                                    Y
                                    |
                                    |
                                    |
                                    |
                                    |
                                    |
                                    |
                                    |
                                    |
                                    |______________________________ X
                                   / 
                                  /  O
                                 /
                                /
                               /
                              /
                             /
                            /
                           -Z

 ******************************************************************************/
TResult IntersectionApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    // 射线和三角形相交检测
    testRayTriangle();

    // 射线和平面相交检测
    testRayPlane();

    // 射线和球体相交检测
    testRaySphere();

    // 射线和AABB相交检测
    testRayAabb();

    // 射线和OBB相交检测
    testRayObb();

    // 两个球相交检测
    testSphereSphere();

    // 球和平面相交检测
    testSpherePlane();

    // AABB和平面相交检测
    testAabbPlane();

    // OBB和平面相交检测
    testObbPlane();

    // 视锥体和球相交检测
    testFrustumSphere();

    // 视锥体和AABB相交检测
    testFrustumAabb();

    // 视锥体和OBB相交检测
    testFrustumObb();

    // 射线和胶囊体相交检测
    testRayCapsule();

    // 胶囊体和球体相交检测
    testCapsuleSphere();

    // 胶囊体和AABB相交检测
    testCapsuleAabb();

    // 胶囊体和OBB相交检测
    testCapsuleObb();

    // 胶囊体和平面相交检测
    testCapsulePlane();

    // 胶囊体和三角形相交检测
    testCapsuleTriangle();

    // 两个胶囊体相交检测
    testCapsuleCapsule();

    // 视锥体和胶囊体相交检测
    testFrustumCapsule();

    return T3D_OK;
}

void IntersectionApp::applicationDidEnterBackground()
{
}

void IntersectionApp::applicationWillEnterForeground()
{
}

void IntersectionApp::applicationWillTerminate()
{

}

void IntersectionApp::applicationLowMemory()
{

}

void IntersectionApp::testRayTriangle()
{
    // 位于 (0, 0, 5) 位置起点，朝向 -Z 方向的射线
    Ray ray(
        Vector3(REAL_ZERO, REAL_ZERO, Real(5)),
        Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // Triangle #0
    Triangle triangle0(TriangleVertices0);

    // 这个Ray和Triangle是相交的
    IntrRayTriangle intr(ray, triangle0);
    printBoolIntrResult("Ray and Triangle #0", intr.test(), true,
        ray.getDebugString(), triangle0.getDebugString());

    // Triangle #1
    Triangle triangle1(TriangleVertices1);

    // 这个Ray和Triangle是不相交的
    intr.setTriangle(&triangle1);
    printBoolIntrResult("Ray and Triangle #1", intr.test(), false,
        ray.getDebugString(), triangle1.getDebugString());
}

void IntersectionApp::testRayPlane()
{
    // 位于 (0, 0, 5) 位置起点，朝向 -Z 方向的射线
    Ray ray(
        Vector3(REAL_ZERO, REAL_ZERO, Real(5)),
        Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // Plane #0
    Plane plane0(
        TriangleVertices0[0], 
        TriangleVertices0[1], 
        TriangleVertices0[2]);

    // 这个Ray和Plane是相交的
    IntrRayPlane intr(ray, plane0);
    printBoolIntrResult("Ray and Plane #0", intr.test(), true,
        ray.getDebugString(), plane0.getDebugString());

    // Plane #1
    Plane plane1(
        Vector3(Real(4), REAL_ONE, REAL_ONE), 
        Vector3(Real(4), -REAL_ONE, REAL_ZERO),
        Vector3(Real(4), REAL_ONE, REAL_ZERO));

    // 这个Ray和Plane是不相交的
    intr.setPlane(&plane1);
    printBoolIntrResult("Ray and Plane #1", intr.test(), false,
        ray.getDebugString(), plane1.getDebugString());
}

void IntersectionApp::testRaySphere()
{
    // 位于 (0, 0, 5) 位置起点，朝向 -Z 方向的射线
    Ray ray(
        Vector3(REAL_ZERO, REAL_ZERO, Real(5)),
        Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // Sphere #0
    Sphere sphere0;
    sphere0.build(BoxVertices0, BoxVerticesCount);

    // 这个Ray和Sphere是相交的
    IntrRaySphere intr(ray, sphere0);
    printBoolIntrResult("Ray and Sphere #0", intr.test(), true,
        ray.getDebugString(), sphere0.getDebugString());

    // Sphere #1
    Sphere sphere1;
    sphere1.build(BoxVertices1, BoxVerticesCount);

    // 这个Ray和Sphere是不相交的
    intr.setSphere(&sphere1);
    printBoolIntrResult("Ray and Sphere #1", intr.test(), false,
        ray.getDebugString(), sphere1.getDebugString());
}


void IntersectionApp::testRayAabb()
{
    // 位于 (0, 0, 5) 位置起点，朝向 -Z 方向的射线
    Ray ray(
        Vector3(REAL_ZERO, REAL_ZERO, Real(5)), 
        Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // AABB #0
    Aabb box0;
    box0.build(BoxVertices0, BoxVerticesCount);

    // 这个Ray和AABB是相交的
    IntrRayAabb intr(ray, box0);
    printBoolIntrResult("Ray and AABB #0", intr.test(), true,
        ray.getDebugString(), box0.getDebugString());

    // AABB #1
    Aabb box1;
    box1.build(BoxVertices1, BoxVerticesCount);

    // 这个Ray和AABB是不相交的
    intr.setAabb(&box1);
    printBoolIntrResult("Ray and AABB #1", intr.test(), false,
        ray.getDebugString(), box1.getDebugString());
}

void IntersectionApp::testRayObb()
{
    // 位于 (0, 0, 5) 位置起点，朝向 -Z 方向的射线
    Ray ray(
        Vector3(REAL_ZERO, REAL_ZERO, Real(5)),
        Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // 构造旋转矩阵
    Matrix3 m;
    m.fromAxisAngle(Vector3::UNIT_Y, Radian(Degree(30).valueRadians()));

    Vector3 points[BoxVerticesCount];
    size_t i = 0;
    for (i = 0; i < BoxVerticesCount; ++i)
    {
        points[i] = m * BoxVertices0[i];
    }

    // OBB #0
    Obb box0;
    box0.build(points, BoxVerticesCount);
    
    // 这个Ray和OBB是相交的
    IntrRayObb intr(ray, box0);
    printBoolIntrResult("Ray and OBB #0", intr.test(), true,
        ray.getDebugString(), box0.getDebugString());

    for (i = 0; i < BoxVerticesCount; ++i)
    {
        points[i] = m * BoxVertices1[i];
    }

    // OBB #1
    Obb box1;
    box1.build(points, BoxVerticesCount);

    // 这个Ray和OBB是不相交的
    intr.setObb(&box1);
    printBoolIntrResult("Ray and OBB #1", intr.test(), false,
        ray.getDebugString(), box1.getDebugString());
}

void IntersectionApp::testSphereSphere()
{
    // Sphere #0
    Sphere sphere0;
    sphere0.build(BoxVertices0, BoxVerticesCount);

    // Sphere #1
    Sphere sphere1;
    sphere1.build(BoxVertices2, BoxVerticesCount);

    // 这两个sphere是相交的
    IntrSphereSphere intr(sphere0, sphere1);
    printBoolIntrResult("Sphere #0 and Sphere #1", intr.test(), true,
        sphere0.getDebugString(), sphere1.getDebugString());

    // Sphere #2
    Sphere sphere2;
    sphere2.build(BoxVertices1, BoxVerticesCount);

    // 这两个sphere是不相交的
    intr.setSphere1(&sphere2);
    printBoolIntrResult("Sphere #0 and Sphere #2", intr.test(), false,
        sphere0.getDebugString(), sphere2.getDebugString());
}

void IntersectionApp::testSpherePlane()
{
    // Sphere
    Sphere sphere;
    sphere.build(BoxVertices0, BoxVerticesCount);

    // Plane #0
    Plane plane0(
        TriangleVertices0[0],
        TriangleVertices0[1],
        TriangleVertices0[2]);

    // 这个Sphere和Plane是相交的 → 0
    IntrSpherePlane intr(sphere, plane0);
    printIntIntrResult("Sphere and Plane #0", intr.test(), 0,
        sphere.getDebugString(), plane0.getDebugString());

    // Plane #1：x=4，法线 +X，球完全在背面 → -1
    Plane plane1(
        Vector3(Real(4), REAL_ONE, REAL_ONE),
        Vector3(Real(4), -REAL_ONE, REAL_ZERO),
        Vector3(Real(4), REAL_ONE, REAL_ZERO));

    // 这个Sphere和Plane是不相交的
    intr.setPlane(&plane1);
    printIntIntrResult("Sphere and Plane #1", intr.test(), -1,
        sphere.getDebugString(), plane1.getDebugString());
}

void IntersectionApp::testAabbPlane()
{
    // Box
    Aabb box;
    box.build(BoxVertices0, BoxVerticesCount);

    // Plane #0
    Plane plane0(
        TriangleVertices0[0],
        TriangleVertices0[1],
        TriangleVertices0[2]);

    // 这个AABB和Plane是相交的 → 0
    IntrAabbPlane intr(box, plane0);
    printIntIntrResult("AABB and Plane #0", intr.test(), 0,
        box.getDebugString(), plane0.getDebugString());

    // Plane #1：x=4，法线 +X，AABB 完全在背面 → -1
    Plane plane1(
        Vector3(Real(4), REAL_ONE, REAL_ONE),
        Vector3(Real(4), -REAL_ONE, REAL_ZERO),
        Vector3(Real(4), REAL_ONE, REAL_ZERO));

    // 这个AABB和Plane是不相交的
    intr.setPlane(&plane1);
    printIntIntrResult("AABB and Plane #1", intr.test(), -1,
        box.getDebugString(), plane1.getDebugString());
}

void IntersectionApp::testObbPlane()
{
    // 构造旋转矩阵
    Matrix3 m;
    m.fromAxisAngle(Vector3::UNIT_Y, Radian(Degree(30).valueRadians()));

    Vector3 points[BoxVerticesCount];
    size_t i = 0;
    for (i = 0; i < BoxVerticesCount; ++i)
    {
        points[i] = m * BoxVertices0[i];
    }

    // OBB
    Obb box;
    box.build(points, BoxVerticesCount);

    // Plane #0
    Plane plane0(
        TriangleVertices0[0],
        TriangleVertices0[1],
        TriangleVertices0[2]);

    // 这个OBB和Plane是相交的 → 0
    IntrObbPlane intr(box, plane0);
    printIntIntrResult("OBB and Plane #0", intr.test(), 0,
        box.getDebugString(), plane0.getDebugString());

    // Plane #1：x=5，法线 +X，OBB 完全在背面 → -1
    Plane plane1(
        Vector3(Real(5), REAL_ONE, REAL_ONE),
        Vector3(Real(5), -REAL_ONE, REAL_ZERO),
        Vector3(Real(5), REAL_ONE, REAL_ZERO));

    // 这个OBB和Plane是不相交的
    intr.setPlane(&plane1);
    printIntIntrResult("OBB and Plane #1", intr.test(), -1,
        box.getDebugString(), plane1.getDebugString());
}

void IntersectionApp::testFrustumSphere()
{
    Frustum frustum;
    buildFrustum(frustum);

    // Sphere #0
    Sphere sphere0;
    sphere0.build(BoxVertices0, BoxVerticesCount);

    // 这个Frustum和Sphere是相交的
    IntrFrustumSphere intr(frustum, sphere0);
    printBoolIntrResult("Frustum and Sphere #0", intr.test(), true,
        frustum.getDebugString(), sphere0.getDebugString());

    // Sphere #1
    Sphere sphere1;
    sphere1.build(BoxVertices3, BoxVerticesCount);

    // 这个Frustum和Sphere是不相交的
    intr.setSphere(&sphere1);
    printBoolIntrResult("Frustum and Sphere #1", intr.test(), false,
        frustum.getDebugString(), sphere1.getDebugString());
}

void IntersectionApp::testFrustumAabb()
{
    Frustum frustum;
    buildFrustum(frustum);

    // AABB #0
    Aabb box0;
    box0.build(BoxVertices0, BoxVerticesCount);

    // 这个Frustum和AABB是相交的
    IntrFrustumAabb intr(frustum, box0);
    printBoolIntrResult("Frustum and AABB #0", intr.test(), true,
        frustum.getDebugString(), box0.getDebugString());

    // AABB #1
    Aabb box1;
    box1.build(BoxVertices3, BoxVerticesCount);

    // 这个Frustum和AABB是不相交的
    intr.setBox(&box1);
    printBoolIntrResult("Frustum and AABB #1", intr.test(), false,
        frustum.getDebugString(), box1.getDebugString());
}

void IntersectionApp::testFrustumObb()
{
    Frustum frustum;
    buildFrustum(frustum);

    // 构造旋转矩阵
    Matrix3 m;
    m.fromAxisAngle(Vector3::UNIT_Y, Radian(Degree(30).valueRadians()));

    Vector3 points[BoxVerticesCount];
    size_t i = 0;
    for (i = 0; i < BoxVerticesCount; ++i)
    {
        points[i] = m * BoxVertices0[i];
    }

    // OBB #0
    Obb box0;
    box0.build(points, BoxVerticesCount);

    // 这个Frustum和OBB是相交的
    IntrFrustumObb intr(frustum, box0);
    printBoolIntrResult("Frustum and OBB #0", intr.test(), true,
        frustum.getDebugString(), box0.getDebugString());

    for (i = 0; i < BoxVerticesCount; ++i)
    {
        points[i] = m * BoxVertices3[i];
    }

    // OBB #1
    Obb box1;
    box1.build(points, BoxVerticesCount);

    // 这个Frustum和OBB是不相交的
    intr.setBox(&box1);
    printBoolIntrResult("Frustum and OBB #1", intr.test(), false,
        frustum.getDebugString(), box1.getDebugString());
}

void IntersectionApp::testRayCapsule()
{
    // 位于 (0, 0, 5) 位置起点，朝向 -Z 方向的射线
    Ray ray(
        Vector3(REAL_ZERO, REAL_ZERO, Real(5)),
        Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // 这个 Ray 和近胶囊是相交的
    IntrRayCapsule intr(ray, CapsuleNear);
    printBoolIntrResult("Ray and Capsule #0", intr.test(), true,
        ray.getDebugString(), CapsuleNear.getDebugString());

    // 这个 Ray 和远胶囊是不相交的
    intr.setCapsule(&CapsuleFar);
    printBoolIntrResult("Ray and Capsule #1", intr.test(), false,
        ray.getDebugString(), CapsuleFar.getDebugString());
}

void IntersectionApp::testCapsuleSphere()
{
    Sphere sphere0;
    sphere0.build(BoxVertices0, BoxVerticesCount);

    // 近胶囊与 Sphere #0 相交
    IntrCapsuleSphere intr(CapsuleNear, sphere0);
    printBoolIntrResult("Capsule #0 and Sphere #0", intr.test(), true,
        CapsuleNear.getDebugString(), sphere0.getDebugString());

    Sphere sphere1;
    sphere1.build(BoxVertices1, BoxVerticesCount);

    // 近胶囊与 Sphere #1 不相交
    intr.setSphere(&sphere1);
    printBoolIntrResult("Capsule #0 and Sphere #1", intr.test(), false,
        CapsuleNear.getDebugString(), sphere1.getDebugString());
}

void IntersectionApp::testCapsuleAabb()
{
    Aabb box0;
    box0.build(BoxVertices0, BoxVerticesCount);

    // 近胶囊与 AABB #0 相交
    IntrCapsuleAabb intr(CapsuleNear, box0);
    printBoolIntrResult("Capsule #0 and AABB #0", intr.test(), true,
        CapsuleNear.getDebugString(), box0.getDebugString());

    Aabb box1;
    box1.build(BoxVertices1, BoxVerticesCount);

    // 近胶囊与 AABB #1 不相交
    intr.setBox(&box1);
    printBoolIntrResult("Capsule #0 and AABB #1", intr.test(), false,
        CapsuleNear.getDebugString(), box1.getDebugString());
}

void IntersectionApp::testCapsuleObb()
{
    Matrix3 m;
    m.fromAxisAngle(Vector3::UNIT_Y, Radian(Degree(30).valueRadians()));

    Vector3 points[BoxVerticesCount];
    size_t i = 0;
    for (i = 0; i < BoxVerticesCount; ++i)
    {
        points[i] = m * BoxVertices0[i];
    }

    Obb box0;
    box0.build(points, BoxVerticesCount);

    // 近胶囊与 OBB #0 相交
    IntrCapsuleObb intr(CapsuleNear, box0);
    printBoolIntrResult("Capsule #0 and OBB #0", intr.test(), true,
        CapsuleNear.getDebugString(), box0.getDebugString());

    for (i = 0; i < BoxVerticesCount; ++i)
    {
        points[i] = m * BoxVertices1[i];
    }

    Obb box1;
    box1.build(points, BoxVerticesCount);

    // 近胶囊与 OBB #1 不相交
    intr.setBox(&box1);
    printBoolIntrResult("Capsule #0 and OBB #1", intr.test(), false,
        CapsuleNear.getDebugString(), box1.getDebugString());
}

void IntersectionApp::testCapsulePlane()
{
    // Plane #0：由 TriangleVertices0 构成，近胶囊与之相交 → 0
    Plane plane0(
        TriangleVertices0[0],
        TriangleVertices0[1],
        TriangleVertices0[2]);

    IntrCapsulePlane intr(CapsuleNear, plane0);
    printIntIntrResult("Capsule #0 and Plane #0", intr.test(), 0,
        CapsuleNear.getDebugString(), plane0.getDebugString());

    // Plane #1：x=4，法线 +X，近胶囊完全在背面 → -1
    Plane plane1(
        Vector3(Real(4), REAL_ONE, REAL_ONE),
        Vector3(Real(4), -REAL_ONE, REAL_ZERO),
        Vector3(Real(4), REAL_ONE, REAL_ZERO));

    intr.setPlane(&plane1);
    printIntIntrResult("Capsule #0 and Plane #1", intr.test(), -1,
        CapsuleNear.getDebugString(), plane1.getDebugString());
}

void IntersectionApp::testCapsuleTriangle()
{
    Triangle triangle0(TriangleVertices0);

    // 近胶囊与 Triangle #0 相交
    IntrCapsuleTriangle intr(CapsuleNear, triangle0);
    printBoolIntrResult("Capsule #0 and Triangle #0", intr.test(), true,
        CapsuleNear.getDebugString(), triangle0.getDebugString());

    Triangle triangle1(TriangleVertices1);

    // 近胶囊与 Triangle #1 不相交
    intr.setTriangle(&triangle1);
    printBoolIntrResult("Capsule #0 and Triangle #1", intr.test(), false,
        CapsuleNear.getDebugString(), triangle1.getDebugString());
}

void IntersectionApp::testCapsuleCapsule()
{
    // 另一近胶囊：沿 X 轻微偏移，仍与 CapsuleNear 相交
    Capsule capsuleOverlap(
        Vector3(REAL_HALF, -REAL_HALF, REAL_ZERO),
        Vector3(REAL_HALF,  REAL_HALF, REAL_ZERO),
        REAL_HALF);

    IntrCapsuleCapsule intr(CapsuleNear, capsuleOverlap);
    printBoolIntrResult("Capsule #0 and Capsule #1", intr.test(), true,
        CapsuleNear.getDebugString(), capsuleOverlap.getDebugString());

    // 近胶囊与远胶囊不相交
    intr.setCapsule1(&CapsuleFar);
    printBoolIntrResult("Capsule #0 and Capsule #2", intr.test(), false,
        CapsuleNear.getDebugString(), CapsuleFar.getDebugString());
}

void IntersectionApp::testFrustumCapsule()
{
    Frustum frustum;
    buildFrustum(frustum);

    // Frustum 与近胶囊相交
    IntrFrustumCapsule intr(frustum, CapsuleNear);
    printBoolIntrResult("Frustum and Capsule #0", intr.test(), true,
        frustum.getDebugString(), CapsuleNear.getDebugString());

    // Frustum 与更远胶囊不相交
    intr.setCapsule(&CapsuleFarFrustum);
    printBoolIntrResult("Frustum and Capsule #1", intr.test(), false,
        frustum.getDebugString(), CapsuleFarFrustum.getDebugString());
}

void IntersectionApp::buildFrustum(Frustum &frustum)
{
    // 构造Frustum的六个平面

    // 近平面
    Plane n(Vector3::NEGATIVE_UNIT_Z, Vector3(0, 0, 10));

    // 远平面
    Plane f(Vector3::UNIT_Z, Vector3(0, 0, -10));

    // 上平面
    Matrix3 m0(Vector3::UNIT_X, Radian(Math::PI / Real(6.0f)));
    Vector3 axis0 = m0 * Vector3::NEGATIVE_UNIT_Y;
    Plane top(axis0, Vector3(0, 10, 0));

    // 下平面
    Matrix3 m1(Vector3::UNIT_X, -Radian(Math::PI / Real(6.0f)));
    Vector3 axis1 = m1 * Vector3::UNIT_Y;
    Plane bottom(axis1, Vector3(0, -10, 0));

    // 左平面
    Matrix3 m2(Vector3::UNIT_Y, Radian(Math::PI / Real(6.0f)));
    Vector3 axis2 = m2 * Vector3::UNIT_X;
    Plane left(axis2, Vector3(-10, 0, 0));

    // 右平面
    Matrix3 m3(Vector3::UNIT_Y, -Radian(Math::PI / Real(6.0f)));
    Vector3 axis3 = m3 * Vector3::NEGATIVE_UNIT_X;
    Plane right(axis3, Vector3(10, 0, 0));

    // Frustum
    frustum.setFace(E_FACE_NEAR, n);
    frustum.setFace(E_FACE_FAR, f);
    frustum.setFace(E_FACE_TOP, top);
    frustum.setFace(E_FACE_BOTTOM, bottom);
    frustum.setFace(E_FACE_LEFT, left);
    frustum.setFace(E_FACE_RIGHT, right);
}
