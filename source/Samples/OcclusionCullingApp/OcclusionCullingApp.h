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

#ifndef __OCCLUSION_CULLING_APP_H__
#define __OCCLUSION_CULLING_APP_H__


#include "../Common/SampleApp.h"


/**
 * \brief Camera driving through a city block of towers, street lamps and cars
 * \remarks
 *   Exercises GPU queries (binary occlusion + predication, timestamps through the pipeline's
 *   GPUProfiler, pipeline statistics) and debug groups / resource names; see
 *   OcclusionCullingBehaviour. Direct3D11 and OpenGL 4 only.
 */
class OcclusionCullingApp : public SampleWindowApp
{
public:
    OcclusionCullingApp();
    ~OcclusionCullingApp() override;

protected:  /// from Tiny3D::Application
    TResult applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

    void applicationWillTerminate() override;
};


#endif  /*__OCCLUSION_CULLING_APP_H__*/
