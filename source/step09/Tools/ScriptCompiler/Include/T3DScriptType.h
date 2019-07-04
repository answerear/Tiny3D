/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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

#ifndef __T3D_SCRIPT_TYPE_H__
#define __T3D_SCRIPT_TYPE_H__


#include "T3DScriptPrerequisites.h"


namespace Tiny3D
{
    #define LOG_TAG                 "Compiler"
    #define CURRENT_VERSION_STR     "0.1.0"

    /**
     * @brief 符号ID
     */
    enum
    {
        TID_LBRACE = 0,     /// {
        TID_RBRACE,         /// }
        TID_COLON,          /// :
        TID_VARIABLE,       /// $...
        TID_WORD,           /// *
        TID_QUOTE,          /// "*"
        TID_NEWLINE,        /// \n
        TID_UNKNOWN,        ///
        TID_END
    };

    /**
     * @brief 符号结构
     */
    struct Token
    {
        String      lexeme; /// 符号的词素
        String      file;   /// 当前文件名
        uint32_t    type;   /// 符号对应的ID
        uint32_t    line;   /// 当前行号
    };

    typedef std::shared_ptr<Token>      TokenPtr;
    typedef std::vector<TokenPtr>       TokenList;
    typedef std::shared_ptr<TokenList>  TokenListPtr;

    /**
     * @brief 结点类型
     */
    enum ConcreteNodeType
    {
        CNT_VARIABLE,           /// 变量 $xxx
        CNT_VARIABLE_ASSIGN,    /// 变量赋值，set $xxx
        CNT_WORD,               /// *
        CNT_IMPORT,             /// import * from *
        CNT_QUOTE,              /// "*"
        CNT_LBRACE,             /// {
        CNT_RBRACE,             /// }
        CNT_COLON               /// :
    };

    struct ConcreteNode;

    typedef std::shared_ptr<ConcreteNode>       ConcreteNodePtr;
    typedef std::list<ConcreteNodePtr>          ConcreteNodeList;
    typedef std::shared_ptr<ConcreteNodeList>   ConcreteNodeListPtr;

    /**
     * @brief 语法树结点
     */
    struct ConcreteNode
    {
        String              token;      /// 符号名称
        String              file;       /// 脚本文件名
        uint32_t            line;       /// 当前符号行号
        ConcreteNodeType    type;       /// 结点类型
        ConcreteNodeList    children;   /// 子结点
        ConcreteNode        *parent;    /// 父结点
    };

    /**
     * @brief 操作码
     */
    enum OpCode
    {
        ID_MATERIAL = 3,
        ID_VERTEX_PROGRAM,
        ID_GEOMETRY_PROGRAM,
        ID_FRAGMENT_PROGRAM,
        ID_TECHNIQUE,
        ID_PASS,
        ID_TEXTURE_UNIT,
        ID_VERTEX_PROGRAM_REF,
        ID_GEOMETRY_PROGRAM_REF,
        ID_FRAGMENT_PROGRAM_REF,
        ID_SHADOW_CASTER_VERTEX_PROGRAM_REF,
        ID_SHADOW_CASTER_FRAGMENT_PROGRAM_REF,
        ID_SHADOW_RECEIVER_VERTEX_PROGRAM_REF,
        ID_SHADOW_RECEIVER_FRAGMENT_PROGRAM_REF,
        ID_SHADOW_CASTER_MATERIAL,
        ID_SHADOW_RECEIVER_MATERIAL,

        ID_LOD_VALUES,
        ID_LOD_STRATEGY,
        ID_LOD_DISTANCES,
        ID_RECEIVE_SHADOWS,
        ID_TRANSPARENCY_CASTS_SHADOWS,
        ID_SET_TEXTURE_ALIAS,

        ID_SOURCE,
        ID_SYNTAX,
        ID_DEFAULT_PARAMS,
        ID_PARAM_INDEXED,
        ID_PARAM_NAMED,
        ID_PARAM_INDEXED_AUTO,
        ID_PARAM_NAMED_AUTO,

        ID_SCHEME,
        ID_LOD_INDEX,
        ID_GPU_VENDOR_RULE,
        ID_GPU_DEVICE_RULE,
        ID_INCLUDE,
        ID_EXCLUDE,

