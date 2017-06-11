

#include "T3DBone.h"


namespace Tiny3D
{
    BonePtr Bone::create(const String &name, const Matrix4 &offsetMatrix, const Matrix4 &localMatrix)
    {
        BonePtr bone = new Bone(name, offsetMatrix, localMatrix);
        if (bone != nullptr)
        {
            bone->release();
        }
        else
        {
            T3D_SAFE_RELEASE(bone);
        }
        return bone;
    }

    Bone::Bone()
        : Node()
        , mTranslation(Vector3::ZERO)
        , mScaling(Vector3(1.0, 1.0, 1.0))
        , mOrientation(Quaternion::IDENTITY)
        , mInverseBoneMatrix()
        , mBindposeMatrix()
        , mInverseDirty(true)
        , mIsDirty(true)
    {

    }

    Bone::Bone(const String &name, const Matrix4 &bindposeMatrix, const Matrix4 &localMatrix)
        : Node()
        , mTranslation(Vector3::ZERO)
        , mScaling(Vector3(1.0, 1.0, 1.0))
        , mOrientation(Quaternion::IDENTITY)
        , mInverseBoneMatrix(bindposeMatrix)
        , mBindposeMatrix(bindposeMatrix)
        , mInverseDirty(true)
        , mIsDirty(true)
    {
        setName(name);

        localMatrix.decomposition(mTranslation, mScaling, mOrientation);

//         Vector3 pos;
//         Vector3 scale;
//         Quaternion orientation;
//         bindposeMatrix.decomposition(pos, scale, orientation);
//         mInverseBoneMatrix.makeTransform(pos, Vector3::UNIT_SCALE, orientation);

//         Vector3 translation;
//         Vector3 scaling;
//         Quaternion orientation;
//         m.decomposition(translation, scaling, orientation);
// 
//         setPosition(translation);
//         setScale(scaling);
//         setOrientation(orientation);
    }

    Bone::~Bone()
    {

    }

    Node::Type Bone::getNodeType() const
    {
        return E_NT_BONE;
    }

    NodePtr Bone::clone() const
    {
        BonePtr bone = new Bone();
        if (bone != nullptr)
        {
            bone->release();
            cloneProperties(bone);
        }
        return bone;
    }

    void Bone::cloneProperties(const NodePtr &node) const
    {
        BonePtr bone = smart_pointer_cast<Bone>(node);

        bone->mTranslation = mTranslation;
        bone->mScaling = mScaling;
        bone->mOrientation = mOrientation;
        bone->mCombineTransform = mCombineTransform;
    }

    void Bone::onAttachParent(const NodePtr &parent)
    {
        Node::onAttachParent(parent);
        setDirty(true, true);
    }

    void Bone::setDirty(bool isDirty, bool recursive /* = false */)
    {
        mIsDirty = isDirty;

        if (recursive)
        {
            auto itr = mChildren.begin();

            while (itr != mChildren.end())
            {
                const BonePtr &bone = smart_pointer_cast<Bone>(*itr);
                bone->setDirty(isDirty, recursive);
                ++itr;
            }
        }
    }

