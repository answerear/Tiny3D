/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
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


#ifndef __T3D_COMMON_H__
#define __T3D_COMMON_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    const String BLANKSTRING;

    enum ComponentOrder : uint32_t
    {
        INVALID = 0,
        TRANSFORM = 0x00000100L,
        CAMERA = TRANSFORM + 1,
        COLLIDER = CAMERA + 1,
        RENDERABLE = COLLIDER + 1,
    };

    /**
     * @enum    PixelFormat
     * @brief   Values that represent pixel formats
     */
    enum class PixelFormat : uint32_t
    {
        E_PF_UNKNOWN = 0,
        E_PF_PALETTE8,
        E_PF_R5G6B5,
        E_PF_A1R5G5B5,
        E_PF_A4R4G4B4,
        E_PF_R8G8B8,
        E_PF_B8G8R8,
        E_PF_A8R8G8B8,
        E_PF_B8G8R8A8,
        E_PF_X8R8G8B8,
        E_PF_B8G8R8X8,
    };

    /**
     * @enum    LightType
     * @brief   Values that represent light types
     */
    enum class LightType : uint32_t
    {
        POINT = 0,
        DIRECTIONAL,
        SPOTLIGHT
    };

    /**
     * @enum    CompareFunction
     * @brief   比较函数，用于深度缓冲测试和模板缓冲测试
     */
    enum class CompareFunction : uint32_t
    {
        ALWAYS_FAIL,
        ALWAYS_PASS,
        LESS,
        LESS_EQUAL,
        EQUAL,
        NOT_EQUAL,
        GREATER_EQUAL,
        GREATER
    };

    /**
     * @typedef uint32_t TrackVertexColorType
     * @brief   Defines an alias representing type of the track vertex color
     */
    typedef uint32_t TrackVertexColorType;

    /**
     * @enum    TrackVertexColor
     * @brief   Values that represent track vertex colors
     */
    enum TrackVertexColor : uint32_t
    {
        NONE     = 0x0,
        AMBIENT  = 0x1,
        DIFFUSE  = 0x2,
        SPECULAR = 0x4,
        EMISSIVE = 0x8
    };

    /**
     * @enum    CullingMode
     * @brief   背面剔除模式
     */
    enum class CullingMode : uint32_t
    {
        NONE = 0,        /**< 不做消隐面剔除 */
        CLOCKWISE,       /**< 按照顶点顺时针顺序的消隐面剔除 */
        ANTICLOCKWISE,   /**< 按照顶点逆时针顺序的消隐面剔除 */
        MAX
    };

    /**
     * @enum    ManualCullingMode
     * @brief   Values that represent manual culling modes
     */
    enum class ManualCullingMode : uint32_t
    {
        NONE,
        BACK,
        FRONT
    };

    /**
     * @enum    ShadingMode
     * @brief   Values that represent shading modes
     */
    enum class ShadingMode : uint32_t
    {
        FLAT,
        GOURAUD,
        PHONG
    };

    /**
     * @brief 多边形渲染模式
     */
    enum class PolygonMode : uint32_t
    {
        NONE,
        POINT,             /**< 顶点模式 */
        WIREFRAME,         /**< 线框模式 */
        SOLID,             /**< 着色模式 */
        MAX
    };

    /**
     * @enum    IlluminationStage
     * @brief   Values that represent illumination stages
     */
    enum class IlluminationStage : uint32_t
    {
        UNKNOWN,
        AMBIENT,
        PER_LIGHT,
        DECAL
    };

    /**
     * @enum    FogMode
     * @brief   Values that represent fog modes
     */
    enum class FogMode : uint32_t
    {
        NONE,
        EXP,
        EXP2,
        LINEAR
    };

    /**
     * @enum    FilterType
     * @brief   Values that represent filter types
     */
    enum class FilterType : uint32_t
    {
        MIN,
        MAG,
        MIP
    };

    /**
     * @enum    FilterOptions
     * @brief   Values that represent filter options
     */
    enum class FilterOptions : uint32_t
    {
        NONE,
        POINT,
        LINEAR,
        ANISOTROPIC
    };

    enum class TexFilterOptions : uint32_t
    {
        NONE,
        BILINEAR,
        TRILINEAR,
        ANISOTROPIC
    };

    /**
     * @enum    TextureAddressMode
     * @brief   Values that represent texture address modes
     */
    enum class TextureAddressMode : uint32_t
    {
        UNKNOWN = 0,
        WRAP,
        MIRROR,
        CLAMP,
        BORDER,
    };

    /**
     * @struct  UVWAddressMode
     * @brief   An uvw address mode.
     */
    struct UVWAddressMode
    {
        TextureAddressMode u, v, w;

        UVWAddressMode()
            : u(TextureAddressMode::CLAMP)
            , v(TextureAddressMode::CLAMP)
            , w(TextureAddressMode::CLAMP)
        {}
    };

    /**
     * @enum    WaveformType
     * @brief   Values that represent waveform types
     */
    enum class WaveformType : uint32_t
    {
        SINE,
        TRIANGLE,
        SQUARE,
        SAWTOOTH,
        INVERSE_SAWTOOTH,
        PWM
    };

    /**
     * @enum    FileSubType
     * @brief   Values that represent file sub types
     */
    enum FileSubType : uint32_t
    {
        E_FST_SCRIPT = 0x00000001,
    };


    #define T3D_MAGIC                   "T3D"

    /**
     * @struct  T3DFileHeader
     * @brief   A 3D file header.
     */
    struct T3DFileHeader
    {
        char        magic[4];   /**< 文件魔数，用于标识文件 */
        uint32_t    subtype;    /**< The subtype */
        uint32_t    version;    /**< 文件版本号 */
        size_t      filesize;   /**< 文件大小 */
    };
}


#endif  /*__T3D_COMMON_H__*/
