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

#ifndef __T3D_NODE_H__
#define __T3D_NODE_H__


#include "Misc/T3DObject.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Node : public Object
    {
    public:
        enum ID
        {
            E_NID_AUTOMATIC = -1,     /// generate node id automatically.
            E_NID_INVALID = 0,          /// invalid node id
        };

        enum Type
        {
            E_NT_UNKNOWN = 0,

            E_NT_BONE,

            // For SGTransformNode
            E_NT_TRANSFORM,

            // All node derived from SGNode
            E_NT_CAMERA,
            E_NT_MODEL,

            // All node derived from SGRenderable
            E_NT_LIGHT,
            E_NT_GEOMETRY,
            E_NT_MESH,
            E_NT_SPHERE,
            E_NT_BOX,
            E_NT_SKELETON,
            E_NT_AXIS,
            E_NT_QUAD,
        };

        virtual ~Node();

        virtual Type getNodeType() const = 0;

        uint32_t getNodeID() const;

        void setName(const String &name);
        const String &getName() const;

        virtual void addChild(const NodePtr &node);
        virtual void removeChild(const NodePtr &node, bool cleanup);
        virtual void removeChild(uint32_t nodeID, bool cleanup);

        virtual void removeAllChildren(bool cleanup);

        void removeFromParent(bool cleanup);

        const Children &getChildren() const;

        const NodePtr &getChild(uint32_t unNodeID) const;
        NodePtr getChild(uint32_t unNodeID);
        const NodePtr &getChild(const String &name) const;
        NodePtr getChild(const String &name);

        const NodePtr &getParent() const;
        NodePtr getParent();

        virtual NodePtr clone() const = 0;

    protected:
        Node(uint32_t uID = E_NID_AUTOMATIC);

        virtual void cloneProperties(const NodePtr &node) const;

        virtual void onAttachParent(const NodePtr &parent);
        virtual void onDetachParent(const NodePtr &parent);

    private:
        uint32_t makeGlobalID() const;

    private:
        uint32_t    mID;            /// 结点ID
        String      mName;          /// 给该结点命名

        NodePtr     mParent;        /// 父结点

    protected:
        Children    mChildren;      /// 子结点
    };
}


#include "T3DNode.inl"


#endif  /*__T3D_NODE_H__*/