    const Transform &Bone::getCombineTransform()
    {
        if (isDirty())
        {
            NodePtr parent = getParent();
            while (parent != nullptr && parent->getNodeType() != E_NT_BONE)
                parent = parent->getParent();

            if (parent != nullptr)
            {
                BonePtr bone = smart_pointer_cast<Bone>(parent);
                const Transform &transform = bone->getCombineTransform();
                mCombineTransform.applyTransform(transform, mTranslation, mOrientation, mScaling/*Vector3::UNIT_SCALE*/);
//                 if (getName() == String("shenti"))
//                 {
//                     T3D_LOG_INFO("skeleton [%s]", getName().c_str());
// 
//                     const Matrix4 &M = mCombineTransform.getAffineMatrix();
//                     T3D_LOG_INFO("matCombine : %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f",
//                         M[0][0], M[0][1], M[0][2], M[0][3],
//                         M[1][0], M[1][1], M[1][2], M[1][3],
//                         M[2][0], M[2][1], M[2][2], M[2][3],
//                         M[3][0], M[3][1], M[3][2], M[3][3]);
// 
//                     Matrix4 T(mTranslation), S(mScaling[0], mScaling[1], mScaling[2], 1.0), R(mOrientation);
//                     Matrix4 LocalM = T * R * S;
//                     T3D_LOG_INFO("local : %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f",
//                         LocalM[0][0], LocalM[0][1], LocalM[0][2], LocalM[0][3],
//                         LocalM[1][0], LocalM[1][1], LocalM[1][2], LocalM[1][3],
//                         LocalM[2][0], LocalM[2][1], LocalM[2][2], LocalM[2][3],
//                         LocalM[3][0], LocalM[3][1], LocalM[3][2], LocalM[3][3]);
// 
//                     const Matrix4 &ParentM = transform.getAffineMatrix();
//                     T3D_LOG_INFO("parent : %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f",
//                         ParentM[0][0], ParentM[0][1], ParentM[0][2], ParentM[0][3],
//                         ParentM[1][0], ParentM[1][1], ParentM[1][2], ParentM[1][3],
//                         ParentM[2][0], ParentM[2][1], ParentM[2][2], ParentM[2][3],
//                         ParentM[3][0], ParentM[3][1], ParentM[3][2], ParentM[3][3]);
// 
//                     Matrix4 MM = ParentM * LocalM;
//                     T3D_LOG_INFO("Parent * Local : %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f",
//                         MM[0][0], MM[0][1], MM[0][2], MM[0][3],
//                         MM[1][0], MM[1][1], MM[1][2], MM[1][3],
//                         MM[2][0], MM[2][1], MM[2][2], MM[2][3],
//                         MM[3][0], MM[3][1], MM[3][2], MM[3][3]);
//                 }
            }
            else
            {
//                 Vector3 pos, scale;
//                 Quaternion orientation;
//                 mWorldMatrix.decomposition(pos, scale, orientation);
// 
//                 Transform transform(pos, scale, orientation);
//                 mCombineTransform.applyTransform(transform, mTranslation, mOrientation, mScaling);
                mCombineTransform.setTranslate(mTranslation);
                mCombineTransform.setOrientation(mOrientation);
                mCombineTransform.setScale(mScaling/*Vector3::UNIT_SCALE*/);
                mCombineTransform.update();

//                 const Matrix4 &MM = mCombineTransform.getAffineMatrix();
//                 T3D_LOG_INFO("Root Translation (% 8f, % 8f, % 8f)", mTranslation[0], mTranslation[1], mTranslation[2]);
//                 T3D_LOG_INFO("Root Scale (% 8f, % 8f, % 8f)", mScaling[0], mScaling[1], mScaling[2]);
//                 T3D_LOG_INFO("Root Rotation ( % 8f, % 8f, % 8f, % 8f)", mOrientation[0], mOrientation[1], mOrientation[2], mOrientation[3]);
//                 T3D_LOG_INFO("Root : %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f",
//                     MM[0][0], MM[0][1], MM[0][2], MM[0][3],
//                     MM[1][0], MM[1][1], MM[1][2], MM[1][3],
//                     MM[2][0], MM[2][1], MM[2][2], MM[2][3],
//                     MM[3][0], MM[3][1], MM[3][2], MM[3][3]);
            }

            setDirty(false);
        }

        return mCombineTransform;
    }

    const Matrix4 &Bone::getInverseBoneMatrix()
    {
//         if (mInverseDirty)
//         {
//             NodePtr parent = getParent();
//             while (parent != nullptr && parent->getNodeType() != E_NT_BONE)
//                 parent = parent->getParent();
// 
//             const Matrix4 &combineMatrix = getCombineTransform().getAffineMatrix();
//             mInverseBoneMatrix = combineMatrix.inverseAffine();
//             mInverseDirty = false;
//         }

        return mInverseBoneMatrix;
    }

    void Bone::updateBone()
    {
        getCombineTransform();

//         auto itr = mChildren.begin();
// 
//         while (itr != mChildren.end())
//         {
//             const BonePtr &bone = smart_pointer_cast<Bone>(*itr);
//             bone->updateBone();
//             ++itr;
//         }
    }

    const Matrix4 &Bone::getBindPoseMatrix()
    {
        return mBindposeMatrix;
    }
}