        ID_AMBIENT,
        ID_DIFFUSE,
        ID_SPECULAR,
        ID_EMISSIVE,
        ID_VERTEXCOLOUR,
        ID_SCENE_BLEND,
        ID_COLOUR_BLEND,
        ID_ONE,
        ID_ZERO,
        ID_DEST_COLOUR,
        ID_SRC_COLOUR,
        ID_ONE_MINUS_DEST_COLOUR,
        ID_ONE_MINUS_SRC_COLOUR,
        ID_DEST_ALPHA,
        ID_SRC_ALPHA,
        ID_ONE_MINUS_DEST_ALPHA,
        ID_ONE_MINUS_SRC_ALPHA,
        ID_SEPARATE_SCENE_BLEND,
        ID_SCENE_BLEND_OP,
        ID_REVERSE_SUBTRACT,
        ID_MIN,
        ID_MAX,
        ID_SEPARATE_SCENE_BLEND_OP,
        ID_DEPTH_CHECK,
        ID_DEPTH_WRITE,
        ID_DEPTH_FUNC,
        ID_DEPTH_BIAS,
        ID_ITERATION_DEPTH_BIAS,
        ID_ALWAYS_FAIL,
        ID_ALWAYS_PASS,
        ID_LESS_EQUAL,
        ID_LESS,
        ID_EQUAL,
        ID_NOT_EQUAL,
        ID_GREATER_EQUAL,
        ID_GREATER,
        ID_ALPHA_REJECTION,
        ID_ALPHA_TO_COVERAGE,
        ID_LIGHT_SCISSOR,
        ID_LIGHT_CLIP_PLANES,
        ID_TRANSPARENT_SORTING,
        ID_ILLUMINATION_STAGE,
        ID_DECAL,
        ID_CULL_HARDWARE,
        ID_CLOCKWISE,
        ID_ANTICLOCKWISE,
        ID_CULL_SOFTWARE,
        ID_BACK,
        ID_FRONT,
        ID_NORMALISE_NORMALS,
        ID_LIGHTING,
        ID_SHADING,
        ID_FLAT,
        ID_GOURAUD,
        ID_PHONG,
        ID_POLYGON_MODE,
        ID_SOLID,
        ID_WIREFRAME,
        ID_POINTS,
        ID_POLYGON_MODE_OVERRIDEABLE,
        ID_FOG_OVERRIDE,
        ID_NONE,
        ID_LINEAR,
        ID_EXP,
        ID_EXP2,
        ID_COLOUR_WRITE,
        ID_MAX_LIGHTS,
        ID_START_LIGHT,
        ID_ITERATION,
        ID_ONCE,
        ID_ONCE_PER_LIGHT,
        ID_PER_LIGHT,
        ID_PER_N_LIGHTS,
        ID_POINT,
        ID_SPOT,
        ID_DIRECTIONAL,
        ID_LIGHT_MASK,
        ID_POINT_SIZE,
        ID_POINT_SPRITES,
        ID_POINT_SIZE_ATTENUATION,
        ID_POINT_SIZE_MIN,
        ID_POINT_SIZE_MAX,

        ID_TEXTURE_ALIAS,
        ID_TEXTURE,
        ID_1D,
        ID_2D,
        ID_3D,
        ID_CUBIC,
        ID_2DARRAY,
        ID_UNLIMITED,
        ID_ALPHA,
        ID_GAMMA,
        ID_ANIM_TEXTURE,
        ID_CUBIC_TEXTURE,
        ID_SEPARATE_UV,
        ID_COMBINED_UVW,
        ID_TEX_COORD_SET,
        ID_TEX_ADDRESS_MODE,
        ID_WRAP,
        ID_CLAMP,
        ID_BORDER,
        ID_MIRROR,
        ID_TEX_BORDER_COLOUR,
        ID_FILTERING,
        ID_BILINEAR,
        ID_TRILINEAR,
        ID_ANISOTROPIC,
        ID_CMPTEST,
//         ID_ON,
//         ID_OFF,
        ID_CMPFUNC,
        ID_MAX_ANISOTROPY,
        ID_MIPMAP_BIAS,
        ID_COLOUR_OP,
        ID_REPLACE,
        ID_ADD,
        ID_MODULATE,
        ID_ALPHA_BLEND,
        ID_COLOUR_OP_EX,
        ID_SOURCE1,
        ID_SOURCE2,
        ID_MODULATE_X2,
        ID_MODULATE_X4,
        ID_ADD_SIGNED,
        ID_ADD_SMOOTH,
        ID_SUBTRACT,
        ID_BLEND_DIFFUSE_COLOUR,
        ID_BLEND_DIFFUSE_ALPHA,
        ID_BLEND_TEXTURE_ALPHA,
        ID_BLEND_CURRENT_ALPHA,
        ID_BLEND_MANUAL,
        ID_DOT_PRODUCT,
        ID_SRC_CURRENT,
        ID_SRC_TEXTURE,
        ID_SRC_DIFFUSE,
        ID_SRC_SPECULAR,
        ID_SRC_MANUAL,
        ID_COLOUR_OP_MULTIPASS_FALLBACK,
        ID_ALPHA_OP_EX,
        ID_ENV_MAP,
        ID_SPHERICAL,
        ID_PLANAR,
        ID_CUBIC_REFLECTION,
        ID_CUBIC_NORMAL,
        ID_SCROLL,
        ID_SCROLL_ANIM,
        ID_ROTATE,
        ID_ROTATE_ANIM,
        ID_SCALE,
        ID_WAVE_XFORM,
        ID_SCROLL_X,
        ID_SCROLL_Y,
        ID_SCALE_X,
        ID_SCALE_Y,
        ID_SINE,
        ID_TRIANGLE,
        ID_SQUARE,
        ID_SAWTOOTH,
        ID_INVERSE_SAWTOOTH,
        ID_TRANSFORM,
        ID_BINDING_TYPE,
        ID_VERTEX,
        ID_FRAGMENT,
        ID_CONTENT_TYPE,
        ID_NAMED,
        ID_SHADOW,
        ID_TEXTURE_SOURCE,
        ID_SHARED_PARAMS,
        ID_SHARED_PARAM_NAMED,
        ID_SHARED_PARAMS_REF,

