/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "IntersectionApp.h"


using namespace Tiny3D;

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


IntersectionApp::IntersectionApp()
    : Application()
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
                           Z

 ******************************************************************************/
bool IntersectionApp::applicationDidFinishLaunching()
{
    // 射线和三角形相交检测
    testRayTriangle();

    // 射线和平面相交检测
    testRayPlane();

    // 射线和球相交性检测
    testRaySphere();

    // 射线和AABB相交性检测
    testRayAabb();

    return true;
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
    bool isIntersection = intr.test();
    printf("Ray and Triangle #0 intersection result is %d\n", isIntersection);

    // Triangle #1
    Triangle triangle1(TriangleVertices1);

    // 这个Ray和Triangle是不相交的
    intr.setTriangle(&triangle1);
    isIntersection = intr.test();
    printf("Ray and Triangle #1 intersection result is %d\n", isIntersection);
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
    bool isIntersection = intr.test();
    printf("Ray and Plane #0 intersection result is %d\n", isIntersection);

    // Plane #1
    Plane plane1(
        Vector3(Real(4), REAL_ONE, REAL_ONE), 
        Vector3(Real(4), -REAL_ONE, REAL_ZERO),
        Vector3(Real(4), REAL_ONE, REAL_ZERO));

    // 这个Ray和Plane是不相交的
    intr.setPlane(&plane1);
    isIntersection = intr.test();
    printf("Ray and Plane #1 intersection result is %d\n", isIntersection);
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
    bool isIntersection = intr.test();
    printf("Ray and Sphere #0 intersection result is %d\n", isIntersection);

    // Sphere #1
    Sphere sphere1;
    sphere1.build(BoxVertices1, BoxVerticesCount);

    // 这个Ray和Sphere是不相交的
    intr.setSphere(&sphere1);
    isIntersection = intr.test();
    printf("Ray and Sphere #1 intersection result is %d\n", isIntersection);
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
    bool isIntersection = intr.test();
    printf("Ray and AABB #0 intersection result is %d\n", isIntersection);

    // AABB #1
    Aabb box1;
    box1.build(BoxVertices1, BoxVerticesCount);

    // 这个Ray和AABB是不相交的
    intr.setAabb(&box1);
    isIntersection = intr.test();
    printf("Ray and AABB #1 intersection result is %d\n", isIntersection);
}

void IntersectionApp::testRayObb()
{
    // 位于 (0, 0, 5) 位置起点，朝向 -Z 方向的射线
    Ray ray(
        Vector3(REAL_ZERO, REAL_ZERO, Real(5)),
        Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // 构造旋转矩阵
    Matrix3 m;
    m.fromAxisAngle(Vector3::UNIT_Y, Degree(30));

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
    bool isIntersection = intr.test();
    printf("Ray and OBB #0 intersection result is %d\n", isIntersection);

    for (i = 0; i < BoxVerticesCount; ++i)
    {
        points[i] = m * BoxVertices1[i];
    }

    // OBB #1
    Obb box1;
    box1.build(points, BoxVerticesCount);

    // 这个Ray和OBB是不相交的
    intr.setObb(&box1);
    isIntersection = intr.test();
    printf("Ray and OBB #1 intersection result is $d\n", isIntersection);
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
    bool isIntersection = intr.test();
    printf("Sphere #0 and Sphere #1 intersection result is %d\n", isIntersection);

    // Sphere #2
    Sphere sphere2;
    sphere2.build(BoxVertices1, BoxVerticesCount);

    // 这两个sphere是不相交的
    intr.setSphere1(&sphere2);
    isIntersection = intr.test();
    printf("Sphere #0 and Sphere #2 intersection result is%d\n", isIntersection);
}

void IntersectionApp::testSpherePlane()
{

}

void IntersectionApp::testAabbPlane()
{

}

void IntersectionApp::testObbPlane()
{

}

void IntersectionApp::testFrustumSphere()
{

}

void IntersectionApp::testFrustumAabb()
{

}

void IntersectionApp::testFrustumObb()
{

}
