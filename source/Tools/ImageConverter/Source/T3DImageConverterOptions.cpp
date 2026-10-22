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


#include "T3DImageConverterOptions.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TextureImportSettings ImageConverterOptions::makeTypeDefaults(TextureImportType type)
    {
        TextureImportSettings s;
        s.TextureType = type;

        switch (type)
        {
        case TextureImportType::kUI:
            s.PremultipliedAlpha = true;
            s.GenerateMipmaps    = false;
            s.FlipY              = false;
            s.MaxSize            = 4096;
            s.sRGB               = true;
            s.Sampler.MinFilter  = FilterOptions::kLinear;
            s.Sampler.MagFilter  = FilterOptions::kLinear;
            s.Sampler.MipFilter  = FilterOptions::kNone;
            s.Sampler.AddressU   = TextureAddressMode::kClamp;
            s.Sampler.AddressV   = TextureAddressMode::kClamp;
            s.Sampler.AddressW   = TextureAddressMode::kClamp;
            break;

        case TextureImportType::kNormalMap:
            s.PremultipliedAlpha = false;
            s.GenerateMipmaps    = true;
            s.FlipY              = false;
            s.MaxSize            = 4096;
            s.sRGB               = false;
            s.Sampler.MinFilter  = FilterOptions::kLinear;
            s.Sampler.MagFilter  = FilterOptions::kLinear;
            s.Sampler.MipFilter  = FilterOptions::kLinear;
            s.Sampler.AddressU   = TextureAddressMode::kWrap;
            s.Sampler.AddressV   = TextureAddressMode::kWrap;
            s.Sampler.AddressW   = TextureAddressMode::kWrap;
            break;

        case TextureImportType::kDefault:
        default:
            s.PremultipliedAlpha = false;
            s.GenerateMipmaps    = true;
            s.FlipY              = false;
            s.MaxSize            = 4096;
            s.sRGB               = true;
            s.Sampler.MinFilter  = FilterOptions::kLinear;
            s.Sampler.MagFilter  = FilterOptions::kLinear;
            s.Sampler.MipFilter  = FilterOptions::kLinear;
            s.Sampler.AddressU   = TextureAddressMode::kWrap;
            s.Sampler.AddressV   = TextureAddressMode::kWrap;
            s.Sampler.AddressW   = TextureAddressMode::kWrap;
            break;
        }

        return s;
    }

    //--------------------------------------------------------------------------

    TextureImportSettings ImageConverterOptions::resolveSettings(const TextureImportSettings *existingSettings) const
    {
        // 优先级：显式选项 > 已有 .ttex.meta 中的 ImportSettings > TextureType 类型默认
        TextureImportType effectiveType = TextureImportType::kDefault;
        if (hasTextureType)
        {
            effectiveType = textureType;
        }
        else if (existingSettings != nullptr)
        {
            effectiveType = existingSettings->TextureType;
        }

        TextureImportSettings result = makeTypeDefaults(effectiveType);

        // 若有历史 .meta 且未显式覆盖类型（或即使覆盖了类型，未被命令行显式覆盖的字段可从历史设置继承？
        // 注意：若用户显式指定了 -t ui，但历史 meta 是 kDefault，则未显式指定的字段若直接拿历史 meta 会把 GenerateMipmaps=true 带过来，违反 -t ui 默认行为。
        // 因此：当 existingSettings 存在且 (!hasTextureType || existingSettings->TextureType == effectiveType) 时，先以 existingSettings 为基底；
        // 若显式切换了 TextureType，则以新类型的默认值为基底，再叠加其他显式覆盖项）
        if (existingSettings != nullptr && (!hasTextureType || existingSettings->TextureType == effectiveType))
        {
            result = *existingSettings;
            result.TextureType = effectiveType;
        }

        if (hasPremultipliedAlpha)
        {
            result.PremultipliedAlpha = premultipliedAlpha;
        }

        if (hasGenerateMipmaps)
        {
            result.GenerateMipmaps = generateMipmaps;
        }

        if (hasFlipY)
        {
            result.FlipY = flipY;
        }

        if (hasMaxSize)
        {
            result.MaxSize = maxSize;
        }

        if (hasFilter)
        {
            result.Sampler.MinFilter = filter;
            result.Sampler.MagFilter = filter;
        }

        // MipFilter 联动规则：GenerateMipmaps == false 时固定为 kNone；GenerateMipmaps == true 时跟随 --filter
        if (!result.GenerateMipmaps)
        {
            result.Sampler.MipFilter = FilterOptions::kNone;
        }
        else
        {
            result.Sampler.MipFilter = (result.Sampler.MinFilter == FilterOptions::kPoint)
                ? FilterOptions::kPoint
                : FilterOptions::kLinear;
        }

        if (hasWrap)
        {
            result.Sampler.AddressU = wrap;
            result.Sampler.AddressV = wrap;
            result.Sampler.AddressW = wrap;
        }

        return result;
    }
}
