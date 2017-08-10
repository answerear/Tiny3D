/***************************************************************************************************
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
 **************************************************************************************************/

#ifndef __T3D_SG_SPRITE_H__
#define __T3D_SG_SPRITE_H__


#include "SceneGraph/T3DSGRenderable.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGSprite : public SGRenderable
    {
    public:
        /**
         * @brief 创建sprite结点对象
         * @param [in] uID : 结点唯一标识，默认为引擎自动生成
         */
        static SGSpritePtr create(uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief 析构函数
         */
        virtual ~SGSprite();

        /**
         * @brief 从父类继承，重写返回对应的结点类型
         * @return 返回结点类型
         * @see enum Type
         */
        virtual Type getNodeType() const override;

        /**
         * @brief 从父类继承，重写父类克隆方法，以实现本类对象的复制操作
         * @return 返回一个新的3D变换结点对象
         */
        virtual NodePtr clone() const override;

        /**
         * @brief 设置纹理UV坐标
         * @param [in] index : 顶点索引
         * @param [in] uv : 纹理坐标
         * @return void
         */
        void setTextureUV(size_t index, const Vector2 &uv);

        /**
         * @brief 设置纹理矩形区域
         * @param [in] rect : 纹理区域
         * @return void
         * @note 可以使用本接口直接设置纹理UV
         */
        void setTextureUV(const Rect &rect);

        /**
         * @brief 设置某个顶点颜色，默认都是白色
         * @param [in] index : 顶点索引
         * @param [in] color : 顶点颜色
         * @return void
         */
        void setVertexColor(size_t index, const Color4 &color);

        /**
         * @brief 设置所有顶点颜色，默认都是白色
         * @param [in] color : 顶点颜色
         * @return void
         */
        void setVerticesColor(const Color4 &color);

        /**
         * @brief 设置锚点位于精灵区域的相对位置
         * @param [in] pos : 锚点位置，单位是百分比，数值介于0到1之间
         * @return void
         */
        void setAnchorPos(const Vector2 &pos);

        /**
         * @brief 获取锚点位置
         * @return 返回锚点位置，返回的是百分比，不是绝对值，数值介于0到1之间
         */
        const Vector2 &getAnchorPos() const;

        /**
         * @brief 设置精灵大小
         * @param [in] size : 大小值
         * @return void
         * @note 这个大小不是直接缩放回来的大小，而是实际屏幕上的像素大小
         */
        void setSize(const Size &size);

        /**
         * @brief 获取精灵大小
         * @return 返回精灵大小
         * @note 这个大小不是直接缩放回来的大小，而是实际屏幕上的像素大小
         */
        const Size &getSize() const;

    protected:
        /**
         * @brief 默认构造函数
         */
        SGSprite(uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief 初始化
         * @return 初始化成功返回true，失败返回false
         */
        bool init();

        /**
         * @brief 从父类继承，重写以实现物体视景体外的剔除
         * @param [in] bound : 视景体区域
         * @param [in] queue : 渲染队列
         * @return void
         */
        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

        /**
         * @brief 从父类继承，返回材质对象给渲染器
         */
        virtual MaterialPtr getMaterial() const override;

        /**
         * @brief 从父类继承，返回渲染图元类型给渲染器
         */
        virtual Renderer::PrimitiveType getPrimitiveType() const override;

        /**
         * @brief 从父类继承，返回顶点数据给渲染器
         */
        virtual VertexDataPtr getVertexData() const override;

        /**
         * @brief 从父类继承，返回顶点索引数据给渲染器
         */
        virtual IndexDataPtr getIndexData() const override;

        /**
         * @brief 从父类继承，返回是否使用顶点索引缓冲
         */
        virtual bool isIndicesUsed() const override;

    protected:
        /**
         * @brief 精灵使用的顶点结构定义
         */
        struct Vertex
        {
            Vector3     position;       /// 顶点位置
            Vector2     texcoord;       /// 纹理UV坐标
            Color4      color;          /// 顶点颜色
        };

        VertexDataPtr       mVertexData;    /// 顶点数据
        MaterialPtr         mMaterial;      /// 材质数据
        Vector2             mAnchorPos;     /// 锚点位置
        Size                mSize;
    };
}


#endif  /*__T3D_SG_SPRITE_H__*/
