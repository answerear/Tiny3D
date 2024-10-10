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

#include "TransformationApp.h"


using namespace Tiny3D;

TransformationApp theApp;

TransformationApp::TransformationApp()
{
}

TransformationApp::~TransformationApp()
{
}

bool TransformationApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    Degree degree;
    Radian radian;

    String s = typeid(degree).name();
    printf("TT %s", s.c_str());

    Real a = REAL_ZERO;

    a = Math::sin(radian);

    Vector2 p2(REAL_ONE, REAL_ZERO);

    Real scalar = REAL_ONE;
    Vector3 p3 = Vector3::UNIT_X;
    p3 = scalar * p3;
    Vector3 t = Vector3::UNIT_Y;
    t = p3.cross(t);

    Vector4 p4(REAL_ONE, REAL_ZERO, REAL_ZERO, REAL_ZERO);
    Vector4 t4(REAL_ZERO, REAL_ONE, REAL_ZERO, REAL_ZERO);
    t4 = p4.cross(t4);

    Matrix2 m2;
    p2 = m2 * p2;

    Matrix3 m3;
    p3 = m3 * p3;

    Quaternion q;
    q = scalar * q;

    Matrix4 m4;
    m4 = scalar * m4;
    p4 = m4 * p4;
    p4 = p4 * m4;

    // test
    m3[0][0] = 0.707f, m3[0][1] = 1.25f, m3[0][2] = 0;
    m3[1][0] = -0.707f, m3[1][1] = 1.25f, m3[1][2] = 0;
    m3[2][0] = 0, m3[2][1] = 0, m3[2][2] = 1;

    Matrix3 R;
    Vector3 S, T;
    m3.QDUDecomposition(R, S, T);
    
    Vector3 axis;
    Radian radians;
    R.toAxisAngle(axis, radians);
    degree = radians.valueDegrees();

    Matrix3 Mr(Vector3::UNIT_Z, Radian(Degree(45).valueRadians()));
    Matrix3 Ms(S[0], S[1], S[2]);
    R = Mr * Ms;

    m4[0][0] = Math::sqrt(2); m4[0][1] = Math::sqrt(2); m4[0][2] = REAL_ZERO; m4[0][3] = -2;
    m4[1][0] = -Math::sqrt(2); m4[1][1] = Math::sqrt(2); m4[1][2] = REAL_ZERO; m4[1][3] = -2;
    m4[2][0] = REAL_ZERO; m4[2][1] = REAL_ZERO; m4[2][2] = 2; m4[2][3] = REAL_ZERO;
    m4[3][0] = REAL_ZERO; m4[3][1] = REAL_ZERO; m4[3][2] = REAL_ZERO; m4[3][3] = REAL_ONE;

    Quaternion Q;
    m4.decomposition(T, S, Q);
    Q.toRotationMatrix(R);

    Vector3 T1, S1;
    Quaternion Q1;
    Matrix3 R1;
    T1[0] = m4[0][3]; T1[1] = m4[1][3]; T1[2] = m4[2][3];
    Real length = Math::sqrt(m4[0][0] * m4[0][0] + m4[1][0] * m4[1][0] + m4[2][0] * m4[2][0]);
    Real invLength = REAL_ONE / length;
    R1[0][0] = m4[0][0] * invLength; 
    R1[1][0] = m4[1][0] * invLength; 
    R1[2][0] = m4[2][0] * invLength;

    S1[0] = length;

    length = Math::sqrt(m4[0][1] * m4[0][1] + m4[1][1] * m4[1][1] + m4[2][1] * m4[2][1]);
    invLength = REAL_ONE / length;
    R1[0][1] = m4[0][1] * invLength;
    R1[1][1] = m4[1][1] * invLength;
    R1[2][1] = m4[2][1] * invLength;

    S1[1] = length;

    invLength = Math::sqrt(m4[0][2] * m4[0][2] + m4[1][2] * m4[1][2] + m4[2][2] * m4[2][2]);
    invLength = REAL_ONE / length;
    R1[0][2] = m4[0][2] * invLength;
    R1[1][2] = m4[1][2] * invLength;
    R1[2][2] = m4[2][2] * invLength;

    S1[2] = length;

    Q1.fromRotationMatrix(R1);

    Matrix3 Ry(Vector3::UNIT_Y, Radian(Degree(30).valueRadians()));
    Matrix3 Rx(Vector3::UNIT_X, Radian(Degree(30).valueRadians()));
    R = Ry * Rx;

//     Rx.fromEulerAnglesXYZ(Degree(30), Degree(0), Degree(0));

    return true;
}

void TransformationApp::applicationDidEnterBackground()
{
}

void TransformationApp::applicationWillEnterForeground()
{
}

void TransformationApp::applicationWillTerminate()
{

}

void TransformationApp::applicationLowMemory()
{

}

