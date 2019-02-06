/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
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


#ifndef __T3D_RENDERER_H__
#define __T3D_RENDERER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DRenderWindow.h"


namespace Tiny3D
{
    /**
     * @brief 渲染器抽象类，负责提供抽象渲染接口，具体渲染器实现这些接口
     */
    class T3D_ENGINE_API Renderer : public Object
    {
    public:
        static const char * const REFERENCE3D;  /**< Tiny3D自带的软渲染器 */
        static const char * const DIRECT3D9;    /**< Direct3D9 渲染器 */
        static const char * const DIRECT3D11;   /**< Direct3D11 渲染器 */
        static const char * const OPENGL3;      /**< OpenGL 3.x 渲染器 */
        static const char * const OPENGLES2;    /**< OpenGL ES 2 渲染器 */
        static const char * const OPENGLES3;    /**< OpenGL ES 3 渲染器 */
        static const char * const VULKAN;       /**< Vulkan 渲染器 */
        static const char * const METAL;        /**< Metal 渲染器 */

        /**
         * @brief 渲染器能力值
         */
        enum Capability
        {
            E_CAP_RENDER_TO_TARGET = 0,     /**< 是否支持渲染到 surface */
            E_CAP_HARDWARE_TRANSFORM,       /**< 是否支持硬件变换 */
            E_CAP_MULTI_TEXTURE,            /**< 是否支持多重纹理 */
            E_CAP_MIP_MAP,                  /**< 是否支持 Mip Map */
            E_CAP_STENCIL_BUFFER,           /**< 是否支持 stencil buffer */
            E_CAP_VERTEX_SHADER_1_1,        /**< 是否支持 1.1 版本顶点着色器 */
            E_CAP_VERTEX_SHADER_2_0,        /**< 是否支持 2.0 版本顶点着色器 */
            E_CAP_VERTEX_SHADER_3_0,        /**< 是否支持 3.0 版本顶点着色器 */
            E_CAP_PIXEL_SHADER_1_1,         /**< 是否支持 1.1 版本片段着色器 */
            E_CAP_PIXEL_SHADER_1_2,         /**< 是否支持 1.2 版本片段着色器 */
            E_CAP_PIXEL_SHADER_1_3,         /**< 是否支持 1.3 版本片段着色器 */
            E_CAP_PIXEL_SHADER_1_4,         /**< 是否支持 1.4 版本片段着色器 */
            E_CAP_PIXEL_SHADER_2_0,         /**< 是否支持 2.0 版本片段着色器 */
            E_CAP_PIXEL_SHADER_3_0,         /**< 是否支持 3.0 版本片段着色器 */
            E_CAP_TEXTURE_NSQUARE,          /**< 是否支持非正方形纹理 */
            E_CAP_TEXTURE_NPOT,             /**< 是否支持非 2 次幂纹理 */
            E_CAP_FRAMEBUFFER,              /**< 是否支持帧缓冲 */
            E_CAP_MAX
        };

        /**
         * @breif 设置变换矩阵状态
         */
        enum TransformState
        {
            E_TS_VIEW = 0,      /**< 视口变换矩阵 */
            E_TS_WORLD,         /**< 世界变换矩阵 */
            E_TS_PROJECTION,    /**< 投影变换矩阵 */
            E_TS_MAX
        };

        /**
         * @brief 渲染图元类型
         */
        enum PrimitiveType
        {
            E_PT_POINT_LIST = 0,    /**< 点列表图元 */
            E_PT_LINE_LIST,         /**< 线列表图元 */
            E_PT_LINE_STRIP,        /**< 线带图元 */
            E_PT_TRIANGLE_LIST,     /**< 三角形列表图元 */
            E_PT_TRIANGLE_STRIP,    /**< 三角形带图元 */
            E_PT_TRIANGLE_FAN,      /**< 三角形扇形图元 */
            E_PT_MAX
        };

        /**
         * @brief 背面剔除模式
         */
        enum CullingMode
        {
            E_CULL_NONE = 0,        /**< 不做消隐面剔除 */
            E_CULL_CLOCKWISE,       /**< 按照顶点顺时针顺序的消隐面剔除 */
            E_CULL_ANTICLOCKWISE,   /**< 按照顶点逆时针顺序的消隐面剔除 */
            E_CULL_MAX
        };

        /**
         * @brief 渲染模式
         */
        enum RenderMode
        {
            E_RM_POINT = 0,         /**< 顶点模式 */
            E_RM_WIREFRAME,         /**< 线框模式 */
            E_RM_SOLID,             /**< 着色模式 */
            E_RM_MAX
        };

        /**
         * @brief 构造函数
         */
        Renderer();

        /**
         * @brief 析构函数
         */
        virtual ~Renderer();

        /**
         * @brief 初始化渲染器
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult init() = 0;

        /**
         * @brief 销毁渲染器
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult destroy() = 0;

        /**
         * @brief 获取渲染器名称
         */
        const String &getName() const;

        /**
         * @brief 渲染一帧
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult render();

        /**
         * @brief 创建渲染窗口
         * @param [in] name : 用于标识窗口的名称，不需要跟窗口标题栏相同
         * @param [in] param : 创建渲染窗口必要数据
         * @param [in] paramEx : 创建渲染窗口额外数据
         * @return 调用成功返回一个渲染窗口对象，失败返回nullptr
         * @remarks 具体渲染系统实现本接口以创建渲染系统相关的渲染窗口对象
         */
        virtual RenderWindowPtr createRenderWindow(
            const String &name,
            const RenderWindowCreateParam &param,
            const RenderWindowCreateParamEx &paramEx) = 0;

