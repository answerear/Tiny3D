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


#ifndef __T3D_VIEWPORT_H__
#define __T3D_VIEWPORT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    TSTRUCT()
    struct T3D_ENGINE_API Viewport
    {
        /// 左上角 X 坐标比例，取值范围 [0,1]
        TPROPERTY()
        Real    Left {0.0f};
        /// 左上角 Y 坐标比例，取值范围 [0,1]
        TPROPERTY()
        Real    Top {0.0f};
        /// 宽度比例，取值范围 [0,1]
        TPROPERTY()
        Real    Width {1.0f};
        /// 高度比例，取值范围 [0,1]
        TPROPERTY()
        Real    Height {1.0f};
        /// 最小深度值
        TPROPERTY()
        Real    MinDepth {0.0f};
        /// 最大深度值
        TPROPERTY()
        Real    MaxDepth {1.0f};
    };
    
    // /**
    //  * @brief 渲染视口
    //  */
    // class T3D_ENGINE_API Viewport : public Object
    // {
    // public:
    //     /**
    //      * @brief 创建视口对象
    //      * @param [in] target : 跟本视口关联的渲染目标对象
    //      * @param [in] left : 视口左边在渲染目标区域的比例值
    //      * @param [in] top : 视口上边在渲染目标区域的比例值
    //      * @param [in] width : 视口宽度占渲染目标区域的比例值
    //      * @param [in] height : 视口高度占渲染目标区域的比例值
    //      * @param [in] zOrder : 视口深度序列值
    //      */
    //     static ViewportPtr create(RenderTargetPtr target, Real left, Real top, Real width, Real height, long_t zOrder);
    //
    //     /**
    //      * @brief 析构函数
    //      */
    //     virtual ~Viewport();
    //
    //     /**
    //      * @brief 获取观察视口对应的相机
    //      */
    //     // CameraPtr getCamera() const;
    //
    //     /**
    //      * @brief 渲染
    //      */
    //     void render();
    //
    //     /**
    //      * @brief 获取渲染目标对象
    //      */
    //     RenderTargetPtr getRenderTarget() const;
    //
    //     /**
    //      * @brief 获取左边在渲染目标区域的比值
    //      */
    //     Real getLeft() const;
    //
    //     /**
    //      * @brief 获取上边在渲染目标区域的比值
    //      */
    //     Real getTop() const;
    //
    //     /**
    //      * @brief 获取宽度在渲染目标区域的比值
    //      */
    //     Real getWidth() const;
    //
    //     /**
    //      * @brief 获取高度在渲染目标区域的比值
    //      */
    //     Real getHeight() const;
    //
    //     /**
    //      * @brief 获取视口实际左边位置
    //      */
    //     size_t getActualLeft() const;
    //
    //     /**
    //      * @brief 获取视口实际上边位置
    //      */
    //     size_t getActualTop() const;
    //
    //     /**
    //      * @brief 获取视口实际宽度
    //      */
    //     size_t getActualWidth() const;
    //
    //     /**
    //      * @brief 获取视口实际高度
    //      */
    //     size_t getActualHeight() const;
    //
    //     /**
    //      * @brief 获取视口变换矩阵
    //      */
    //     const Matrix4 &getViewportMatrix() const;
    //
    //     /**
    //      * @brief 设置视口位置和大小
    //      * @param [in] left : 视口左边在渲染目标区域的相对位置
    //      * @param [in] top : 视口上边在渲染目标区域的相对位置
    //      * @param [in] width : 视口宽度在渲染目标区域的相对值
    //      * @param [in] height : 视口高度在渲染目标区域的相对值
    //      */
    //     void setDimensions(Real left, Real top, Real width, Real height);
    //
    //     /**
    //      * @brief 设置清除标记
    //      * @param [in] flags : 清除标记
    //      * @return void
    //      * @see Renderer::ClearFlags
    //      */
    //     void setClearFlags(uint32_t flags);
    //
    //     /**
    //      * @brief 获取清除标记
    //      * @return 返回清除标记
    //      */
    //     uint32_t getClearFlags() const;
    //
    //     /**
    //      * @brief 设置用于清除的 z-buffer 值
    //      * @param [in] z : z值
    //      * @return void
    //      */
    //     void setClearZ(Real z);
    //
    //     /**
    //      * @brief 获取用于清除的 z-buffer 值
    //      */
    //     Real getClearZ() const;
    //
    //     /**
    //      * @brief 设置背景颜色
    //      * @param [in] color : 颜色值
    //      * @return void
    //      */
    //     void setBkgndColor(const ColorRGB &color);
    //
    //     /**
    //      * @brief 获取背景颜色
    //      * @return 返回背景颜色
    //      */
    //     const ColorRGB &getBkgndColor() const;
    //
    // protected:
    //     /**
    //      * @brief 构造函数
    //      * @param [in] target : 跟本视口关联的渲染目标对象
    //      * @param [in] left : 视口左边在渲染目标区域的比例值
    //      * @param [in] top : 视口上边在渲染目标区域的比例值
    //      * @param [in] width : 视口宽度占渲染目标区域的比例值
    //      * @param [in] height : 视口高度占渲染目标区域的比例值
    //      * @param [in] zOrder : 视口深度序列值
    //      */
    //     Viewport(RenderTargetPtr target, Real left, Real top, Real width, Real height, long_t zOrder);
    //
    //     /**
    //      * @brief 更新视口位置和大小
    //      */
    //     void updateDimensions();
    //
    // protected:
    //     // CameraPtr         mCamera;        /**< 关联本视口对应的相机 */
    //     RenderTarget        *mRenderTarget; /**< 关联本视口对应的渲染目标 */
    //
    //     Real        mLeft;          /**< 视口左边在渲染目标区域的相对位置 */
    //     Real        mTop;           /**< 视口上边在渲染目标区域的相对位置 */
    //     Real        mWidth;         /**< 视口宽度在渲染目标区域的相对值 */
    //     Real        mHeight;        /**< 视口高度在渲染目标区域的相对值 */
    //
    //     size_t      mActualLeft;    /**< 视口实际的左边位置 */
    //     size_t      mActualTop;     /**< 视口实际的上边位置 */
    //     size_t      mActualWidth;   /**< 视口实际的宽度 */
    //     size_t      mActualHeight;  /**< 视口实际的高度 */
    //
    //     uint32_t    mClearFlags;    /**< 清除标记 */
    //     Real        mClearZ;        /**< 用于清除的 z-buffer 值 */
    //
    //     ColorRGB    mBkgndColor;    /**< 背景颜色 */
    //
    //     Matrix4     mMatrix;        /**< 视口变换矩阵 */
    // };
}


#include "T3DViewport.inl"


#endif  /*__T3D_VIEWPORT_H__*/
