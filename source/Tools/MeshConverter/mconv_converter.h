/**
 * 转换后的*.t3db (xml) 格式如下：
 *      <?xml version="1.0" encoding="utf-8"?>
 *      <TINY3D xmlns="xxx" version="0.0.1">
 *      <scene>
 *          <model id="wolf">
 *              <mesh id="wolf_skin">
 *                  <attributes count="5">
 *                      <attirbute id="POSITION" size="3" type="float" />
 *                      <attribute id="TEXCOORD" size="2" tpye="float" />
 *                      <attribute id="NORMAL" size="3" type="float" />
 *                      <attribute id="TANGENT" size="3" type="float" />
 *                      <attribute id="BINORMAL" size="3" type="float" />
 *                      <attribute id="COLOR" size="4" type="float" />
 *                      <attribute id="BLEND_WEIGHT" size="4" type="float" />
 *                      <attribute id="BLEND_INDEX" size="4" type="ushort" />
 *                  </attributes>

 *                  <vertices count="12">
 *                      -1.0 1.0 1.0 
 *                  </vertices>

 *                  <parts count="1">
 *                      <part id="wolf" primitive="triangles" count="30" material="wolf">
 *                          <indices count="90" 16bits="true">
 *                              1 2 3 1 2 3 1 2 3
 *                          </indices>
 *                      </part>
 *                  </parts>
 *              </mesh>

 *              <materials id="" count="1">
 *                  <material id="wolf">
 *                      <mode>Lambert</mode>
 *                      <ambient>1.0 1.0 1.0 1.0</ambient>
 *                      <diffuse>1.0 1.0 1.0 1.0</diffuse>
 *                      <specular>1.0 1.0 1.0 1.0</specular>
 *                      <emissive>1.0 1.0 1.0 1.0</emissive>
 *                      <shininess>20.0</shininess>
 *                      <transparency>0.0</transparency>
 *                      <reflection>1.0</reflection>
 *                      <textures count="2">
 *                          <texture id="texture">wolf.png</texture>
 *                          <texture id="lightmap">lightmap.png</texture>
 *                      </textures>
 *                      <effects count="3">
 *                          <effect id="shark" type="HLSL">ss.hlsl</effect>
 *                          <effect id="flash" type="GLSL">ss.glsl</effect>
 *                          <effect id="flash" type="CG">ss.cg</effect>
 *                      </effects>
 *                  </material>
 *              </materials>

 *              <skin id="wolf_skin" count="10">
 *                  <bone id="bone_head">
 *                      <transform>
 *                          1.0 0.0 0.0 0.0
 *                          0.0 1.0 0.0 0.0
 *                          0.0 0.0 1.0 0.0
 *                          0.0 0.0 0.0 1.0
 *                      </transform>
 *                  </bone>
 *              </skin>

 *              <skeleton id="wolf_skeleton">
 *                  <bone id="bone_head">
 *                      <transform>
 *                          1.0 0.0 0.0 0.0
 *                          0.0 1.0 0.0 0.0
 *                          0.0 0.0 1.0 0.0
 *                          0.0 0.0 0.0 1.0
 *                      </transform>
 *                      <bone id="bone_body">
 *                          <transform>
 *                          </transform>
 *                      </bone>
 *                  </bone>
 *              </skeleton>

 *              <animation id="" count="3>
 *                  <action id="idle">
 *                      <keyframe type="translation" bone="head" count="2">
 *                          <frame id="1" time="0.1">1.0 1.0 1.0</frame>
 *                          <frame id="2" time="0.5">0.0 1.0 0.0</frame>
 *                      </keyframe>
 *                      <keyframe type="rotation" bone="head" count="2">
 *                          <frame id="2" time="1.0">1.0 1.0 1.0 1.0</frame>
 *                          <frame id="3" time="1.2">1.0 1.0 1.0 1.0</frame>
 *                      </keyframe>
 *                      <keyframe type="scaling" bone="head" count="2">
 *                          <frame id="4" time="1.5">1.2 1.2 1.2</frame>
 *                          <frame id="5" time="2.0">1.2 1.2 1.2</frame>
 *                      </keyframe>
 *                  </action>
 *                  <action id="walk">
 *                  </action>
 *                  <action id="run">
 *                  </action>
 *              </animation>

 *              <bound type="box">-1.0 -1.0 -1.0 1.0 1.0 1.0</bound>
 *          </model>

 *          <light>
 *          </light>

 *          <camera>
 *          </camera>
 *      </scene>
 */

#ifndef __MCONV_CONVERTER_H__
#define __MCONV_CONVERTER_H__


#include "mconv_prerequisites.h"
#include "mconv_settings.h"


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

    class Converter
    {
    public:
        Converter();
        virtual ~Converter();

        bool execute(int argc, char *argv[]);

    protected:
        bool importScene();
        bool convert();
        bool exportScene();

        bool convertToT3D();
        bool convertToFBX();

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

        bool processVertexAttribute(FbxMesh *pFbxMesh, Mesh *pMesh);
        bool readPosition(FbxMesh *pFbxMesh, int nControlPointIdx, FbxVector3 &pos);
        bool readColor(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector4 &color);
        bool readUV(FbxMesh *pFbxMesh, int nControlPointIdx, int nUVIndex, int nLayer, FbxVector2 &uv);
        bool readNormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &normal);
        bool readTangent(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &tangent);
        bool readBinormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &binormal);
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

        void cleanup();

        typedef std::map<FbxNode*, Skeleton*>       Skeletons;
        typedef Skeletons::iterator                 SkeletonsItr;
        typedef Skeletons::const_iterator           SkeletonsConstItr;
        typedef std::pair<FbxNode*, Skeleton*>      SkeletonsValue;

        Settings    mSettings;

        Serializer  *mImporter;
        Serializer  *mExporter;

        void        *mSrcData;
        void        *mDstData;

        Skeletons   mSkeletons;
        bool        mHasSkeleton;
        bool        mHasVertexBlending;
        bool        mHasAnimation;

#ifdef _DEBUG
        int         mTabCount;
#endif
    };
}


#endif  /*__MCONV_CONVERTER_H__*/
