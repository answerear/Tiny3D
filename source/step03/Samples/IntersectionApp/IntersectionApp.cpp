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
    Vector3(REAL_ONE, -REAL_ONE, REAL_ZERO),
    Vector3(REAL_ZERO, REAL_ONE, REAL_ZERO)
};

const Vector3 TriangleVertices1[TriVerticesCount] =
{
    Vector3(Real(3), -REAL_ONE, REAL_ZERO),
    Vector3(Real(5), -REAL_ONE, REAL_ZERO),
    Vector3(Real(4), REAL_ONE, REAL_ZERO)
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
    // ���ߺ��������ཻ���
    testRayTriangle();

    // ���ߺ�ƽ���ཻ���
    testRayPlane();

    // ���ߺ����ཻ�Լ��
    testRaySphere();

    // ���ߺ�AABB�ཻ�Լ��
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
    // λ�� (0, 0, 5) λ����㣬���� -Z ���������
    Ray ray(
        Vector3(REAL_ZERO, REAL_ZERO, Real(5)),
        Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // Triangle #0
    Triangle triangle0(TriangleVertices0);

    // ���Ray��Triangle���ཻ��
    IntrRayTriangle intr(ray, triangle0);
    bool isIntersection = intr.test();
    printf("Ray and Triangle #0 intersection result is %d\n", isIntersection);

    // Triangle #1
    Triangle triangle1(TriangleVertices1);

    // ���Ray��Triangle�ǲ��ཻ��
    intr.setTriangle(&triangle1);
    isIntersection = intr.test();
    printf("Ray and Triangle #1 intersection result is %d\n", isIntersection);
}

void IntersectionApp::testRayPlane()
{
    // λ�� (0, 0, 5) λ����㣬���� -Z ���������
    Ray ray(
        Vector3(REAL_ZERO, REAL_ZERO, Real(5)),
        Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // Plane #0
    Plane plane0(
        TriangleVertices0[0], 
        TriangleVertices0[1], 
        TriangleVertices0[2]);

    // ���Ray��Plane���ཻ��
    IntrRayPlane intr(ray, plane0);
    bool isIntersection = intr.test();
    printf("Ray and Plane #0 intersection result is %d\n", isIntersection);

    // Plane #1
    Plane plane1(
        Vector3(Real(4), REAL_ONE, REAL_ONE), 
        Vector3(Real(4), -REAL_ONE, REAL_ZERO),
        Vector3(Real(4), REAL_ONE, REAL_ZERO));

    // ���Ray��Plane�ǲ��ཻ��
    intr.setPlane(&plane1);
    isIntersection = intr.test();
    printf("Ray and Plane #1 intersection result is %d\n", isIntersection);
}

void IntersectionApp::testRaySphere()
{
    // λ�� (0, 0, 5) λ����㣬���� -Z ���������
    Ray ray(
        Vector3(REAL_ZERO, REAL_ZERO, Real(5)),
        Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // Sphere #0
    Sphere sphere0;
    sphere0.build(BoxVertices0, BoxVerticesCount);

    // ���Ray��Sphere���ཻ��
    IntrRaySphere intr(ray, sphere0);
    bool isIntersection = intr.test();
    printf("Ray and Sphere #0 intersection result is %d\n", isIntersection);

    // Sphere #1
    Sphere sphere1;
    sphere1.build(BoxVertices1, BoxVerticesCount);

    // ���Ray��Sphere�ǲ��ཻ��
    intr.setSphere(&sphere1);
    isIntersection = intr.test();
    printf("Ray and Sphere #1 intersection result is %d\n", isIntersection);
}


void IntersectionApp::testRayAabb()
{
    // λ�� (0, 0, 5) λ����㣬���� -Z ���������
    Ray ray(
        Vector3(REAL_ZERO, REAL_ZERO, Real(5)), 
        Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // AABB #0
    Aabb box0;
    box0.build(BoxVertices0, BoxVerticesCount);

    // ���Ray��AABB���ཻ��
    IntrRayAabb intr(ray, box0);
    bool isIntersection = intr.test();
    printf("Ray and AABB #0 intersection result is %d\n", isIntersection);

    // AABB #1
    Aabb box1;
    box1.build(BoxVertices1, BoxVerticesCount);

    // ���Ray��AABB�ǲ��ཻ��
    intr.setAabb(&box1);
    isIntersection = intr.test();
    printf("Ray and AABB #1 intersection result is %d\n", isIntersection);
}

