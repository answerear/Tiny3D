
#ifndef __T3D_SG_NODE_H__
#define __T3D_SG_NODE_H__


#include "Misc/T3DObject.h"
#include "Math/T3DMath.h"
#include "Math/T3DVector3.h"
#include "Math/T3DMatrix3.h"
#include "Math/T3DMatrix4.h"
#include "Misc/T3DSmartPointer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGNode : public Object
    {
        friend class SceneManager;

        T3D_DISABLE_COPY(SGNode);

    public:
        enum ID
        {
            E_NID_AUTOMATIC = -1,     /// generate node id automatically.
            E_NID_INVALID = 0,          /// invalid node id
        };

        enum Type
        {
            E_NT_UNKNOWN = 0,

            // For SGTransformNode
            E_NT_TRANSFORM,

            // All node derived from SGVisual
            E_NT_CAMERA,
            E_NT_LIGHT,
            E_NT_MESH,
            E_NT_SKYBOX,
            E_NT_SKYDOOM,
            E_NT_SKYPLANE,
            E_NT_BILLBOARD,
            E_NT_SPRITE,
            E_NT_INDICATOR,
            E_NT_TERRAIN,

            // All node derived from SGRenderable
            E_NT_TILE,
            E_NT_PLANE,
            E_NT_SPHERE,
            E_NT_BOX,
            E_ET_AXIS,
            E_NT_QUAD,
            E_NT_SUBMESH,
        };

    protected:
        /**
         * @brief Constructor.
         * @remarks
         *      All derived class should implement a static create() method to
         *      create the concrete object.
         */
        SGNode(uint32_t unID = E_NID_AUTOMATIC);

    public:
        virtual ~SGNode();

        virtual Type getNodeType() const = 0;

        uint32_t getNodeID() const;

        void setName(const String &name);
        const String &getName() const;

        void setUserData(long_t data);
        long_t getUserData() const;

        void setUserObject(const ObjectPtr &rkObject);
        const ObjectPtr &getUserObject() const;
        ObjectPtr getUserObject();

        virtual void addChild(const SGNodePtr &node);
        virtual void removeChild(const SGNodePtr &node);
        virtual void removeChild(uint32_t nodeID);

        virtual void removeAllChildren();

        void removeFromParent();

        const SGChildren &getChildren() const;

        const SGNodePtr &getChild(uint32_t unNodeID) const;
        SGNodePtr getChild(uint32_t unNodeID);
        const SGNodePtr &getChild(const String &name) const;
        SGNodePtr getChild(const String &name);

        const SGNodePtr &getParent() const;
        SGNodePtr getParent();

        void setDirty(bool isDirty, bool recursive = false);
        bool isDirty() const;

        virtual SGNodePtr clone() const = 0;

    protected:
        /** 
         * @brief Perform self transformation and recursively call all children
         *      updateTransform()
         */
        virtual void updateTransform();

        /**
         * @brief Perform frustum culling against a frustum bound and recursively
         *      call all children frustumCulling()
         */
        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue);

        virtual void cloneProperties(SGNode *node) const;

        virtual void onAttachParent(const SGNodePtr &parent);
        virtual void onDetachParent(const SGNodePtr &parent);

        uint32_t makeGlobalID() const;

    private:
        uint32_t    mID;            /// Identify of this node
        String      mName;          /// Name of this node

        long_t      mUserData;      /// Store user data

        ObjectPtr   mUserObject;   /// Pointer to user object

        SGNodePtr   mParent;       /// Pointer to parent scene node

        SGChildren  mChildren;      /// List of containing all children scene node

        bool        mIsDirty;
    };
}


#include "T3DSGNode.inl"


#endif  /*__T3D_SG_NODE_H__*/
