

#ifndef __MCONV_FBX_CONVERTER_H__
#define __MCONV_FBX_CONVERTER_H__


#include "mconv_converter.h"


namespace mconv
{
    class Serializer;
    class Node;
    class Mesh;
    class Model;
    class Skeleton;
    class Bone;
    class Animation;
    class Action;
    class Material;
    class VertexAttribute;
    class VertexBuffer;
    class VertexBuffers;
    class Transform;
    class SubMesh;

    class FBXConverter : public ConverterImpl
    {
    public:
        FBXConverter(const Settings &settings);
        virtual ~FBXConverter();

    protected:
        virtual bool importScene() override;
        virtual bool exportScene() override;
        virtual bool convertToT3D() override;
        virtual void cleanup() override;

        void convertMatrix(const FbxAMatrix &fbxMat, Matrix4 &m);

        bool processFbxScene(FbxScene *pFbxScene, Node *pRoot);
        bool processFbxNode(FbxNode *pFbxNode, Node *pTransformNode, Node *pParent);
        bool processFbxAnimation(FbxNode *pFbxNode, Model *pModel);

        bool processFbxMesh(FbxNode *pFbxNode, Node *pParent, Transform *pTransform, Node *&pNode);
        bool processFbxSkin(FbxNode *pFbxNode, Node *pParent, Mesh *pMesh);
        bool processFbxSkeleton(FbxNode *pFbxNode, Node *pParent, Model *pModel, Node *&pNode);
        bool processFbxCamera(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode);
        bool processFbxLight(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode);
        bool processFbxMaterial(FbxNode *pFbxNode, Node *pParent);
        
        bool processBoundingBox(Node *pRoot);
        bool computeBoundingBox(SubMesh *pSubMesh);
        bool computeBoundingSphere(SubMesh *pSubMesh);
        bool computeAlignAxisBoundingBox(SubMesh *pSubMesh);
//         bool computeBoundingBox(Mesh *pMesh);
//         bool computeBoundingSphere(Model *pModel, Mesh *pMesh);
//         bool computeAlignAxisBoundingBox(Model *pModel, Mesh *pMesh);

        bool searchVertexBuffer(Mesh *pMesh, VertexBuffer *&pVertexBuffer);

        bool putVertexAttribute(VertexBuffer *pVertexBuffer, bool bHasAttributes, const VertexAttribute &rkSource, const VertexAttribute &rkOther);
        bool processVertexAttribute(FbxMesh *pFbxMesh, VertexBuffer *pVertexBuffer);
        bool readPosition(FbxMesh *pFbxMesh, int nControlPointIdx, Vector3 &pos);
        bool readColor(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector4 &color);
        bool readUV(FbxMesh *pFbxMesh, int nControlPointIdx, int nUVIndex, int nLayer, Vector2 &uv);
        bool readNormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector3 &normal);
        bool readTangent(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector3 &tangent);
        bool readBinormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector3 &binormal);
        bool readMaterial(FbxMesh *pFbxMesh, int nTriangleIndex, int &nMaterialIndex);

        bool optimizeMesh(Node *pNode);

        bool updateSkinInfo(FbxNode *pFbxNode, size_t boneIdx, const Matrix4 &m);
        bool updateBoneMatrix(FbxNode *pFbxNode, const Matrix4 &m, Node *pParent, Node *&pNode);
        bool fixBoneIndex(Bone *pBone);

//         // 根据某个Cluster的link node回溯获取骨骼根节点
//         bool searchSkeletonRoot(FbxNode *pFbxNode, FbxNode *&pFbxRootNode);
//         // 根据骨骼根节点，搜索看是否已经生成过对应的骨骼数据
//         bool searchSkeleton(FbxNode *pFbxNode);

        // 根据模型名称搜索是否曾经在本model下创建过动画节点
        bool searchAnimation(const String &name, Node *pNode, Animation *&pAnim);
        // 根据动作名称，搜索是否已经存在该动作
        bool searchAction(const String &name, Animation *pAnim, Action *&pAction);

        // 更新顶点中的骨骼索引和骨骼权重值
        bool updateVertexBlendIndexAndWeight(Mesh *pMesh, int nCtrlPointIdx, int nBlendIndex, double fBlendWeight);
        bool updateVertexBlendIndexAndWeight(VertexBuffer *pVB, int nCtrlPointIndex, int nBlendIndex, double fBlendWeight);
        void putVertexBlendAndWeightAttributes(VertexBuffer *pVB);

        // 添加骨骼权重和骨骼索引到顶点属性里
        void updateVertexBlendAttributes(Mesh *pMesh);

        // 从FBX的纹理wrap模式转换成字符串
        String FbxWrapModeToString(FbxTexture::EWrapMode eWrapMode) const;

        bool searchMaterial(const String &name, Material *&pMaterial);

        String getFileName(const String &strPath);
        
        typedef std::map<FbxNode*, Skeleton*>       Skeletons;
        typedef Skeletons::iterator                 SkeletonsItr;
        typedef Skeletons::const_iterator           SkeletonsConstItr;
        typedef std::pair<FbxNode*, Skeleton*>      SkeletonsValue;

        typedef std::map<FbxNode*, Bone*>           Bones;
        typedef Bones::iterator                     BonesItr;
        typedef Bones::const_iterator               BonesConstItr;
        typedef std::pair<FbxNode*, Bone*>          BonesValue;

        Serializer  *mImporter;
        Serializer  *mExporter;

        void        *mSrcData;
        void        *mDstData;

        Node        *mCurScene;         // 当前场景节点，对于split mode是会变化的，对于merge和shared vertex永远只有一个不变的
        Node        *mCurModel;         // 当前模型节点
        Node        *mCurMesh;          // 用于共享顶点模式的当前网格节点
        Node        *mCurSubMeshes;     // 用于共享顶点模式的当前子网格集合节点
        Node        *mCurSkeleton;      // 骨骼节点
        Node        *mCurAnimation;     // 动画节点
        Node        *mCurMaterials;     // 只用于merge和shared vertex文件格式下
//         Node        *mCurBound;         // 当前的碰撞区，不管是splite模式、merge模式、还是shared vertex模式，整个model只有一个bound
        Node        *mRootTransform;    // 场景变换根结点
        Node        *mCurSkin;          // 当前蒙皮信息结点

        Bones       mBones;

        uint16_t    mMaxBoneIdx;        // 骨骼最大索引值
        uint16_t    mBoneIdx;           // 蒙皮信息的骨骼索引

//         Skeletons   mSkeletons;
        bool        mHasSkeleton;
        bool        mHasVertexBlending;
        bool        mHasAnimation;

        int         mBoneCount;         // 用来记录骨骼数量，主要用于不知道为何offset matrix和skeleton hiarachy里面的骨骼数有差异
    };
}


#endif  /*__MCONV_FBX_CONVERTER_H__*/
