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


#pragma once


#include "ImWidget.h"


namespace Tiny3D
{
    /**
     * 节点基类，使用者实现其定义接口
     */
    class ImTreeBarNode
    {
    public:
        virtual ~ImTreeBarNode() = default;

        /// 获取节点名称
        virtual const String &getName() const = 0;

        /// 获取父节点
        virtual ImTreeBarNode *getParent() const = 0;

        /// 获取指定名称的子节点
        virtual ImTreeBarNode *getChild(const String &name) const = 0;

        /// 获取所有子节点
        virtual TList<ImTreeBarNode*> getChildren() const = 0;

        /// 获取前一个兄弟节点
        virtual ImTreeBarNode *getPrevSibling() = 0;

        /// 获取后 一个兄弟节点
        virtual ImTreeBarNode *getNextSibling() = 0;
    };
    
    class TINYIMGUI_API ImTreeBar
        : public ImWidget
        , public EventHandler
    {
    public:
        ~ImTreeBar() override;

        WidgetType getWidgetType() const override { return WidgetType::kTreeBar; }

        TResult create(uint32_t id, const String &name, ImWidget *parent, ImTreeBarNode *root, const ImTreeBarNodeClickedCallback &clickedNode);

        /**
         * 设置选中节点
         * @param [in] node : 选中节点对象
         */
        void setSelectedNode(ImTreeBarNode *node);

        /**
         * 获取选中节点
         * @return 返回选中节点
         */
        ImTreeBarNode *getSelectedNode() const { return mSelectedNode; }
        
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;
        
    protected:
        using SelectedNodes = TList<ImTreeBarNode*>;
        
        /// 根节点
        ImTreeBarNode *mRoot {nullptr};
        /// 选中节点
        ImTreeBarNode *mSelectedNode {nullptr};
        /// 选中节点链
        SelectedNodes mSelectedNodes {};
        /// 点击节点回调
        ImTreeBarNodeClickedCallback mClickedNodeCB {nullptr};
    };
}
