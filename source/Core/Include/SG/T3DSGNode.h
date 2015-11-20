
#ifndef __T3D_SG_NODE_H__
#define __T3D_SG_NODE_H__


#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGNode : public Object
    {
        friend class SceneGraphManager;

    public:
        enum ID
        {
            E_NID_AUTOMATIC = -1,     /// generate node id automatically.
            E_NID_INVALID = 0,          /// invalid node id
        };

        enum Type
        {
            E_NT_UNKNOWN = 0,
            E_NT_NODE,
            E_NT_TRANSFORM,
            E_NT_CAMERA,
            E_NT_LIGHT,
            E_NT_MESH,
            E_NT_SKYBOX,
            E_NT_SKYDOOM,
            E_NT_SKYPLANE,
            E_NT_BILLBOARD,
            E_NT_SPRITE,
            E_NT_SPHERE,
            E_NT_CUBE,
            E_NT_INDICATOR,
            E_NT_TERRAIN,
        };

        SGNode(uint32_t unID = E_NID_AUTOMATIC);
        virtual ~SGNode();

        Type getNodeType() const;

        uint32_t getNodeID() const;

        void setName(const String &name);
        const String &getName() const;

        void setUserData(long_t data);
        long_t getUserData() const;

        void setUserObject(Object *pObject);
        Object *getUserObject() const;

        virtual void addChild(SGNode *node);
        virtual void removeChild(SGNode *node);
        virtual void removeChild(uint32_t nodeID);

        virtual void removeAllChildren();

        void removeFromParent();

        const SGChildren &getChildren() const;

        SGNode *getChild(uint32_t unNodeID);
        SGNode *getChild(const String &name);

        SGNode *getParent();

    protected:
        virtual void update();

        virtual void onAttachParent(SGNode *parent);
        virtual void onDetachParent(SGNode *parent);

        uint32_t makeGlobalID() const;

    protected:
        Type        mType;          /// Type of this node

        uint32_t    mID;            /// Identify of this node
        String      mName;          /// Name of this node

        long_t      mUserData;      /// Store user data

        Object      *mUserObject;   /// Pointer to user object

        SGNode      *mParent;       /// Pointer to parent scene node

        SGChildren  mChildren;      /// List of containing all children scene node
    };
}


#include "T3DSGNode.inl"


#endif  /*__T3D_SG_NODE_H__*/
