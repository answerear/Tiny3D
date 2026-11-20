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


#include "Render/T3DSamplerState.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHISamplerState.h"

#include <cctype>


namespace  Tiny3D
{
    //--------------------------------------------------------------------------

    SamplerStatePtr SamplerState::create(const SamplerDesc &desc, uint32_t hash)
    {
        return T3D_NEW SamplerState(hash, desc);
    }

    //--------------------------------------------------------------------------

    bool SamplerState::inferDescFromName(const String &name, SamplerDesc &desc)
    {
        static const String kSamplerPrefix = "sampler";

        String body = name;
        if (body.size() > kSamplerPrefix.size() && StringUtil::startsWith(body, kSamplerPrefix, true))
        {
            body = body.substr(kSamplerPrefix.size());
        }

        // 按 '_'、数字边界与大小写边界拆词："LinearClampU_RepeatV" -> linear clamp u repeat v，
        // 连续大写算一个词，最后一个大写字母后跟小写时归下一个词："UVClamp" -> uv clamp
        TArray<String> words;
        String word;
        auto flush = [&words, &word]()
        {
            if (!word.empty())
            {
                StringUtil::toLowerCase(word);
                words.push_back(word);
                word.clear();
            }
        };
        for (size_t i = 0; i < body.size(); ++i)
        {
            const char c = body[i];
            if (!std::isalnum(static_cast<unsigned char>(c)))
            {
                flush();
                continue;
            }
            if (!word.empty())
            {
                const char prev = word.back();
                const bool upper = std::isupper(static_cast<unsigned char>(c)) != 0;
                const bool prevLower = std::islower(static_cast<unsigned char>(prev)) != 0;
                const bool prevUpper = std::isupper(static_cast<unsigned char>(prev)) != 0;
                const bool nextLower = i + 1 < body.size() && std::islower(static_cast<unsigned char>(body[i + 1])) != 0;
                const bool digitEdge = (std::isdigit(static_cast<unsigned char>(c)) != 0) != (std::isdigit(static_cast<unsigned char>(prev)) != 0);
                if ((upper && prevLower) || (upper && prevUpper && nextLower) || digitEdge)
                {
                    flush();
                }
            }
            word.push_back(c);
        }
        flush();

        auto isAxes = [](const String &w)
        {
            return !w.empty() && w.size() <= 3 && w.find_first_not_of("uvw") == String::npos;
        };

        bool recognized = false;
        for (size_t i = 0; i < words.size(); ++i)
        {
            const String &w = words[i];

            if (w == "point")
            {
                desc.MinFilter = desc.MagFilter = desc.MipFilter = FilterOptions::kPoint;
                recognized = true;
                continue;
            }
            if (w == "linear" || w == "bilinear")
            {
                desc.MinFilter = desc.MagFilter = FilterOptions::kLinear;
                desc.MipFilter = FilterOptions::kPoint;
                recognized = true;
                continue;
            }
            if (w == "trilinear")
            {
                desc.MinFilter = desc.MagFilter = desc.MipFilter = FilterOptions::kLinear;
                recognized = true;
                continue;
            }
            if (w == "compare" || w == "comparison")
            {
                desc.IsComparison = true;
                desc.CompareFunc = CompareFunction::kLessEqual;
                recognized = true;
                continue;
            }

            TextureAddressMode mode = TextureAddressMode::kNone;
            if (w == "repeat" || w == "wrap")
            {
                mode = TextureAddressMode::kWrap;
            }
            else if (w == "clamp")
            {
                mode = TextureAddressMode::kClamp;
            }
            else if (w == "border")
            {
                mode = TextureAddressMode::kBorder;
            }
            else if (w == "mirror" || w == "mirroronce")
            {
                mode = TextureAddressMode::kMirror;
                if (w == "mirroronce")
                {
                    mode = TextureAddressMode::kMirrorOnce;
                }
                else if (i + 1 < words.size() && words[i + 1] == "once")
                {
                    mode = TextureAddressMode::kMirrorOnce;
                    ++i;
                }
            }
            if (mode == TextureAddressMode::kNone)
            {
                continue;
            }

            recognized = true;
            if (i + 1 < words.size() && isAxes(words[i + 1]))
            {
                const String &axes = words[++i];
                if (axes.find('u') != String::npos) desc.AddressU = mode;
                if (axes.find('v') != String::npos) desc.AddressV = mode;
                if (axes.find('w') != String::npos) desc.AddressW = mode;
            }
            else
            {
                desc.AddressU = desc.AddressV = desc.AddressW = mode;
            }
        }

        return recognized;
    }

    //--------------------------------------------------------------------------

    SamplerState::SamplerState(uint32_t hash, const SamplerDesc &desc)
        : RenderStateResource(hash, desc)
    {
        
    }

    //--------------------------------------------------------------------------

    RenderResource::Type SamplerState::getType() const
    {
        return Type::kSamplerState;
    }

    //--------------------------------------------------------------------------

    bool SamplerState::onLoad()
    {
        mRHIResource = T3D_AGENT.getActiveRHIContext()->createSamplerState(this);
        return true;
    }

    //--------------------------------------------------------------------------

    bool SamplerState::onUnload()
    {
        mRHIResource = nullptr;
        return true;
    }

    //--------------------------------------------------------------------------
}