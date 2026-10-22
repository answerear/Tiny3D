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

#ifndef __T3D_IMAGE_CONVERTER_OPTIONS_H__
#define __T3D_IMAGE_CONVERTER_OPTIONS_H__


#include "T3DImageConverterPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief ImageConverter 参数选项
     * \remarks 所有导入设置在选项上带 has* 覆盖标记，以便区分用户显式指定与读取已有 .meta / 类型默认
     */
    struct ImageConverterOptions : public Allocator
    {
        enum class UUIDPolicy
        {
            kReuse = 0,
            kNew,
        };

        /// 输入文件列表 (-i <f1> [f2 ...])
        StringArray         srcPaths {};

        /// 单张输出文件完整路径 (-o <path>，仅单张有效)
        String              dstFilePath {};

        /// 批量输出目录 (-O <dir>)
        String              dstDir {};

        /// 是否写出配对 .ttex.meta (-a)
        bool                isGeneratingMeta {false};

        /// UUID 处理策略 (-u reuse|new)
        UUIDPolicy          uuidPolicy {UUIDPolicy::kReuse};

        /// 显式指定单张 UUID (--uuid <hex32>，仅单张有效)
        UUID                explicitUUID {UUID::INVALID};

        /// 强制重新生成，忽略 SourceHash 缓存 (-f)
        bool                forceRebuild {false};

        /// 是否将 .ttex 导出为文本(JSON)格式 (--text，默认二进制)
        bool                isTextMode {false};

        /// 输出详细过程日志 (-v)
        bool                verbose {false};

        /// 显示帮助 (-? / -h)
        bool                showHelp {false};

        // ---- 显式覆盖标记与值 ----
        bool                hasTextureType {false};
        TextureImportType   textureType {TextureImportType::kDefault};

        bool                hasPremultipliedAlpha {false};
        bool                premultipliedAlpha {false};

        bool                hasGenerateMipmaps {false};
        bool                generateMipmaps {true};

        bool                hasFlipY {false};
        bool                flipY {false};

        bool                hasMaxSize {false};
        uint32_t            maxSize {4096};

        bool                hasFilter {false};
        FilterOptions       filter {FilterOptions::kLinear};

        bool                hasWrap {false};
        TextureAddressMode  wrap {TextureAddressMode::kWrap};

        // ---- IPC 模式选项（仅进程启动命令行使用） ----
        bool                ipcMode {false};
        uint16_t            ipcPort {0};
        String              ipcToken {};

        /**
         * \brief 根据 TextureImportType 生成类型默认导入设置
         */
        static TextureImportSettings makeTypeDefaults(TextureImportType type);

        /**
         * \brief 合并最终生效的 TextureImportSettings
         * \param [in] existingSettings : 从已有 .ttex.meta 读取的设置（若存在则传入指针，否则传 nullptr）
         * \return 合并后的 TextureImportSettings
         */
        TextureImportSettings resolveSettings(const TextureImportSettings *existingSettings) const;
    };
}


#endif  /*__T3D_IMAGE_CONVERTER_OPTIONS_H__*/
