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
 *                          <indices count="90">
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

 *              <skeleton id="wolf_skeleton">
 *                  <bone id="bone_head" count="1">
 *                      <translate>1.0 1.0 1.0</translate>
 *                      <rotate>1.0 1.0 1.0 1.0</rotate>
 <                      <scale>1.2 1.2 1.2</scale>
 *                      <bone id="bone_body" count="1">
 *                          <translate></translate>
 *                          <rotate></rotate>
 *                          <scale></scale>
 *                      </bone>
 *                  </bone>
 *              </skeleton>

 *              <animation id="" count="3>
 *                  <action id="idle" spane="4.0">
 *                      <keyframe type="translate" count="2">
 *                          <frame id="1" time="0.1">1.0 1.0 1.0</frame>
 *                          <frame id="2" time="0.5">0.0 1.0 0.0</frame>
 *                      </keyframe>
 *                      <keyframe type="rotate" count="2">
 *                          <frame id="2" time="1.0">1.0 1.0 1.0 1.0</frame>
 *                          <frame id="3" time="1.2">1.0 1.0 1.0 1.0</frame>
 *                      </keyframe>
 *                      <keyframe type="scale" count="2">
 *                          <frame id="4" time="1.5">1.2 1.2 1.2</frame>
 *                          <frame id="5" time="2.0">1.2 1.2 1.2</frame>
 *                      </keyframe>
 *                  </action>
 *                  <action id="walk">
 *                  </action>
 *                  <action id="run">
 *                  </action>
 *              </animation>
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

    class Converter
    {
    public:
        Converter();
        virtual ~Converter();

        bool execute(int argc, char *argv[]);

    protected:
        struct BindingNode
        {
            BindingNode()
                : pSkel(nullptr)
                , pMesh(nullptr)
            {

            }

            Skeleton    *pSkel;
            Mesh        *pMesh;
        };

        bool importScene();
        bool convert();
        bool exportScene();

        bool convertToT3D();
        bool convertToFBX();

        bool processFbxScene(FbxScene *pFbxScene, Node *pRoot);
        bool processFbxNode(FbxNode *pFbxNode, Node *pParent);
        bool processFbxAnimation(FbxNode *pFbxNode, Node *pParent);

        bool processFbxMesh(FbxNode *pFbxNode, Node *pParent, Node *&pNode);
        bool processFbxSkin(FbxNode *pFbxNode, Node *pParent, Mesh *pMesh);
        bool processFbxSkeleton(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode);
        bool processFbxCamera(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode);
        bool processFbxLight(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode);
        bool processFbxMaterial(FbxNode *pFbxNode, Node *pParent);

        bool processVertexAttribute(FbxMesh *pFbxMesh, Mesh *pMesh);
        bool readPosition(FbxMesh *pFbxMesh, int nControlPointIdx, FbxVector3 &pos);
        bool readColor(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector4 &color);
        bool readUV(FbxMesh *pFbxMesh, int nControlPointIdx, int nUVIndex, int nLayer, FbxVector2 &uv);
        bool readNormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &normal);
        bool readTangent(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &tangent);
        bool readBinormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &binormal);
        bool readMaterial(FbxMesh *pFbxMesh, int nTriangleIndex, int &nMaterialIndex);

        bool optimizeMesh(Node *pRoot);
        bool optimizeSkeleton();

        void cleanup();

        Settings    mSettings;

        Serializer  *mImporter;
        Serializer  *mExporter;

        void        *mSrcData;
        void        *mDstData;

        Model       *mModel;
        Skeleton    *mSkeleton;

        bool        mHasSkeleton;

#ifdef _DEBUG
        int         mTabCount;
#endif
    };
}


#endif  /*__MCONV_CONVERTER_H__*/
