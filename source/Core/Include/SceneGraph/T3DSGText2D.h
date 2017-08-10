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
#include "Resource/T3DFont.h"


namespace Tiny3D
{
    /**
     * @class SGText2D
     * @brief 用于显示2D文本的渲染对象
     */
    class T3D_ENGINE_API SGText2D : public SGRenderable
    {
    public:
        /**
         * @brief 创建2D文本渲染结点对象
         * @param [in] text : 文本字符串，默认空串
         * @param [in] fontSize : 字体大小，默认10号
         * @param [in] textColor : 文本颜色，默认白色
         * @param [in] fontName : 字体字库名称，默认用FontManager里面设置的
         * @param [in] uID : 结点唯一标识，默认为引擎自动生成
         * @return 创建成功返回一个新对象，创建失败则返回nullptr
         * @see void FontManager::setDefaultFontName(const String &fontName)
         */
        static SGText2DPtr create(const String &text = "", size_t fontSize = 10, 
            const Color4 &textColor = Color4::WHITE, const String &fontName = "", 
            uint32_t uID = E_NID_AUTOMATIC);

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
         * @param [in] text : 文本字符串
         * @return void
         * @see const String &getText() const
         */
        void setText(const String &text);

        /**
         * @brief 获取文本
         * @return 返回文本字符串
         * @see void setText(const String &text)
         */
        const String &getText() const { return mText; }

        /**
         * @brief 设置文本颜色
         * @param [in] color : 文本颜色
         * @return void
         * @see const Color4 &getTextColor() const
         */
        void setTextColor(const Color4 &color);

        /**
         * @brief 获取文本颜色
         * @return 返回文本颜色
         * @see void setTextColor(const Color4 &color)
         */
        const Color4 &getTextColor() const;

        /**
         * @brief 设置锚点位于文本区域的相对位置
         * @param [in] pos : 锚点位置，单位是百分比，数值介于0到1之间
         * @return void
         * @see const Vector2 &getAnchorPos() const
         */
        void setAnchorPos(const Vector2 &pos);

        /**
         * @brief 获取锚点位置
         * @return 返回锚点位置，返回的是百分比，不是绝对值，数值介于0到1之间
         * @see void setAnchorPos(const Vector2 &pos)
         */
        const Vector2 &getAnchorPos() const;

    protected:
        /**
         * @brief 默认构造函数
         */
        SGText2D(uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief 初始化
         * @return 初始化成功返回true，失败返回false
         */
        bool init(const String &text, size_t fontSize, const Color4 &texColor, const String &fontName);

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

        /**
         * @brief 根据文本更新顶点等各种字符渲染相关信息
         * @return 更新成功返回true，否则返回false
         */
        bool updateVertices();

    protected:
        struct Vertex
        {
            Vector3     position;
            Vector2     texcoord;
            uint32_t    diffuse;
        };

        VertexDataPtr       mVertexData;    /// 顶点数据
        IndexDataPtr        mIndexData;     /// 索引数据
        MaterialPtr         mMaterial;      /// 材质
        FontPtr             mFont;          /// 文本对应的字体
        Vector2             mAnchorPos;     /// 锚点位置
        Size                mSize;          /// 字符串渲染区域的大小
        String              mText;          /// 文本字符串
        Font::CharSet       mCharSet;       /// 字符串对应的字符集
    };
}


#endif  /*__T3D_SG_SPRITE_H__*/
