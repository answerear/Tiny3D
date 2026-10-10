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

TResult TransformationApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    //=========================================================================
    // Degree / Radian
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Degree / Radian ==========");

    Degree degree;
    Radian radian;

    String s = typeid(degree).name();
    T3D_LOG_INFO(LOG_TAG_APP, "Degree type: %s", s.c_str());

    Real a = REAL_ZERO;
    a = Math::sin(radian);
    T3D_LOG_INFO(LOG_TAG_APP, "sin(0 radian) = %f", a);

    //=========================================================================
    // Vector2
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Vector2 ==========");

    Vector2 p2(REAL_ONE, REAL_ZERO);
    T3D_LOG_INFO(LOG_TAG_APP, "p2 = %s", p2.getDebugString().c_str());

    //=========================================================================
    // Vector3
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Vector3 ==========");

    Real scalar = REAL_ONE;
    Vector3 p3 = Vector3::UNIT_X;
    T3D_LOG_INFO(LOG_TAG_APP, "p3 (UNIT_X) = %s", p3.getDebugString().c_str());

    p3 = scalar * p3;
    T3D_LOG_INFO(LOG_TAG_APP, "scalar(%f) * p3 = %s", scalar, p3.getDebugString().c_str());

    Vector3 t = Vector3::UNIT_Y;
    t = p3.cross(t);
    T3D_LOG_INFO(LOG_TAG_APP, "p3.cross(UNIT_Y) = %s", t.getDebugString().c_str());

    //=========================================================================
    // Vector4
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Vector4 ==========");

    Vector4 p4(REAL_ONE, REAL_ZERO, REAL_ZERO, REAL_ZERO);
    Vector4 t4(REAL_ZERO, REAL_ONE, REAL_ZERO, REAL_ZERO);
    T3D_LOG_INFO(LOG_TAG_APP, "p4 = %s", p4.getDebugString().c_str());
    T3D_LOG_INFO(LOG_TAG_APP, "t4 = %s", t4.getDebugString().c_str());

    t4 = p4.cross(t4);
    T3D_LOG_INFO(LOG_TAG_APP, "p4.cross(t4) = %s", t4.getDebugString().c_str());

    //=========================================================================
    // Matrix2 * Vector2
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Matrix2 ==========");

    Matrix2 m2;
    T3D_LOG_INFO(LOG_TAG_APP, "m2 (identity) = %s", m2.getDebugString().c_str());
    T3D_LOG_INFO(LOG_TAG_APP, "p2 = %s", p2.getDebugString().c_str());

    p2 = m2 * p2;
    T3D_LOG_INFO(LOG_TAG_APP, "m2 * p2 = %s", p2.getDebugString().c_str());

    //=========================================================================
    // Matrix3 * Vector3
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Matrix3 ==========");

    Matrix3 m3;
    T3D_LOG_INFO(LOG_TAG_APP, "m3 (identity) = %s", m3.getDebugString().c_str());
    T3D_LOG_INFO(LOG_TAG_APP, "p3 = %s", p3.getDebugString().c_str());

    p3 = m3 * p3;
    T3D_LOG_INFO(LOG_TAG_APP, "m3 * p3 = %s", p3.getDebugString().c_str());

    //=========================================================================
    // Quaternion
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Quaternion ==========");

    Quaternion q;
    T3D_LOG_INFO(LOG_TAG_APP, "q (identity) = %s", q.getDebugString().c_str());

    q = scalar * q;
    T3D_LOG_INFO(LOG_TAG_APP, "scalar(%f) * q = %s", scalar, q.getDebugString().c_str());

    //=========================================================================
    // Matrix4
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Matrix4 ==========");

    Matrix4 m4;
    T3D_LOG_INFO(LOG_TAG_APP, "m4 (identity):\n%s", m4.getDebugString(true).c_str());

    m4 = scalar * m4;
    T3D_LOG_INFO(LOG_TAG_APP, "scalar(%f) * m4:\n%s", scalar, m4.getDebugString(true).c_str());

    p4 = Vector4(REAL_ONE, REAL_ZERO, REAL_ZERO, REAL_ZERO);
    T3D_LOG_INFO(LOG_TAG_APP, "p4 = %s", p4.getDebugString().c_str());

    p4 = m4 * p4;
    T3D_LOG_INFO(LOG_TAG_APP, "m4 * p4 = %s", p4.getDebugString().c_str());

    p4 = p4 * m4;
    T3D_LOG_INFO(LOG_TAG_APP, "p4 * m4 = %s", p4.getDebugString().c_str());

    //=========================================================================
    // Matrix3 QDU Decomposition
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Matrix3 QDU Decomposition ==========");

    m3[0][0] = 0.707f, m3[0][1] = 1.25f, m3[0][2] = 0;
    m3[1][0] = -0.707f, m3[1][1] = 1.25f, m3[1][2] = 0;
    m3[2][0] = 0, m3[2][1] = 0, m3[2][2] = 1;
    T3D_LOG_INFO(LOG_TAG_APP, "m3:\n%s", m3.getDebugString(true).c_str());

    Matrix3 R;
    Vector3 S, T;
    m3.QDUDecomposition(R, S, T);
    T3D_LOG_INFO(LOG_TAG_APP, "QDU => R:\n%s", R.getDebugString(true).c_str());
    T3D_LOG_INFO(LOG_TAG_APP, "QDU => S = %s", S.getDebugString().c_str());
    T3D_LOG_INFO(LOG_TAG_APP, "QDU => T = %s", T.getDebugString().c_str());

    Vector3 axis;
    Radian radians;
    R.toAxisAngle(axis, radians);
    degree = radians.valueDegrees();
    T3D_LOG_INFO(LOG_TAG_APP, "R => axis = %s, angle = %f degrees",
        axis.getDebugString().c_str(), (float)degree.valueDegrees());

    Matrix3 Mr(Vector3::UNIT_Z, Radian(Degree(45).valueRadians()));
    Matrix3 Ms(S[0], S[1], S[2]);
    R = Mr * Ms;
    T3D_LOG_INFO(LOG_TAG_APP, "Mr(45 deg Z) * Ms:\n%s", R.getDebugString(true).c_str());

    //=========================================================================
    // Matrix4 Decomposition
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Matrix4 Decomposition ==========");

    m4[0][0] = Math::sqrt(2); m4[0][1] = Math::sqrt(2); m4[0][2] = REAL_ZERO; m4[0][3] = -2;
    m4[1][0] = -Math::sqrt(2); m4[1][1] = Math::sqrt(2); m4[1][2] = REAL_ZERO; m4[1][3] = -2;
    m4[2][0] = REAL_ZERO; m4[2][1] = REAL_ZERO; m4[2][2] = 2; m4[2][3] = REAL_ZERO;
    m4[3][0] = REAL_ZERO; m4[3][1] = REAL_ZERO; m4[3][2] = REAL_ZERO; m4[3][3] = REAL_ONE;
    T3D_LOG_INFO(LOG_TAG_APP, "m4:\n%s", m4.getDebugString(true).c_str());

    Quaternion Q;
    m4.decomposition(T, S, Q);
    Q.toRotationMatrix(R);
    T3D_LOG_INFO(LOG_TAG_APP, "decomposition => T = %s", T.getDebugString().c_str());
    T3D_LOG_INFO(LOG_TAG_APP, "decomposition => S = %s", S.getDebugString().c_str());
    T3D_LOG_INFO(LOG_TAG_APP, "decomposition => Q = %s", Q.getDebugString().c_str());
    T3D_LOG_INFO(LOG_TAG_APP, "Q => R:\n%s", R.getDebugString(true).c_str());

    //=========================================================================
    // Manual column extraction for scale and rotation
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Manual Column Extraction ==========");

    Vector3 T1, S1;
    Quaternion Q1;
    Matrix3 R1;
    T1[0] = m4[0][3]; T1[1] = m4[1][3]; T1[2] = m4[2][3];
    T3D_LOG_INFO(LOG_TAG_APP, "T1 (translation) = %s", T1.getDebugString().c_str());

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

    length = Math::sqrt(m4[0][2] * m4[0][2] + m4[1][2] * m4[1][2] + m4[2][2] * m4[2][2]);
    invLength = REAL_ONE / length;
    R1[0][2] = m4[0][2] * invLength;
    R1[1][2] = m4[1][2] * invLength;
    R1[2][2] = m4[2][2] * invLength;
    S1[2] = length;

    Q1.fromRotationMatrix(R1);
    T3D_LOG_INFO(LOG_TAG_APP, "S1 (scale) = %s", S1.getDebugString().c_str());
    T3D_LOG_INFO(LOG_TAG_APP, "R1:\n%s", R1.getDebugString(true).c_str());
    T3D_LOG_INFO(LOG_TAG_APP, "Q1 = %s", Q1.getDebugString().c_str());

    //=========================================================================
    // Euler angle rotation matrices
    //=========================================================================
    T3D_LOG_INFO(LOG_TAG_APP, "========== Euler Rotation ==========");

    Matrix3 Ry(Vector3::UNIT_Y, Radian(Degree(30).valueRadians()));
    Matrix3 Rx(Vector3::UNIT_X, Radian(Degree(30).valueRadians()));
    R = Ry * Rx;
    T3D_LOG_INFO(LOG_TAG_APP, "Ry(30) * Rx(30):\n%s", R.getDebugString(true).c_str());

    //=========================================================================
    // Performance Benchmark
    //=========================================================================
    runPerformanceBenchmark();

    return T3D_OK;
}

