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

#ifndef __SRGB_APP_H__
#define __SRGB_APP_H__


#include "../Common/SampleApp.h"
#include "SRGBTestBehaviour.h"


/**
 * \brief sRGB texture and render target verification chart
 * \remarks
 *   Rows from top to bottom, each band compared with the one above it :
 *   0 - gray ramp : UNORM reference | sRGB texture encoded back | sRGB texture as sampled (darker)
 *   1 - colour bars : UNORM reference | R8G8B8A8_SRGB | B8G8R8A8_SRGB
 *   2 - dark linear ramp stored in 8 bits : UNORM target (visible steps) | sRGB target (smooth)
 *   3 - checker : one pixel checker | sRGB bilinear average | UNORM bilinear average (too dark)
 *   The readback checks run once on the third frame and log PASS / FAIL.
 */
class SRGBApp : public SampleWindowApp
{
public:
    SRGBApp();
    ~SRGBApp() override;

protected:  /// from Tiny3D::Application
    TResult applicationDidFinishLaunching(int32_t argc, char *argv[]) override;
    void applicationWillTerminate() override;
    void onRender() override;
    void onPostRender() override;

    Tiny3D::SRGBTestBehaviourPtr mTest {nullptr};
};


#endif  /*__SRGB_APP_H__*/