        /**
         * @brief 关联渲染目标到渲染器上
         * @param [in] target : 渲染目标
         * @return 成功返回 T3D_OK
         */
        virtual TResult attachRenderTarget(RenderTargetPtr target);

        /**
         * @brief 从渲染器上分离渲染目标
         * @param [in] name : 渲染目标名称
         * @return 成功返回 T3D_OK
         */
        virtual TResult detachRenderTarget(const String &name);

        /**
         * @brief 获取渲染器上绑定的渲染目标
         * @param [in] name : 渲染目标名称
         * @return 成功返回渲染目标对象
         */
        RenderTargetPtr getRenderTarget(const String &name);

        /**
         * @brief 开始渲染
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult beginRender() = 0;

        /**
         * @brief 结束渲染
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult endRender() = 0;

        /**
         * @brief 查询渲染器能力
         * @param [in] cap : 能力枚举值
         * @return 具备cap对应的能力则返回true，否则返回false
         * @see Renderer::Capability
         */
        virtual bool queryCapability(Capability cap) const = 0;

        /**
         * @brief 开启或者关闭某种能力
         * @param [in] cap : 能力枚举值
         * @param [in] enabled : 开关标记
         * @return 调用成功返回 T3D_OK
         * @remarks 如果具体渲染系统本身不具备相应的能力，则本接口调用无效
         * @see Renderer::Capability
         */
        virtual TResult enableCapability(Capability cap, bool enabled) = 0;

        /**
         * @brief 设置变换矩阵
         * @param [in] state : 变换矩阵类型
         * @param [in] mat : 变换矩阵
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult setTransform(TransformState state, 
            const Matrix4 &mat) = 0;

        /**
         * @brief 设置视图变换矩阵
         * @param [in] mat : 变换矩阵
         * @return 调用成功返回 T3D_OK
         */
        TResult setViewTransform(const Matrix4 &mat);

        /**
         * @brief 设置世界变换矩阵
         * @param [in] mat : 变换矩阵
         * @return 调用成功返回 T3D_OK
         */
        TResult setWorldTransform(const Matrix4 &mat);

        /**
         * @brief 设置投影变换矩阵
         * @param [in] mat : 变换矩阵
         * @return 调用成功返回 T3D_OK
         */
        TResult setProjectionTransform(const Matrix4 &mat);

        /**
         * @brief 获取对应类型的变换矩阵
         * @param [in] state : 变换矩阵类型
         * @return 返回对应类型的变换矩阵
         */
        virtual const Matrix4 &getTransform(TransformState state) const = 0;

        /**
         * @brief 设置裁剪模式
         * @param [in] mode : 裁剪模式
         * @return 成功返回 T3D_OK
         */
        virtual TResult setCullingMode(CullingMode mode) = 0;

        /**
         * @brief 获取裁剪模式
         */
        virtual CullingMode getCullingMode() const;

        /**
         * @brief 设置渲染模式
         * @param [in] mode : 渲染模式
         * @return 成功返回 T3D_OK
         * @see enum RenderMode
         */
        virtual TResult setRenderMode(RenderMode mode) = 0;

        /**
         * @brief 获取渲染模式
         */
        virtual RenderMode getRenderMode() const;

        /**
         * @brief 设置渲染视口
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult setViewport(ViewportPtr viewport) = 0;

        /**
         * @brief 获取渲染视口
         */
        virtual ViewportPtr getViewport() const;

        /**
         * @brief 绘制顶点数组
         * @param [in] vao : 顶点数组对象
         * @return 调动成功返回 T3D_OK
         */
        virtual TResult drawVertexArray(VertexArrayPtr vao) = 0;

        /**
         * @brief 绘制顶点列表
         * @param [in] priType : 图元类型
         * @param [in] vbo : 顶点缓冲
         * @param [in] startIdx : 顶点缓冲区的起始位置
         * @param [in] priCount : 图元数量
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult drawVertexList(PrimitiveType priType,
            HardwareVertexBufferPtr vbo, size_t startIdx, size_t priCount) = 0;

        /**
         * @brief 绘制索引列表
         * @param [in] priType : 图元类型
         * @param [in] vbo : 顶点缓冲
         * @param [in] ibo : 索引缓冲
         * @param [in] startIdx : 顶点索引起始位置偏移
         * @param [in] priCount : 图元数量
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult drawIndexList(PrimitiveType priType,
            HardwareVertexBufferPtr vbo, HardwareIndexBufferPtr ibo,
            size_t startIdx, size_t priCount) = 0;

    protected:
        typedef TMap<String, RenderTargetPtr>       RenderTargetList;
        typedef RenderTargetList::iterator          RenderTargetListItr;
        typedef RenderTargetList::const_iterator    RenderTargetListConstItr;
        typedef RenderTargetList::value_type        RenderTargetListValue;

        String              mName;              /**< 渲染器名称 */

        RenderTargetList    mRenderTargets;     /**< 渲染目标列表 */

        ViewportPtr         mViewport;          /**< 当前渲染视口对象 */

        CullingMode         mCullingMode;       /**< 裁剪模式 */
        RenderMode          mRenderMode;        /**< 渲染模式 */
    };
}


#include "T3DRenderer.inl"


#endif  /*__T3D_RENDERER_H__*/