//-----------------------------------------------------------------------------

void TransformationApp::runPerformanceBenchmark()
{
    T3D_LOG_INFO(LOG_TAG_APP,
        "========== Performance Benchmark ==========");

    // Output current SIMD status
#if defined(T3D_SIMD_SSE)
    T3D_LOG_INFO(LOG_TAG_APP, "SIMD Status: SSE Enabled");
#elif defined(T3D_SIMD_NEON)
    T3D_LOG_INFO(LOG_TAG_APP, "SIMD Status: NEON Enabled");
#elif defined(T3D_SIMD_NONE)
    T3D_LOG_INFO(LOG_TAG_APP, "SIMD Status: NONE (Scalar)");
#else
    T3D_LOG_INFO(LOG_TAG_APP, "SIMD Status: Unknown");
#endif

    const int32_t ITERATIONS = 1000000;
    T3D_LOG_INFO(LOG_TAG_APP, "Iterations: %d", ITERATIONS);

    T3D_PERF_RESET();

    // Prepare test data
    Matrix4 matA(
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f);
    Matrix4 matB(
        16.0f, 15.0f, 14.0f, 13.0f,
        12.0f, 11.0f, 10.0f, 9.0f,
        8.0f, 7.0f, 6.0f, 5.0f,
        4.0f, 3.0f, 2.0f, 1.0f);
    Vector4 vecA(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4 vecB(5.0f, 6.0f, 7.0f, 8.0f);

    //--- Matrix4 * Matrix4 ---
    {
        volatile float sink = 0.0f;
        T3D_PERF_BEGIN("Matrix4*Matrix4");
        for (int32_t i = 0; i < ITERATIONS; ++i)
        {
            Matrix4 result = matA * matB;
            sink += result[0][0];
        }
        T3D_PERF_END("Matrix4*Matrix4");
        T3D_LOG_INFO(LOG_TAG_APP, "[Matrix4*Matrix4] %lld us (sink=%f)",
            (long long)T3D_PERF_GET_LAST_US("Matrix4*Matrix4"), (float)sink);
    }

    //--- Matrix4 * Vector4 ---
    {
        volatile float sink = 0.0f;
        T3D_PERF_BEGIN("Matrix4*Vector4");
        for (int32_t i = 0; i < ITERATIONS; ++i)
        {
            Vector4 result = matA * vecA;
            sink += result.x();
        }
        T3D_PERF_END("Matrix4*Vector4");
        T3D_LOG_INFO(LOG_TAG_APP, "[Matrix4*Vector4] %lld us (sink=%f)",
            (long long)T3D_PERF_GET_LAST_US("Matrix4*Vector4"), (float)sink);
    }

    //--- Vector4 + Vector4 ---
    {
        volatile float sink = 0.0f;
        T3D_PERF_BEGIN("Vector4+Vector4");
        for (int32_t i = 0; i < ITERATIONS; ++i)
        {
            Vector4 result = vecA + vecB;
            sink += result.x();
        }
        T3D_PERF_END("Vector4+Vector4");
        T3D_LOG_INFO(LOG_TAG_APP, "[Vector4+Vector4] %lld us (sink=%f)",
            (long long)T3D_PERF_GET_LAST_US("Vector4+Vector4"), (float)sink);
    }

    //--- Vector4.dot ---
    {
        volatile float sink = 0.0f;
        T3D_PERF_BEGIN("Vector4.dot");
        for (int32_t i = 0; i < ITERATIONS; ++i)
        {
            Real result = vecA.dot(vecB);
            sink += result;
        }
        T3D_PERF_END("Vector4.dot");
        T3D_LOG_INFO(LOG_TAG_APP, "[Vector4.dot] %lld us (sink=%f)",
            (long long)T3D_PERF_GET_LAST_US("Vector4.dot"), (float)sink);
    }

    //--- Matrix4 + Matrix4 ---
    {
        volatile float sink = 0.0f;
        T3D_PERF_BEGIN("Matrix4+Matrix4");
        for (int32_t i = 0; i < ITERATIONS; ++i)
        {
            Matrix4 result = matA + matB;
            sink += result[0][0];
        }
        T3D_PERF_END("Matrix4+Matrix4");
        T3D_LOG_INFO(LOG_TAG_APP, "[Matrix4+Matrix4] %lld us (sink=%f)",
            (long long)T3D_PERF_GET_LAST_US("Matrix4+Matrix4"), (float)sink);
    }

    //--- Matrix4 transpose ---
    {
        volatile float sink = 0.0f;
        T3D_PERF_BEGIN("Matrix4.transpose");
        for (int32_t i = 0; i < ITERATIONS; ++i)
        {
            Matrix4 result = matA.transpose();
            sink += result[0][0];
        }
        T3D_PERF_END("Matrix4.transpose");
        T3D_LOG_INFO(LOG_TAG_APP, "[Matrix4.transpose] %lld us (sink=%f)",
            (long long)T3D_PERF_GET_LAST_US("Matrix4.transpose"), (float)sink);
    }

    //--- Matrix4 * scalar ---
    {
        volatile float sink = 0.0f;
        T3D_PERF_BEGIN("Matrix4*scalar");
        for (int32_t i = 0; i < ITERATIONS; ++i)
        {
            Matrix4 result = matA * 2.5f;
            sink += result[0][0];
        }
        T3D_PERF_END("Matrix4*scalar");
        T3D_LOG_INFO(LOG_TAG_APP, "[Matrix4*scalar] %lld us (sink=%f)",
            (long long)T3D_PERF_GET_LAST_US("Matrix4*scalar"), (float)sink);
    }

    // Output summary
    T3D_LOG_INFO(LOG_TAG_APP, "---------- Summary ----------");
    T3D_PERF_LOG();
}

//-----------------------------------------------------------------------------

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
