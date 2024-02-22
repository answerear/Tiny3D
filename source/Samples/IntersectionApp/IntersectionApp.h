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

#ifndef __INTERSECTION_APP_H__
#define __INTERSECTION_APP_H__


#include <Tiny3D.h>
#include "../Common/SampleApp.h"


class IntersectionApp : public SampleConsoleApp
{
public:
    IntersectionApp();
    ~IntersectionApp() override;

protected:  // from Tiny3D::Application
    bool applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

    void applicationDidEnterBackground() override;

    void applicationWillEnterForeground() override;

    void applicationWillTerminate() override;

    void applicationLowMemory() override;

    // 射线和三角形相交检测
    void testRayTriangle();

    // 射线和平面相交检测
    void testRayPlane();

    // 射线和球体相交检测
    void testRaySphere();

    // 射线和AABB相交检测
    void testRayAabb();

    // 射线和OBB相交检测
    void testRayObb();

    // 两个球相交检测
    void testSphereSphere();

    // 球和平面相交检测
    void testSpherePlane();

    // AABB和平面相交检测
    void testAabbPlane();

    // OBB和平面相交检测
    void testObbPlane();

    // 视锥体和球相交检测
    void testFrustumSphere();

    // 视锥体和AABB相交检测
    void testFrustumAabb();

    // 视锥体和OBB相交检测
    void testFrustumObb();

    void buildFrustum(Tiny3D::Frustum &frustum);
};


#endif  /*__INTERSECTION_APP_H__*/
