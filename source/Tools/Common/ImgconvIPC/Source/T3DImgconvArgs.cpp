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


#include "T3DImgconvArgs.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::input(const String &path)
    {
        args.push_back("-i");
        args.push_back(path);
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::inputs(const StringArray &paths)
    {
        if (!paths.empty())
        {
            args.push_back("-i");
            for (const auto &p : paths)
            {
                args.push_back(p);
            }
        }
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::output(const String &path)
    {
        args.push_back("-o");
        args.push_back(path);
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::outputDir(const String &dir)
    {
        args.push_back("-O");
        args.push_back(dir);
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::type(TextureImportType t)
    {
        args.push_back("-t");
        switch (t)
        {
        case TextureImportType::kUI:
            args.push_back("ui");
            break;
        case TextureImportType::kNormalMap:
            args.push_back("normal");
            break;
        case TextureImportType::kDefault:
        default:
            args.push_back("default");
            break;
        }
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::meta(bool enable)
    {
        if (enable)
        {
            args.push_back("-a");
        }
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::uuid(const UUID &id)
    {
        if (id != UUID::INVALID)
        {
            args.push_back("--uuid");
            args.push_back(id.toString());
        }
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::uuid(const String &hex32)
    {
        if (!hex32.empty())
        {
            args.push_back("--uuid");
            args.push_back(hex32);
        }
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::newUuid(bool forceNew)
    {
        args.push_back("-u");
        args.push_back(forceNew ? "new" : "reuse");
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::premultiply(bool enable)
    {
        args.push_back(enable ? "--premultiply" : "--no-premultiply");
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::mips(bool enable)
    {
        args.push_back(enable ? "--mips" : "--no-mips");
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::flipY(bool enable)
    {
        args.push_back(enable ? "--flip-y" : "--no-flip-y");
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::maxSize(uint32_t size)
    {
        args.push_back("--max-size");
        args.push_back(std::to_string(size));
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::filter(FilterOptions f)
    {
        args.push_back("--filter");
        args.push_back(f == FilterOptions::kPoint ? "point" : "linear");
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::wrap(TextureAddressMode m)
    {
        args.push_back("--wrap");
        switch (m)
        {
        case TextureAddressMode::kClamp:
            args.push_back("clamp");
            break;
        case TextureAddressMode::kMirror:
            args.push_back("mirror");
            break;
        case TextureAddressMode::kBorder:
            args.push_back("border");
            break;
        case TextureAddressMode::kWrap:
        default:
            args.push_back("repeat");
            break;
        }
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::force(bool enable)
    {
        if (enable)
        {
            args.push_back("-f");
        }
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::verbose(bool enable)
    {
        if (enable)
        {
            args.push_back("-v");
        }
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs &ImgconvArgs::textMode(bool enable)
    {
        if (enable)
        {
            args.push_back("--text");
        }
        return *this;
    }

    //--------------------------------------------------------------------------

    ImgconvArgs ImgconvArgs::fromImportSettings(
        const String &srcPath,
        const String &dstPath,
        const TextureImportSettings &settings,
        bool writeMeta,
        bool forceRebuild)
    {
        ImgconvArgs builder;
        builder.input(srcPath);
        if (!dstPath.empty())
        {
            builder.output(dstPath);
        }
        builder.type(settings.TextureType);
        builder.premultiply(settings.PremultipliedAlpha);
        builder.mips(settings.GenerateMipmaps);
        builder.flipY(settings.FlipY);
        builder.maxSize(settings.MaxSize);
        builder.filter(settings.Sampler.MinFilter == FilterOptions::kPoint
            ? FilterOptions::kPoint : FilterOptions::kLinear);
        builder.wrap(settings.Sampler.AddressU);
        if (writeMeta)
        {
            builder.meta(true);
        }
        if (forceRebuild)
        {
            builder.force(true);
        }
        return builder;
    }
}
