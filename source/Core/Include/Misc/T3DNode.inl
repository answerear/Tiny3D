

namespace Tiny3D
{
    inline uint32_t Node::getNodeID() const
    {
        return mID;
    }

    inline void Node::setName(const String &name)
    {
        mName = name;
    }

    inline const String &Node::getName() const
    {
        return mName;
    }

    inline const Children &Node::getChildren() const
    {
        return mChildren;
    }

    inline const NodePtr &Node::getParent() const
    {
        return mParent;
    }

    inline NodePtr Node::getParent()
    {
        return mParent;
    }
}