        ID_PARTICLE_SYSTEM,
        ID_EMITTER,
        ID_AFFECTOR,

        ID_COMPOSITOR,
        ID_TARGET,
        ID_TARGET_OUTPUT,

        ID_INPUT,
        ID_PREVIOUS,
        ID_TARGET_WIDTH,
        ID_TARGET_HEIGHT,
        ID_TARGET_WIDTH_SCALED,
        ID_TARGET_HEIGHT_SCALED,
        ID_COMPOSITOR_LOGIC,
        ID_TEXTURE_REF,
        ID_SCOPE_LOCAL,
        ID_SCOPE_CHAIN,
        ID_SCOPE_GLOBAL,
        ID_POOLED,
        //ID_GAMMA, - already registered for material
        ID_NO_FSAA,
        ID_DEPTH_POOL,
        ID_ONLY_INITIAL,
        ID_VISIBILITY_MASK,
        ID_LOD_BIAS,
        ID_MATERIAL_SCHEME,
        ID_SHADOWS_ENABLED,

        ID_CLEAR,
        ID_STENCIL,
        ID_RENDER_SCENE,
        ID_RENDER_QUAD,
        ID_IDENTIFIER,
        ID_FIRST_RENDER_QUEUE,
        ID_LAST_RENDER_QUEUE,
        ID_QUAD_NORMALS,
        ID_CAMERA_FAR_CORNERS_VIEW_SPACE,
        ID_CAMERA_FAR_CORNERS_WORLD_SPACE,

        ID_BUFFERS,
        ID_COLOUR,
        ID_DEPTH,
        ID_COLOUR_VALUE,
        ID_DEPTH_VALUE,
        ID_STENCIL_VALUE,

        ID_CHECK,
        ID_COMP_FUNC,
        ID_REF_VALUE,
        ID_MASK,
        ID_FAIL_OP,
        ID_KEEP,
        ID_INCREMENT,
        ID_DECREMENT,
        ID_INCREMENT_WRAP,
        ID_DECREMENT_WRAP,
        ID_INVERT,
        ID_DEPTH_FAIL_OP,
        ID_PASS_OP,
        ID_TWO_SIDED,
        ID_READ_BACK_AS_TEXTURE,
        // Suport for shader model 5.0
        // More program IDs
        ID_TESSELLATION_HULL_PROGRAM,
        ID_TESSELLATION_DOMAIN_PROGRAM,
        ID_COMPUTE_PROGRAM,
        ID_TESSELLATION_HULL_PROGRAM_REF,
        ID_TESSELLATION_DOMAIN_PROGRAM_REF,
        ID_COMPUTE_PROGRAM_REF,
        // More binding IDs
        ID_GEOMETRY,
        ID_TESSELLATION_HULL,
        ID_TESSELLATION_DOMAIN,
        ID_COMPUTE,

        // Support for subroutine
        ID_SUBROUTINE,

        // added during 1.11. re-sort for 1.12
        ID_LINE_WIDTH,
        ID_SAMPLER,
        ID_SAMPLER_REF,
        ID_THREAD_GROUPS,
        ID_RENDER_CUSTOM,

        ID_END_BUILTIN_IDS
    };

    enum BuiltInValue
    {
        ID_ON = 1,
        ID_OFF = 2,
        ID_TRUE = 1,
        ID_FALSE = 2,
        ID_YES = 1,
        ID_NO = 2
    };
}


#endif  /*__T3D_SCRIPT_TYPE_H__*/
