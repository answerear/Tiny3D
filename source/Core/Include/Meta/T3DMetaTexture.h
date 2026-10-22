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

#ifndef __T3D_META_TEXTURE_H__
#define __T3D_META_TEXTURE_H__


#include "T3DMeta.h"
#include "Render/T3DSamplerState.h"


namespace Tiny3D
{
#if defined(T3D_OS_DESKTOP)

    /**
     * \brief 纹理导入类型
     */
    TENUM()
    enum class TextureImportType : uint32_t
    {
        kDefault = 0,
        kUI,
        kNormalMap,
    };

    /**
     * \brief 纹理导入设置（存储于 MetaTexture，仅包含数据字段，转换逻辑在 ImageConverter 中实现）
     */
    TSTRUCT()
    struct T3D_ENGINE_API TextureImportSettings
    {
        /// 纹理导入类型
        TPROPERTY()
        TextureImportType   TextureType {TextureImportType::kDefault};

        /// 是否预乘 alpha（UI 类型默认 true，Default/NormalMap 默认 false）
        TPROPERTY()
        bool                PremultipliedAlpha {false};

        /// 是否生成完整 mipmap 链（Default/NormalMap 默认 true，UI 类型由 imgconv 填为 false）
        TPROPERTY()
        bool                GenerateMipmaps {true};

        /// 是否垂直翻转
        TPROPERTY()
        bool                FlipY {false};

        /// 最大边长上限
        TPROPERTY()
        uint32_t            MaxSize {4096};

        /// 是否为 sRGB 颜色空间（预留，本期不生效）
        TPROPERTY()
        bool                sRGB {true};

        /// 压缩格式标识（预留，本期不生效）
        TPROPERTY()
        String              Compression {};

        /// 采样器描述
        TPROPERTY()
        SamplerDesc         Sampler {};
    };

    /**
     * \brief Texture 资源（.ttex / .ttexture）的 .meta 元数据
     * \remarks UUID 与对应 Texture 资源的 UUID 一致
     */
    TCLASS()
    class T3D_ENGINE_API MetaTexture : public Meta
    {
        TRTTI_ENABLE(Meta)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建 MetaTexture 实例
         * \param [in] uuid : 与 Texture 资源相同的 UUID
         * \return 新建的 MetaTexturePtr
         */
        static MetaTexturePtr create(const UUID &uuid = UUID::generate())
        {
            return T3D_NEW MetaTexture(uuid);
        }

        /**
         * \brief 返回 kTexture 类型标识
         * \return Meta::kTexture
         */
        Type getType() const override { return kTexture; }

        /**
         * \brief 获取源数据与导入设置组合哈希
         * \return 哈希字符串
         */
        TPROPERTY(RTTRFuncName="SourceHash", RTTRFuncType="getter")
        const String &getSourceHash() const { return mSourceHash; }

        /**
         * \brief 设置源数据与导入设置组合哈希
         * \param [in] hash : 哈希字符串
         */
        TPROPERTY(RTTRFuncName="SourceHash", RTTRFuncType="setter")
        void setSourceHash(const String &hash) { mSourceHash = hash; }

        /**
         * \brief 获取纹理导入设置
         * \return 纹理导入设置常引用
         */
        TPROPERTY(RTTRFuncName="ImportSettings", RTTRFuncType="getter")
        const TextureImportSettings &getImportSettings() const { return mImportSettings; }

        /**
         * \brief 获取可修改的纹理导入设置
         * \return 纹理导入设置引用
         */
        TextureImportSettings &getImportSettings() { return mImportSettings; }

        /**
         * \brief 设置纹理导入设置
         * \param [in] settings : 纹理导入设置
         */
        TPROPERTY(RTTRFuncName="ImportSettings", RTTRFuncType="setter")
        void setImportSettings(const TextureImportSettings &settings) { mImportSettings = settings; }

    protected:
        /// 默认构造
        MetaTexture() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 与 Texture 资源相同的 UUID
         */
        MetaTexture(const UUID &uuid)
            : Meta(uuid)
        {
        }

        /// 源文件字节 + 序列化后设置的 hash，用于增量导入
        String                  mSourceHash {};

        /// 纹理导入参数设置
        TextureImportSettings   mImportSettings {};
    };

#endif
}


#endif    /*__T3D_META_TEXTURE_H__*/
