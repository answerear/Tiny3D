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


#include "T3DGL3PRenderer.h"
#include "T3DGL3PRenderWindow.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(GL3PRenderer);

    GL3PRenderer::GL3PRenderer()
    {
    }

    GL3PRenderer::~GL3PRenderer()
    {
    }

    String GL3PRenderer::getName() const
    {
        return DIRECT3D9;
    }

    RenderWindow *GL3PRenderer::createRenderWindow(
        const RenderWindowCreateParam &rkCreateParam,
        const RenderWindowCreateParamEx &rkCreateParamEx)
    {
        RenderWindow *window = new GL3PRenderWindow();
        window->create("MainWindow", rkCreateParam, rkCreateParamEx);
        attachRenderTarget(window);

        return window;
    }

    bool GL3PRenderer::initialize()
    {
        

        return true;
    }

    void GL3PRenderer::uninitialize()
    {
        
    }

    bool GL3PRenderer::beginRender(const Color4 &bkgndColor)
    {
        float r = float(bkgndColor.red()) / 256.0f;
        float g = float(bkgndColor.green()) / 256.0f;
        float b = float(bkgndColor.blue()) / 256.0f;
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        return true;
    }

    bool GL3PRenderer::endRender()
    {
        return true;
    }

    bool GL3PRenderer::queryCapability(Capability cap)
    {
        return false;
    }

    void GL3PRenderer::enableCapability(Capability cap, bool enabled /* = true */)
    {

    }

    void GL3PRenderer::setTransform(TransformState state, const Matrix4 &mat)
    {
        
    }

    const Matrix4 &GL3PRenderer::getTransform(TransformState state) const
    {
        return Matrix4::IDENTITY;
    }

    void GL3PRenderer::setLightEnabled(bool enable)
    {
        
    }

    void GL3PRenderer::setAmbientLight(const Color4 &ambient)
    {
        
    }

    void GL3PRenderer::addDynamicLight(size_t index, const SGLightPtr light)
    {
        
    }

    void GL3PRenderer::removeDynamicLight(size_t index)
    {

    }

    void GL3PRenderer::removeAllDynamicLights()
    {

    }

    void GL3PRenderer::setMaterial(const MaterialPtr &material)
    {
        
    }

    void GL3PRenderer::setCullingMode(CullingMode mode)
    {
        
    }

    void GL3PRenderer::setRenderMode(RenderMode mode)
    {
        
    }

    void GL3PRenderer::setViewport(const ViewportPtr &viewport)
    {
        glViewport(viewport->getActualLeft(), viewport->getActualTop(), viewport->getActualWidth(), viewport->getActualHeight());
    }

    void GL3PRenderer::drawVertexList(PrimitiveType primitiveType,
        const VertexDataPtr &vertexData, uint32_t startIdx,
        uint32_t primitiveCount)
    {
        
    }

    void GL3PRenderer::drawIndexList(PrimitiveType primitiveType,
        const VertexDataPtr &vertexData, const IndexDataPtr &indexData,
        uint32_t startIdx, uint32_t pritimitiveCount)
    {
        
    }

    void GL3PRenderer::makeProjectionMatrix(const Radian &rkFovY, Real aspect,
        Real nearDist, Real farDist, bool ortho, Matrix4 &mat)
    {
        
    }

    void GL3PRenderer::makeViewportMatrix(ViewportPtr viewport, Matrix4 &mat)
    {
        mat.makeZero();
        mat[0][0] = viewport->getActualWidth() * Real(0.5);
        mat[1][1] = -viewport->getActualHeight() * Real(0.5);
        mat[2][2] = Real(1.0);
        mat[3][3] = Real(1.0);
        mat[0][3] = viewport->getActualLeft() + viewport->getActualWidth() * Real(0.5);
        mat[1][3] = viewport->getActualTop() + viewport->getActualHeight() * Real(0.5);
        mat[2][3] = Real(0.0);
    }

    void GL3PRenderer::updateFrustum(const Matrix4 &m, Plane *plane, size_t planeCount)
    {
        // 快速计算视棱锥六个裁剪平面原理：
        //
        //  假设点V'是投影变换后的点，V是投影变换前相机坐标系的点，M是投影变换矩阵，则可得：
        //      V' = M * V
        //  其中
        //      V' = (x' y' z' w')
        //
        //      V = (x y z w), (w = 1)
        //
        //          | m00 m01 m02 m03 |
        //      M = | m10 m11 m12 m13 |
        //          | m20 m21 m22 m23 |
        //          | m30 m31 m32 m33 |
        //  则
        //      | m00 m01 m02 m03 |   | x |   | x*m00 + y*m01 + z*m02 + w*m03 |   | V * row0 |
        //      | m10 m11 m12 m13 |   | y |   | x*m10 + y*m11 + z*m12 + w*m13 |   | V * row1 |
        //      | m20 m21 m22 m23 | * | z | = | x*m20 + y*m21 + z*m22 + w*m23 | = | V * row2 |
        //      | m30 m31 m32 m33 |   | w |   | x*m30 + y*m31 + z*m32 + w*m33 |   | V * row3 |
        //
        //  这里的*代表点积，而rowi = (mi0 mi1 mi2 mi3)
        // 
        //  经过转换后，V'在齐次裁剪空间里。这个空间实际上现在已经是一个轴对齐的盒子。
        //  如果V'在这个盒子空间里，则变换前的V点也在这个没有经过变换的盒子空间里。
        //  这里要分一下DX和OpenGL的区别，因为投影空间不一样：
        //
        //      1、在DX里，V'在这个空间里，则符合下面的不等式规则
        //              -w' < x' < w'
        //              -w' < y' < w'
        //               0 < z' < w'
        //          用 -w' < x' 推导：
        //              -(V * row3) < (V * row0)
        //          移动项得：
        //              0 < (V * row3) + (V * row0)
        //          合并同类项，最终可得：
        //              0 < V * (row3 + row0)
        //
        //          如此类推可得：
        //              left    :   0 < V * (row3 + row0)   a=m30+m00, b=m31+m01, c=m32+m02, d=m33+m03
        //              right   :   0 < V * (row3 - row0)   a=m30-m00, b=m31-m01, c=m32-m02, d=m33-m03
        //              bottom  :   0 < V * (row3 + row1)   a=m30+m10, b=m31+m11, c=m32+m12, d=m33+m13
        //              top     :   0 < V * (row3 - row1)   a=m30-m10, b=m31-m11, c=m32-m12, d=m33-m13
        //              near    :   0 < V * row2            a=m20,     b=m21,     c=m22,     d=m23
        //              far     :   0 < V * (row3 - row2)   a=m30-m20, b=m31-m21, c=m32-m22, d=m33-m23
        //

        T3D_ASSERT(planeCount == Frustum::E_MAX_FACE);

        // Left
        plane[Frustum::E_FACE_LEFT][0] = m[3][0] + m[0][0];
        plane[Frustum::E_FACE_LEFT][1] = m[3][1] + m[0][1];
        plane[Frustum::E_FACE_LEFT][2] = m[3][2] + m[0][2];
        plane[Frustum::E_FACE_LEFT][3] = m[3][3] + m[0][3];
        plane[Frustum::E_FACE_LEFT].normalize();

        // Right
        plane[Frustum::E_FACE_RIGHT][0] = m[3][0] - m[0][0];
        plane[Frustum::E_FACE_RIGHT][1] = m[3][1] - m[0][1];
        plane[Frustum::E_FACE_RIGHT][2] = m[3][2] - m[0][2];
        plane[Frustum::E_FACE_RIGHT][3] = m[3][3] - m[0][3];
        plane[Frustum::E_FACE_RIGHT].normalize();

        // Bottom
        plane[Frustum::E_FACE_BOTTOM][0] = m[3][0] + m[1][0];
        plane[Frustum::E_FACE_BOTTOM][1] = m[3][1] + m[1][1];
        plane[Frustum::E_FACE_BOTTOM][2] = m[3][2] + m[1][2];
        plane[Frustum::E_FACE_BOTTOM][3] = m[3][3] + m[1][3];
        plane[Frustum::E_FACE_BOTTOM].normalize();

        // Top
        plane[Frustum::E_FACE_TOP][0] = m[3][0] - m[1][0];
        plane[Frustum::E_FACE_TOP][1] = m[3][1] - m[1][1];
        plane[Frustum::E_FACE_TOP][2] = m[3][2] - m[1][2];
        plane[Frustum::E_FACE_TOP][3] = m[3][3] - m[1][3];
        plane[Frustum::E_FACE_TOP].normalize();

        // Near
        plane[Frustum::E_FACE_NEAR][0] = m[2][0];
        plane[Frustum::E_FACE_NEAR][1] = m[2][1];
        plane[Frustum::E_FACE_NEAR][2] = m[2][2];
        plane[Frustum::E_FACE_NEAR][3] = m[2][3];
        plane[Frustum::E_FACE_NEAR].normalize();

        // Far
        plane[Frustum::E_FACE_FAR][0] = m[3][0] - m[2][0];
        plane[Frustum::E_FACE_FAR][1] = m[3][1] - m[2][1];
        plane[Frustum::E_FACE_FAR][2] = m[3][2] - m[2][2];
        plane[Frustum::E_FACE_FAR][3] = m[3][3] - m[2][3];
        plane[Frustum::E_FACE_FAR].normalize();
    }
}
