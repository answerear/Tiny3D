/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#pragma once


#include "T3DBuiltinGenPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief BuiltinGenerator 模块内部辅助类，用于在重新生成内置资源时保留已有 guid
     * \note  仅供本模块的 Builtin* 生成类使用，不对外暴露
     */
    class BuiltinGuidUtil
    {
    public:
        /**
         * \brief 读取目标目录下已存在的 .meta 文件里记录的 UUID
         * \param [in] dir : meta 文件所在目录
         * \param [in] metaFilename : meta 文件名（如 "white.ttex.meta"）
         * \return 若 meta 存在且解析成功，返回其记录的 UUID；否则返回 UUID::INVALID
         */
        static UUID readExistingMetaUUID(const String &dir, const String &metaFilename);

        /**
         * \brief 读取已存在的 ShaderLab meta（*.shader.meta）里的两个 UUID
         * \param [in] dir : meta 文件所在目录
         * \param [in] metaFilename : meta 文件名（如 "Tiny3DStandard.shader.meta"）
         * \param [out] outLabUUID : ShaderLab meta 自身的 UUID
         * \param [out] outShaderUUID : 其引用的已编译 Shader 的 UUID
         * \return meta 存在且解析成功返回 true；否则返回 false（两个出参保持 UUID::INVALID）
         */
        static bool readExistingShaderLabMeta(const String &dir, const String &metaFilename,
            UUID &outLabUUID, UUID &outShaderUUID);
    };
}
