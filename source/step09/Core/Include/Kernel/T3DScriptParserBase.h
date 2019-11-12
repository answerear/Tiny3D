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


#ifndef __T3D_SCRIPT_PARSER_BASE_H__
#define __T3D_SCRIPT_PARSER_BASE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DObject.h"


namespace Tiny3D
{
    #define T3D_CHECK_READ_CONTENT(rd, sz, msg)    \
        if (rd != sz)  \
        {   \
            ret = T3D_ERR_RES_INVALID_CONTENT;  \
            T3D_LOG_ERROR(LOG_TAG_RESOURCE, msg);   \
            break;  \
        }

    class T3D_ENGINE_API ScriptParserBase : public Object
    {
    protected:
        /**
         * @brief 操作码
         */
        enum OpCode
        {
            E_OP_MATERIAL = 3,
            E_OP_VERTEX_PROGRAM,
            E_OP_GEOMETRY_PROGRAM,
            E_OP_FRAGMENT_PROGRAM,
            E_OP_TECHNIQUE,
            E_OP_PASS,
            E_OP_TEXTURE_UNIT,
            E_OP_VERTEX_PROGRAM_REF,
            E_OP_GEOMETRY_PROGRAM_REF,
            E_OP_FRAGMENT_PROGRAM_REF,
            E_OP_SHADOW_CASTER_VERTEX_PROGRAM_REF,
            E_OP_SHADOW_CASTER_FRAGMENT_PROGRAM_REF,
            E_OP_SHADOW_RECEIVER_VERTEX_PROGRAM_REF,
            E_OP_SHADOW_RECEIVER_FRAGMENT_PROGRAM_REF,
            E_OP_SHADOW_CASTER_MATERIAL,
            E_OP_SHADOW_RECEIVER_MATERIAL,

            E_OP_LOD_VALUES,
            E_OP_LOD_STRATEGY,
            E_OP_LOD_DISTANCES,
            E_OP_RECEIVE_SHADOWS,
            E_OP_TRANSPARENCY_CASTS_SHADOWS,
            E_OP_SET_TEXTURE_ALIAS,

            E_OP_SOURCE,
            E_OP_ENTRY_POINT,
            E_OP_STAGE,
            E_OP_SYNTAX,
            E_OP_DEFAULT_PARAMS,
            E_OP_PARAM_INDEXED,
            E_OP_PARAM_NAMED,
            E_OP_PARAM_INDEXED_AUTO,
            E_OP_PARAM_NAMED_AUTO,

            E_OP_SCHEME,
            E_OP_LOD_INDEX,
            E_OP_GPU_VENDOR_RULE,
            E_OP_GPU_DEVICE_RULE,
            E_OP_INCLUDE,
            E_OP_EXCLUDE,

            E_OP_AMBIENT,
            E_OP_DIFFUSE,
            E_OP_SPECULAR,
            E_OP_EMISSIVE,
            E_OP_VERTEXCOLOUR,
            E_OP_SCENE_BLEND,
            E_OP_COLOUR_BLEND,
            E_OP_ONE,
            E_OP_ZERO,
            E_OP_DEST_COLOUR,
            E_OP_SRC_COLOUR,
            E_OP_ONE_MINUS_DEST_COLOUR,
            E_OP_ONE_MINUS_SRC_COLOUR,
            E_OP_DEST_ALPHA,
            E_OP_SRC_ALPHA,
            E_OP_ONE_MINUS_DEST_ALPHA,
            E_OP_ONE_MINUS_SRC_ALPHA,
            E_OP_SEPARATE_SCENE_BLEND,
            E_OP_SCENE_BLEND_OP,
            E_OP_REVERSE_SUBTRACT,
            E_OP_MIN,
            E_OP_MAX,
            E_OP_SEPARATE_SCENE_BLEND_OP,
            E_OP_DEPTH_CHECK,
            E_OP_DEPTH_WRITE,
            E_OP_DEPTH_FUNC,
            E_OP_DEPTH_BIAS,
            E_OP_ITERATION_DEPTH_BIAS,
            E_OP_ALWAYS_FAIL,
            E_OP_ALWAYS_PASS,
            E_OP_LESS_EQUAL,
            E_OP_LESS,
            E_OP_EQUAL,
            E_OP_NOT_EQUAL,
            E_OP_GREATER_EQUAL,
            E_OP_GREATER,
            E_OP_ALPHA_REJECTION,
            E_OP_ALPHA_TO_COVERAGE,
            E_OP_LIGHT_SCISSOR,
            E_OP_LIGHT_CLIP_PLANES,
            E_OP_TRANSPARENT_SORTING,
            E_OP_ILLUMINATION_STAGE,
            E_OP_DECAL,
            E_OP_CULL_HARDWARE,
            E_OP_CLOCKWISE,
            E_OP_ANTICLOCKWISE,
            E_OP_CULL_SOFTWARE,
            E_OP_BACK,
            E_OP_FRONT,
            E_OP_NORMALISE_NORMALS,
            E_OP_LIGHTING,
            E_OP_SHADING,
            E_OP_FLAT,
            E_OP_GOURAUD,
            E_OP_PHONG,
            E_OP_POLYGON_MODE,
            E_OP_SOLID,
            E_OP_WIREFRAME,
            E_OP_POINTS,
            E_OP_POLYGON_MODE_OVERRIDEABLE,
            E_OP_FOG_OVERRIDE,
            E_OP_NONE,
            E_OP_LINEAR,
            E_OP_EXP,
            E_OP_EXP2,
            E_OP_COLOUR_WRITE,
            E_OP_MAX_LIGHTS,
            E_OP_START_LIGHT,
            E_OP_ITERATION,
            E_OP_ONCE,
            E_OP_ONCE_PER_LIGHT,
            E_OP_PER_LIGHT,
            E_OP_PER_N_LIGHTS,
            E_OP_POINT,
            E_OP_SPOT,
            E_OP_DIRECTIONAL,
            E_OP_LIGHT_MASK,
            E_OP_POINT_SIZE,
            E_OP_POINT_SPRITES,
            E_OP_POINT_SIZE_ATTENUATION,
            E_OP_POINT_SIZE_MIN,
            E_OP_POINT_SIZE_MAX,

