

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
        bool processFbxNode(FbxNode *pFbxNode, Node *pParent);
        bool processFbxAnimation(FbxNode *pFbxNode, Model *pModel);

        bool processFbxMesh(FbxNode *pFbxNode, Node *pParent, Node *&pNode);
        bool processFbxSkin(FbxNode *pFbxNode, Node *pParent, Mesh *pMesh);
        bool processFbxSkeleton(FbxNode *pFbxNode, Node *pParent, Model *pModel);
        bool processFbxCamera(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode);
        bool processFbxLight(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode);
        bool processFbxMaterial(FbxNode *pFbxNode, Node *pParent);
        
        bool computeBoundingBox(Mesh *pMesh);
        bool computeBoundingSphere(Model *pModel, Mesh *pMesh);
        bool computeAlignAxisBoundingBox(Model *pModel, Mesh *pMesh);

        bool putVertexAttribute(Mesh *pMesh, bool bHasAttributes, const VertexAttribute &rkSource, const VertexAttribute &rkOther);
        bool processVertexAttribute(FbxMesh *pFbxMesh, Mesh *pMesh);
//         bool readPosition(FbxMesh *pFbxMesh, int nControlPointIdx, FbxVector3 &pos);
        bool readPosition(FbxMesh *pFbxMesh, int nControlPointIdx, Vector3 &pos);
//         bool readColor(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector4 &color);
        bool readColor(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector4 &color);
//         bool readUV(FbxMesh *pFbxMesh, int nControlPointIdx, int nUVIndex, int nLayer, FbxVector2 &uv);
        bool readUV(FbxMesh *pFbxMesh, int nControlPointIdx, int nUVIndex, int nLayer, Vector2 &uv);
//         bool readNormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &normal);
//         bool readTangent(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &tangent);
//         bool readBinormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &binormal);
        bool readNormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector3 &normal);
        bool readTangent(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector3 &tangent);
        bool readBinormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector3 &binormal);
        bool readMaterial(FbxMesh *pFbxMesh, int nTriangleIndex, int &nMaterialIndex);

        bool optimizeMesh(Node *pRoot);

        // 根据某个Cluster的link node回溯获取骨骼根节点
        bool searchSkeletonRoot(FbxNode *pFbxNode, FbxNode *&pFbxRootNode);
        // 根据骨骼根节点，搜索看是否已经生成过对应的骨骼数据
        bool searchSkeleton(FbxNode *pFbxNode);

        // 根据模型名称搜索是否曾经在本model下创建过动画节点
        bool searchAnimation(const String &name, Node *pNode, Animation *&pAnim);
        // 根据动作名称，搜索是否已经存在该动作
        bool searchAction(const String &name, Animation *pAnim, Action *&pAction);

        // 更新顶点中的骨骼索引和骨骼权重值
        bool updateVertexBlendIndexAndWeight(Mesh *pMesh, int nCtrlPointIdx, int nBlendIndex, double fBlendWeight);

        // 添加骨骼权重和骨骼索引到顶点属性里
        void updateVertexBlendAttributes(Mesh *pMesh);

        // 从FBX的纹理wrap模式转换成字符串
        String FbxWrapModeToString(FbxTexture::EWrapMode eWrapMode) const;

        bool searchMaterial(const String &name, Material *&pMaterial);

        struct SceneInfo
        {
            void    *mRoot;
            String  mName;
        };

        typedef std::map<FbxNode*, Skeleton*>       Skeletons;
        typedef Skeletons::iterator                 SkeletonsItr;
        typedef Skeletons::const_iterator           SkeletonsConstItr;
        typedef std::pair<FbxNode*, Skeleton*>      SkeletonsValue;

        typedef std::list<SceneInfo>                SceneList;
        typedef SceneList::iterator                 SceneListItr;
        typedef SceneList::const_iterator           SceneListConstItr;

        Serializer  *mImporter;
        Serializer  *mExporter;

        void        *mSrcData;
//         void        *mDstData;

        Node        *mCurScene;         // 当前场景节点，对于split mode是会变化的，对于merge和shared vertex永远只有一个不变的
        Node        *mCurModel;         // 当前模型节点
        Node        *mCurMesh;          // 用于共享顶点模式的当前网格节点
        Node        *mCurSkin;          // 当前蒙皮节点
        Node        *mCurSkeleton;      // 骨骼节点
        Node        *mCurAnimation;     // 动画节点
        Node        *mCurMaterials;     // 只用于merge和shared vertex文件格式下
        Node        *mCurBound;         // 当前的碰撞区，不管是splite模式、merge模式、还是shared vertex模式，整个model只有一个bound

        SceneList   mSceneList;

        Skeletons   mSkeletons;
        bool        mHasSkeleton;
        bool        mHasVertexBlending;
        bool        mHasAnimation;

#ifdef _DEBUG
        int         mTabCount;
#endif
    };
}


#endif  /*__MCONV_FBX_CONVERTER_H__*/
