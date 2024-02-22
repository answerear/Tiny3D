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

#include "RHI/T3DRHIRenderer.h"
#include "RHI/T3DRHIContext.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const RHIRenderer::NULLRENDERER = "NullRenderer";
    const char * const RHIRenderer::REFERENCE3D = "Reference3D";
    const char * const RHIRenderer::DIRECT3D9 = "Direct3D9";
    const char * const RHIRenderer::DIRECT3D11 = "Direct3D11";
    const char * const RHIRenderer::OPENGL3 = "OpenGL 3.x";
    const char * const RHIRenderer::OPENGLES2 = "OpenGL ES 2";
    const char * const RHIRenderer::OPENGLES3 = "OpenGL ES 3";
    const char * const RHIRenderer::VULKAN = "Vulkan";
    const char * const RHIRenderer::METAL = "Metal";

    //--------------------------------------------------------------------------
}
