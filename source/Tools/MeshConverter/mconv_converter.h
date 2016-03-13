/**
 * 转换后的*.t3db (xml) 格式如下：
 *      <?xml version="1.0" encoding="utf-8"?>
 *      <TINY3D xmlns="xxx" version="0.0.1">
 *      <scene>
 *          <model id="wolf">
 *              <mesh id="wolf_skin">
 *                  <attributes count="5">
 *                      <attirbute id="POSITION" size="3" type="float">
 *                      <attribute id="TEXCOORD" size="2" tpye="float">
 *                      <attribute id="NORMAL" size="3" type="float">
 *                      <attribute id="TANGENT" size="3" type="float">
 *                      <attribute id="BINORMAL" size="3" type="float">
 *                      <attribute id="COLOR" size="4" type="float">
 *                      <attribute id="BLEND_WEIGHT" size="4" type="float">
 *                      <attribute id="BLEND_INDEX" size="4" type="float">
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
 *                  <bone id="bone_head">
 *                      <transform>
 *                          1.0 0.0 0.0 0.0
 *                          0.0 1.0 0.0 0.0
 *                          0.0 0.0 1.0 0.0
 *                          0.0 0.0 0.0 1.0
 *                      </transform>
 *                      <children count="1">
 *                          <bone id="bone_body">
 *                              <transform>
 *                                  1.0 0.0 0.0 0.0
 *                                  0.0 1.0 0.0 0.0
 *                                  0.0 0.0 1.0 0.0
 *                                  0.0 0.0 0.0 1.0
 *                              </transform>
 *                          </bone>
 *                      </children>
 *                  </bone>
 *              </skeleton>

 *              <animations id="" count="3>
 *                  <animation id="idle">
 *                      <keyframe></keyframe>
 *                      <keyframe></keyframe>
 *                  </animation>
 *                  <animation id="walk">
 *                  </animation>
 *                  <animation id="run">
 *                  </animation>
 *              </animations>

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

        Settings    mSettings;

        Serializer  *mImporter;
        Serializer  *mExporter;

        void        *mSrcData;
        void        *mDstData;
    };
}


#endif  /*__MCONV_CONVERTER_H__*/
