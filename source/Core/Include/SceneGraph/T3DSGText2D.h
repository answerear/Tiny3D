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

#ifndef __T3D_SG_TEXT2D_H__
#define __T3D_SG_TEXT2D_H__


#include "SceneGraph/T3DSGRenderable.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGText2D : public SGRenderable
    {
    public:
        /**
         * @brief 创建2D文本渲染结点对象
         * @param [in] uID : 结点唯一标识，默认为引擎自动生成
         */
        static SGText2DPtr create(uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief 析构函数
         */
        virtual ~SGText2D();

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
         * @brief 设置文本
         */
        void setText(const String &text);

        /**
         * @brief 获取文本
         */
        const String &getText() const;

    protected:
        /**
         * @brief 默认构造函数
         */
        SGText2D(uint32_t uID = E_NID_AUTOMATIC);

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
        VertexDataPtr       mVertexData;    /// 顶点数据
        MaterialPtr         mMaterial;      /// 材质
        FontPtr             mFont;          /// 文本对应的字体
        Vector2             mAnchorPos;     /// 锚点位置
    };
}


#endif  /*__T3D_SG_SPRITE_H__*/