            E_OP_TEXTURE_ALIAS,
            E_OP_TEXTURE,
            E_OP_1D,
            E_OP_2D,
            E_OP_3D,
            E_OP_CUBIC,
            E_OP_2DARRAY,
            E_OP_UNLIMITED,
            E_OP_ALPHA,
            E_OP_GAMMA,
            E_OP_PIXELFORMAT,
            E_OP_ANIM_TEXTURE,
            E_OP_CUBIC_TEXTURE,
            E_OP_SEPARATE_UV,
            E_OP_COMBINED_UVW,
            E_OP_TEX_COORD_SET,
            E_OP_TEX_ADDRESS_MODE,
            E_OP_WRAP,
            E_OP_CLAMP,
            E_OP_BORDER,
            E_OP_MIRROR,
            E_OP_TEX_BORDER_COLOUR,
            E_OP_FILTERING,
            E_OP_BILINEAR,
            E_OP_TRILINEAR,
            E_OP_ANISOTROPIC,
            E_OP_CMPTEST,
            E_OP_ON,
            E_OP_OFF,
            E_OP_CMPFUNC,
            E_OP_MAX_ANISOTROPY,
            E_OP_MIPMAP_BIAS,
            E_OP_COLOUR_OP,
            E_OP_REPLACE,
            E_OP_ADD,
            E_OP_MODULATE,
            E_OP_ALPHA_BLEND,
            E_OP_COLOUR_OP_EX,
            E_OP_SOURCE1,
            E_OP_SOURCE2,
            E_OP_MODULATE_X2,
            E_OP_MODULATE_X4,
            E_OP_ADD_SIGNED,
            E_OP_ADD_SMOOTH,
            E_OP_SUBTRACT,
            E_OP_BLEND_DIFFUSE_COLOUR,
            E_OP_BLEND_DIFFUSE_ALPHA,
            E_OP_BLEND_TEXTURE_ALPHA,
            E_OP_BLEND_CURRENT_ALPHA,
            E_OP_BLEND_MANUAL,
            E_OP_DOT_PRODUCT,
            E_OP_SRC_CURRENT,
            E_OP_SRC_TEXTURE,
            E_OP_SRC_DIFFUSE,
            E_OP_SRC_SPECULAR,
            E_OP_SRC_MANUAL,
            E_OP_COLOUR_OP_MULTIPASS_FALLBACK,
            E_OP_ALPHA_OP_EX,
            E_OP_ENV_MAP,
            E_OP_SPHERICAL,
            E_OP_PLANAR,
            E_OP_CUBIC_REFLECTION,
            E_OP_CUBIC_NORMAL,
            E_OP_SCROLL,
            E_OP_SCROLL_ANIM,
            E_OP_ROTATE,
            E_OP_ROTATE_ANIM,
            E_OP_SCALE,
            E_OP_WAVE_XFORM,
            E_OP_SCROLL_X,
            E_OP_SCROLL_Y,
            E_OP_SCALE_X,
            E_OP_SCALE_Y,
            E_OP_SINE,
            E_OP_TRIANGLE,
            E_OP_SQUARE,
            E_OP_SAWTOOTH,
            E_OP_INVERSE_SAWTOOTH,
            E_OP_TRANSFORM,
            E_OP_BINDING_TYPE,
            E_OP_VERTEX,
            E_OP_FRAGMENT,
            E_OP_CONTENT_TYPE,
            E_OP_NAMED,
            E_OP_SHADOW,
            E_OP_TEXTURE_SOURCE,
            E_OP_SHARED_PARAMS,
            E_OP_SHARED_PARAM_NAMED,
            E_OP_SHARED_PARAMS_REF,

            E_OP_PARTICLE_SYSTEM,
            E_OP_EMITTER,
            E_OP_AFFECTOR,

            E_OP_COMPOSITOR,
            E_OP_TARGET,
            E_OP_TARGET_OUTPUT,

