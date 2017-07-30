/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
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

#ifndef __MCONV_OGRE_DATA_H__
#define __MCONV_OGRE_DATA_H__


#include "mconv_prerequisites.h"
#include "mconv_string.h"


namespace mconv
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Ogre Mesh, Skeleton and Animation Data
    ////////////////////////////////////////////////////////////////////////////////////////////////

    enum OgreVertexElementSemantic
    {
        VES_POSITION = 1,
        VES_BLEND_WEIGHTS = 2,
        VES_BLEND_INDICES = 3,
        VES_NORMAL = 4,
        VES_DIFFUSE = 5,
        VES_SPECULAR = 6,
        VES_TEXTURE_COORDINATES = 7,
        VES_BINORMAL = 8,
        VES_TANGENT = 9,
    };

    enum OgreVertexElementType
    {
        VET_FLOAT1 = 0,
        VET_FLOAT2 = 1,
        VET_FLOAT3 = 2,
        VET_FLOAT4 = 3,
        VET_COLOR = 4,
        VET_SHORT1 = 5,
        VET_SHORT2 = 6,
        VET_SHORT3 = 7,
        VET_SHORT4 = 8,
        VET_UBYTE4 = 9,
        VET_COLOR_ARGB = 10,
        VET_COLOR_ABGR = 11,
    };

#pragma pack(push, 1)
    struct OgreChunkHeader
    {
        uint16_t    id;
        uint32_t    length;
    };
