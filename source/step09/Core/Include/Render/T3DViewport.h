/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_VIEWPORT_H__
#define __T3D_VIEWPORT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @brief 渲染视口
     */
    class T3D_ENGINE_API Viewport : public Object
    {
    public:
        /**
         * @brief 创建视口对象
         * @param [in] camera : 跟本视口关联的相机对象
         * @param [in] target : 跟本视口关联的渲染目标对象
         * @param [in] left : 视口左边在渲染目标区域的比例值
         * @param [in] top : 视口上边在渲染目标区域的比例值
         * @param [in] width : 视口宽度占渲染目标区域的比例值
         * @param [in] height : 视口高度占渲染目标区域的比例值
         * @param [in] zOrder : 视口深度序列值
         */
        static ViewportPtr create(SGCameraPtr camera, RenderTargetPtr target, 
            Real left, Real top, Real width, Real height, long_t zOrder);

        /**
         * @brief 析构函数
         */
        virtual ~Viewport();

        /**
         * @brief 获取观察视口对应的相机
         */
        SGCameraPtr getCamera() const;

        /**
         * @brief 渲染
         */
        void render();

        /**
         * @brief 获取渲染目标对象
         */
        RenderTargetPtr getRenderTarget() const;

        /**
         * @brief 获取左边在渲染目标区域的比值
         */
        Real getLeft() const;

        /**
         * @brief 获取上边在渲染目标区域的比值
         */
        Real getTop() const;

        /**
         * @brief 获取宽度在渲染目标区域的比值
         */
        Real getWidth() const;

        /**
         * @brief 获取高度在渲染目标区域的比值
         */
        Real getHeight() const;

        /**
         * @brief 获取视口实际左边位置
         */
        size_t getActualLeft() const;

        /**
         * @brief 获取视口实际上边位置
         */
        size_t getActualTop() const;

        /**
         * @brief 获取视口实际宽度
         */
        size_t getActualWidth() const;

        /**
         * @brief 获取视口实际高度
         */
        size_t getActualHeight() const;

        /**
         * @brief 获取视口变换矩阵
         */
        const Matrix4 &getViewportMatrix() const;

        /**
         * @brief 设置视口位置和大小
         * @param [in] left : 视口左边在渲染目标区域的相对位置
         * @param [in] top : 视口上边在渲染目标区域的相对位置
         * @param [in] width : 视口宽度在渲染目标区域的相对值
         * @param [in] height : 视口高度在渲染目标区域的相对值
         */
        void setDimensions(Real left, Real top, Real width, Real height);

        /**
         * @brief 设置背景颜色
         * @param [in] color : 颜色值
         */
        void setBkgndColor(const Color4 &color);

        /**
         * @brief 获取背景颜色
         */
        const Color4 &getBkgndColor() const;

    protected:
        /**
         * @brief 构造函数
         * @param [in] camera : 跟本视口关联的相机对象
         * @param [in] target : 跟本视口关联的渲染目标对象
         * @param [in] left : 视口左边在渲染目标区域的比例值
         * @param [in] top : 视口上边在渲染目标区域的比例值
         * @param [in] width : 视口宽度占渲染目标区域的比例值
         * @param [in] height : 视口高度占渲染目标区域的比例值
         * @param [in] zOrder : 视口深度序列值
         */
        Viewport(SGCameraPtr camera, RenderTargetPtr target, Real left, 
            Real top, Real width, Real height, long_t zOrder);

        /**
         * @brief 更新视口位置和大小
         */
        void updateDimensions();

    protected:
        SGCameraPtr         mCamera;        /**< 关联本视口对应的相机 */
        RenderTargetPtr     mRenderTarget;  /**< 关联本视口对应的渲染目标 */

        Real        mLeft;          /**< 视口左边在渲染目标区域的相对位置 */
        Real        mTop;           /**< 视口上边在渲染目标区域的相对位置 */
        Real        mWidth;         /**< 视口宽度在渲染目标区域的相对值 */
        Real        mHeight;        /**< 视口高度在渲染目标区域的相对值 */

        size_t      mActualLeft;    /**< 视口实际的左边位置 */
        size_t      mActualTop;     /**< 视口实际的上边位置 */
        size_t      mActualWidth;   /**< 视口实际的宽度 */
        size_t      mActualHeight;  /**< 视口实际的高度 */

        Color4      mBkgndColor;    /**< 背景颜色 */

        Matrix4     mMatrix;        /**< 视口变换矩阵 */
    };
}


#include "T3DViewport.inl"


#endif  /*__T3D_VIEWPORT_H__*/