            E_OP_INPUT,
            E_OP_PREVIOUS,
            E_OP_TARGET_WIDTH,
            E_OP_TARGET_HEIGHT,
            E_OP_TARGET_WIDTH_SCALED,
            E_OP_TARGET_HEIGHT_SCALED,
            E_OP_COMPOSITOR_LOGIC,
            E_OP_TEXTURE_REF,
            E_OP_SCOPE_LOCAL,
            E_OP_SCOPE_CHAIN,
            E_OP_SCOPE_GLOBAL,
            E_OP_POOLED,
            E_OP_NO_FSAA,
            E_OP_DEPTH_POOL,
            E_OP_ONLY_INITIAL,
            E_OP_VISIBILITY_MASK,
            E_OP_LOD_BIAS,
            E_OP_MATERIAL_SCHEME,
            E_OP_SHADOWS_ENABLED,

            E_OP_CLEAR,
            E_OP_STENCIL,
            E_OP_RENDER_SCENE,
            E_OP_RENDER_QUAD,
            E_OP_IDENTIFIER,
            E_OP_FIRST_RENDER_QUEUE,
            E_OP_LAST_RENDER_QUEUE,
            E_OP_QUAD_NORMALS,
            E_OP_CAMERA_FAR_CORNERS_VIEW_SPACE,
            E_OP_CAMERA_FAR_CORNERS_WORLD_SPACE,

            E_OP_BUFFERS,
            E_OP_COLOUR,
            E_OP_DEPTH,
            E_OP_COLOUR_VALUE,
            E_OP_DEPTH_VALUE,
            E_OP_STENCIL_VALUE,

            E_OP_CHECK,
            E_OP_COMP_FUNC,
            E_OP_REF_VALUE,
            E_OP_MASK,
            E_OP_FAIL_OP,
            E_OP_KEEP,
            E_OP_INCREMENT,
            E_OP_DECREMENT,
            E_OP_INCREMENT_WRAP,
            E_OP_DECREMENT_WRAP,
            E_OP_INVERT,
            E_OP_DEPTH_FAIL_OP,
            E_OP_PASS_OP,
            E_OP_TWO_SIDED,
            E_OP_READ_BACK_AS_TEXTURE,
            // Suport for shader model 5.0
            // More program IDs
            E_OP_TESSELLATION_HULL_PROGRAM,
            E_OP_TESSELLATION_DOMAIN_PROGRAM,
            E_OP_COMPUTE_PROGRAM,
            E_OP_TESSELLATION_HULL_PROGRAM_REF,
            E_OP_TESSELLATION_DOMAIN_PROGRAM_REF,
            E_OP_COMPUTE_PROGRAM_REF,
            // More binding IDs
            E_OP_GEOMETRY,
            E_OP_TESSELLATION_HULL,
            E_OP_TESSELLATION_DOMAIN,
            E_OP_COMPUTE,

            // Support for subroutine
            E_OP_SUBROUTINE,

            // added during 1.11. re-sort for 1.12
            E_OP_LINE_WIDTH,
            E_OP_SAMPLER,
            E_OP_SAMPLER_REF,
            E_OP_THREAD_GROUPS,
            E_OP_RENDER_CUSTOM,


            E_OP_END_BUILTIN_IDS
        };

        enum BuiltInValue
        {
            E_BV_ON = 1,
            E_BV_OFF = 2,
            E_BV_TRUE = 1,
            E_BV_FALSE = 2,
            E_BV_YES = 1,
            E_BV_NO = 2
        };

        enum BuiltinType
        {
            E_BT_NONE = 0,
            E_BT_REAL,
            E_BT_INT,
        };

        enum NodeType
        {
            E_NT_UNKNOWN,
            E_NT_ATOM,
            E_NT_OBJECT,
            E_NT_PROPERTY,
            E_NT_IMPORT,
            E_NT_VARIABLE_SET,
            E_NT_VARIABLE_ACCESS
        };

    public:
        /** 析构函数 */
        virtual ~ScriptParserBase();

        /** 解析数据流到object对象中 */
        virtual TResult parseObject(
            DataStream &stream, 
            Object *object, 
            uint32_t version) = 0;

    protected:
        /** 构造函数 */
        ScriptParserBase();

        /** 解析子对象 */
        virtual TResult parseObjects(
            DataStream &stream, Object *object, uint32_t version);

        /** 解析一个字符串 */
        TResult parseString(DataStream &stream, String &str);

        /** 解析一个颜色值 */
        TResult parseColor(DataStream &stream, ColorARGB &color);

        /** 解析一个矩阵 */
        TResult parseMatrix(DataStream &stream, Matrix4 &m);

        /** 給無名對象生成一個名字 */
        String generateName(const String &prefix);

        char    *mText;     /**< 用于存放读取字符串，避免重复申请内存，只在不够的时候动态扩展大小 */
        size_t  mTextLen;   /**< 用户存放读取字符串的缓冲区大小 */

        size_t  mNameSuffix;    /**< 用於生成無名對象的名字 */
    };
}


#endif  /*__T3D_SCRIPT_PARSER_BASE_H__*/