#pragma pack(pop)

    struct OgreChunkData
    {
        OgreChunkHeader     header;
        uint32_t            read;

        OgreChunkData()
        {
            header.id = 0;
            header.length = 0;
            read = 0;
        }
    };

    struct OgreTextureAlias
    {
        OgreTextureAlias()  {}
        OgreTextureAlias(const String &texture, const String &alias) : textureName(texture), aliasName(alias)   {}

        String  textureName;
        String  aliasName;
    };

    struct OgreBoneAssignment
    {
        OgreBoneAssignment()
            : vertexID(0)
            , boneID(0)
            , weight(0.0f)
        {}

        uint32_t    vertexID;
        uint16_t    boneID;
        float       weight;
    };

    struct OgreVertexElement
    {
        OgreVertexElement()
            : source(0)
            , type(0)
            , semantic(0)
            , offset(0)
            , index(0)
        {}

        uint16_t    source;
        uint16_t    type;
        uint16_t    semantic;
        uint16_t    offset;
        uint16_t    index;
    };

    struct OgreVertexBuffer
    {
        OgreVertexBuffer()
            : bindIndex(0)
            , vertexSize(0)
        {}

        uint16_t            bindIndex;
        uint16_t            vertexSize;
        std::vector<float>  vertices;
    };

    struct OgreGeometry
    {
        OgreGeometry()
            : vertexCount(0)
        {}

        uint32_t                        vertexCount;
        std::vector<OgreVertexElement>  elements;
        std::vector<OgreVertexBuffer>   buffers;
    };

    struct OgreSubMesh
    {
        OgreSubMesh()
            : materialName("")
            , hasSharedVertices(false)
            , indices32Bit(false)
            , operation(0)
        {}

        String                          materialName;
        bool                            hasSharedVertices;
        std::vector<uint32_t>           indices;
        bool                            indices32Bit;
        OgreGeometry                    geometry;
        uint16_t                        operation;
        std::vector<OgreTextureAlias>   textureAliases;
        std::vector<OgreBoneAssignment> boneAssignments;
    };

    struct OgreBone
    {
        OgreBone()
            : name("")
            , position(Vector3::ZERO)
            , orientation(Quaternion::IDENTITY)
            , scale(Vector3::UNIT_SCALE)
            , handle(0xFFFF)
            , parent(0xFFFF)
        {}

        String                          name;
        Vector3                         position;
        Quaternion                      orientation;
        Vector3                         scale;
        uint16_t                        handle;
        uint16_t                        parent;
    };

    struct OgreKeyframe
    {
        OgreKeyframe()
            : boneID(0)
            , time(0.0f)
            , position(Vector3::ZERO)
            , orientation(Quaternion::IDENTITY)
            , scale(Vector3::UNIT_SCALE)
        {}

        uint16_t                        boneID;
        float                           time;
        Vector3                         position;
        Quaternion                      orientation;
        Vector3                         scale;
    };

    struct OgreAnimation
    {
        OgreAnimation()
            : name("")
            , length(0.0f)
        {}

        String                          name;
        float                           length;
        std::vector<OgreKeyframe>       keyframes;
    };

    struct OgreSkeleton
    {
        std::vector<OgreBone>           bones;
        std::vector<OgreAnimation>      animations;
    };

    struct OgreMesh
    {
        OgreMesh()
            : hasSkeleton(false)
            , minEdge(Vector3::ZERO)
            , maxEdge(Vector3::ZERO)
            , radius(1.0f)
        {}

        bool                            hasSkeleton;
        OgreGeometry                    geometry;
        std::vector<OgreSubMesh>        submeshes;
		std::vector<OgreBoneAssignment> boneAssignments;
        Vector3                         minEdge;
        Vector3                         maxEdge;
        float                           radius;

        OgreSkeleton                    skeleton;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Ogre Material Data
    ////////////////////////////////////////////////////////////////////////////////////////////////

    enum OgreTextureType
    {
        TEX_TYPE_1D = 1,
        TEX_TYPE_2D,
        TEX_TYPE_3D,
        TEX_TYPE_CUBE
    };

    enum OgreBindingType
    {
        BT_FRAGMENT = 0,
        BT_VERTEX
    };

    enum OgreTextureAddressMode
    {
        TAM_WRAP = 0,
        TAM_MIRROR,
        TAM_CLAMP,
        TAM_BORDER
    };

    struct OgreUVWAddressMode
    {
        OgreUVWAddressMode()
            : u("wrap")
            , v("wrap")
            , w("wrap")
        {}

        String u, v, w;
    };

    struct OgreTexture
    {
        OgreTexture()
            : name("")
            , type(TEX_TYPE_2D)
            , numMipMaps(1)
            , alpha(false)
            , gamma(false)
            , pixelFormat("PF_UNKNOWN")
        {}

        String                          name;
        OgreTextureType                 type;
        int32_t                         numMipMaps;
        bool                            alpha;
        bool                            gamma;
        String                          pixelFormat;
    };

    enum OgreFilterOptions
    {
        FO_NONE = 0,
        FO_POINT,
        FO_LINEAR,
        FO_ANISOTROPIC
    };

    struct OgreTextureFiltering
    {
        OgreTextureFiltering()
            : minFilter(FO_NONE)
            , magFilter(FO_NONE)
            , mipFilter(FO_NONE)
        {}

        OgreFilterOptions   minFilter;
        OgreFilterOptions   magFilter;
        OgreFilterOptions   mipFilter;
    };
    
    struct OgreTextureUnit
    {
        OgreTextureUnit()
            : textureSource("")
            , bindingType(BT_VERTEX)
            , texCoordSet(0)
            , colorOp("")
            , envMap("")
            , scrollUV(Vector2::ZERO)
            , scrollAnimUV(Vector2::ZERO)
            , rotateAngle(0.0f)
            , rotateSpeed(0.0f)
            , maxAnisotropy(0)
            , mipmapBias(0.0f)
        {
            texBorderColor[0] = texBorderColor[1] = texBorderColor[2] = texBorderColor[3] = 1.0f;
        }

        String                          name;                   /// the name of texture unit
        String                          textureSource;          /// texture_source
        OgreTexture                     texture;                /// texture
        StringVector                    animTexture;            /// anim_texture
        StringVector                    cubicTexture;           /// cubic_texture
        OgreBindingType                 bindingType;            /// binding_type
        uint32_t                        texCoordSet;            /// tex_coord_set
        OgreUVWAddressMode              texAddressMode;         /// tex_address_mode
        float                           texBorderColor[4];      /// tex_border_colour
        String                          colorOp;                /// colour_op
        StringVector                    colorOpEx;              /// colour_op_ex
        StringVector                    colorOpMultiFallback;   /// colour_op_multipass_fallback
        StringVector                    alphaOpEx;              /// alpha_op_ex
        String                          envMap;                 /// env_map
        Vector2                         scrollUV;               /// scroll
        Vector2                         scrollAnimUV;           /// scroll_anim
        float                           rotateAngle;            /// rotate
        float                           rotateSpeed;            /// rotate_anim
        Vector2                         scaleUV;                /// scale
        StringVector                    waveXForm;              /// wave_xform
        Matrix4                         transform;              /// transform
        OgreTextureFiltering            filtering;              /// filtering
        uint32_t                        maxAnisotropy;          /// max_anisotropy
        String                          textureAlias;           /// texture_alias
        float                           mipmapBias;             /// mipmap_bias
        String                          contentType;            /// content_type
    };

    enum OgreBlendMode
    {
        BM_NONE = 0,
        BM_ADD,
        BM_MODULATE,
        BM_COLOR_BLEND,
        BM_ALPHA_BLEND,
        BM_ONE,
        BM_ZERO,
        BM_DEST_COLOR,
        BM_SRC_COLOR,
        BM_ONE_MINUS_DEST_COLOR,
        BM_ONE_MINUS_SRC_COLOR,
        BM_DEST_ALPHA,
        BM_SRC_ALPHA,
        BM_ONE_MINUS_DEST_ALPHA,
        BM_ONE_MINUS_SRC_ALPHA
    };

    enum OgreCullingHardware
    {
        CULL_HW_NONE = 0,
        CULL_HW_ANTICLOCKWISE,
        CULL_HW_CLOCKWISE
    };

    enum OgreCullingSoftware
    {
        CULL_SW_NONE = 0,
        CULL_SW_BACK,
        CULL_SW_FRONT
    };

    enum OgreIlluminationStage
    {
        IS_NONE = 0,
        IS_AMBIENT,
        IS_PER_LIGHT,
        IS_DECAL
    };

    struct OgreAlphaRejection
    {
        OgreAlphaRejection()
            : func("")
            , value(0)
        {}

        String                          func;
        uint8_t                         value;
    };

    struct OgreFog
    {
        OgreFog()
            : enabled(false)
            , type("")
            , red(1.0f)
            , green(1.0f)
            , blue(1.0f)
            , alpha(1.0f)
            , density(1.0f)
            , linearStart(1.0f)
            , linearEnd(1.0f)
        {}

        bool                            enabled;
        String                          type;
        float                           red;
        float                           green;
        float                           blue;
        float                           alpha;
        float                           density;
        float                           linearStart;
        float                           linearEnd;
    };

    struct OgreDepthBias
    {
        OgreDepthBias()
            : constantBias(0.0f)
            , slopeScaleBias(0.0f)
        {}

        float                           constantBias;
        float                           slopeScaleBias;
    };

    struct OgrePointAttenuation
    {
        OgrePointAttenuation()
            : enabled(false)
            , constant(0.0f)
            , linear(0.0f)
            , quadratic(0.0f)
        {}

        bool                            enabled;
        float                           constant;
        float                           linear;
        float                           quadratic;
    };

    struct OgrePass
    {
        OgrePass()
            : shininess(1.0f)
            , sceneBlend(BM_NONE)
            , separateSceneBlend(BM_NONE)
            , depthCheckEnabled(false)
            , depthWriteEnabled(false)
            , normalizeNormals(false)
            , alphaCoverageEnabled(false)
            , transparentSortingEnabled(false)
            , colorWriteEnabled(false)
            , lightScissorEnabled(false)
            , lightClipPlanesEnabled(false)
            , cullHardware(CULL_HW_NONE)
            , cullSoftware(CULL_SW_NONE)
            , lightingEnabled(false)
            , shading("Phong")
            , polygonModeOverridable(false)
            , iterationDepthBias(0.0f)
            , maxLights(0)
            , startLight(0)
            , pointSize(1.0f)
            , pointSpritesEnabled(false)
            , pointSizeMin(0.0f)
            , pointSizeMax(0.0f)
            , illuminationStage(IS_NONE)
        {
            ambient[0] = ambient[1] = ambient[2] = ambient[3] = 1.0f;
            diffuse[0] = diffuse[1] = diffuse[2] = diffuse[3] = 1.0f;
            specular[0] = specular[1] = specular[2] = specular[3] = 1.0f;
            emissive[0] = emissive[1] = emissive[2] = emissive[3] = 1.0f;
        }

        String                          name;                       /// name of pass
        float                           ambient[4];                 /// ambient
        float                           diffuse[4];                 /// diffuse
        float                           specular[4];                /// specular
        float                           emissive[4];                /// emissive
        float                           shininess;                  /// shininess
        StringVector                    sceneBlend;                 /// scene_blend
        OgreBlendMode                   separateSceneBlend;         /// seperate_scene_blend
        bool                            depthCheckEnabled;          /// depth_check
        bool                            depthWriteEnabled;          /// depth_write
        String                          depthFunc;                  /// depth_func
        bool                            normalizeNormals;           /// normalise_normals
        OgreAlphaRejection              alphaRejection;             /// alpha_rejection
        bool                            alphaCoverageEnabled;       /// alpha_to_coverage
        bool                            transparentSortingEnabled;  /// transparent_sorting
        bool                            colorWriteEnabled;          /// colour_write
        bool                            lightScissorEnabled;        /// light_scissor
        bool                            lightClipPlanesEnabled;     /// light_clip_planes
        OgreCullingHardware             cullHardware;               /// cull_hardware
        OgreCullingSoftware             cullSoftware;               /// cull_software
        bool                            lightingEnabled;            /// lighting
        OgreFog                         fog;                        /// fog_override
        String                          shading;                    /// shading
        String                          polygonMode;                /// polygon_mode
        bool                            polygonModeOverridable;     /// polygon_mode_overrideable
        OgreDepthBias                   depthBias;                  /// depth_bias
        float                           iterationDepthBias;         /// iteration_depth_bias
        std::vector<OgreTextureUnit>    textures;                   /// texture_unit
        String                          vertexProgramRef;           /// vertex_program_ref
        String                          geometryProgramRef;         /// geometry_program_ref
        String                          shadowCasterVertexRef;      /// shadow_caster_vertex_program_ref
        String                          shadowReceiverVertexRef;    /// shadow_receiver_vertex_program_ref
        String                          shadowReceiverFragmentRef;  /// shadow_receiver_fragment_program_ref
        String                          fragmentProgramRef;         /// fragment_program_ref
        uint16_t                        maxLights;                  /// max_lights
        uint16_t                        startLight;                 /// start_light
        String                          iteration[4];               /// iteration
        float                           pointSize;                  /// point_size
        bool                            pointSpritesEnabled;        /// point_sprites
        OgrePointAttenuation            attenuation;                /// point_size_attenuation
        float                           pointSizeMin;               /// point_size_min
        float                           pointSizeMax;               /// point_size_max
        OgreIlluminationStage           illuminationStage;          /// illumination_stage
    };

    enum OgreTechniqueRule
    {
        TR_INCLUDE = 0,
        TR_EXCLUDE
    };

    struct OgreGPUVendorRule
    {
        OgreGPUVendorRule()
            : rule(TR_INCLUDE)
        {}

        OgreTechniqueRule               rule;
        String                          name;
    };

    struct OgreGPUDeviceRule
    {
        OgreGPUDeviceRule()
            : rule(TR_INCLUDE)
            , caseSensitive(false)
        {}

        OgreTechniqueRule               rule;
        String                          devicePattern;
        bool                            caseSensitive;
    };

    struct OgreTechnique
    {
        OgreTechnique()
            : LODIndex(0)
        {}

        String                          name;
        String                          scheme;                     /// scheme
        uint16_t                        LODIndex;                   /// lod_index
        String                          shadowCasterMaterial;       /// shadow_caster_material
        String                          shadowReceiverMaterial;     /// shadow_receiver_material
        OgreGPUVendorRule               gpuVendorRule;              /// gpu_vendor_rule
        OgreGPUDeviceRule               gpuDeviceRule;              /// gpu_device_rule
        std::vector<OgrePass>           passes;                     /// pass
    };

    struct OgreMaterial
    {
        OgreMaterial()
            : receiveShadows(false)
            , transparencyCastsShadows(false)
        {}

        String                          baseName;                   /// name of parent material
        String                          name;                       /// the name of material
        bool                            receiveShadows;             /// receive_shadows
        bool                            transparencyCastsShadows;   /// transparency_casts_shadows
        std::vector<float>              LODDistances;               /// lod_distances
        std::vector<OgreTechnique>      techniques;                 /// technique
    };
}


#endif  /*__MCONV_OGRE_DATA_H__*/
