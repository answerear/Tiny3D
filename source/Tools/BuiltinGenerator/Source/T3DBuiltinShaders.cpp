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


#include "T3DBuiltinShaders.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TResult BuiltinShaders::generate(const String &rootPath)
    {
        TResult ret = T3D_OK;

        do
        {
            BGEN_LOG_INFO("Begin generating shaders (%s) ...", rootPath.c_str());
            
            String tempPath = rootPath + Dir::getNativeSeparator() + "TempShaders";

            // 先删除掉原来的临时文件夹
            Dir::removeDir(tempPath, true);

            // 重新创建临时文件夹
            if (!Dir::makeDir(tempPath))
            {
                BGEN_LOG_ERROR("Create directory %s failed !", tempPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 清理缓存
            mShaders.clear();

            // 生成临时的 shader 文件
            String searchPath = rootPath + Dir::getNativeSeparator() + "shaders";
            ret = generateShader(searchPath, tempPath);

            BGEN_LOG_INFO("Completed generating shaders (%s) !", rootPath.c_str());
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinShaders::generateShader(const String &searchPath, const String &outputPath)
    {
        TResult ret;

        String searchFile = searchPath + Dir::getNativeSeparator() + "*.shader";
        Dir dir;
        bool working = dir.findFile(searchFile);

        while (working)
        {
            if (dir.isDots())
            {
                // . or ..
            }
            else if (dir.isDirectory())
            {
                // directory
                generateShader(dir.getFilePath(), outputPath);
            }
            else
            {
                // file
                const String filePath = dir.getFilePath();

                BGEN_LOG_INFO("Begin compiling shader (%s) ...", filePath.c_str());
                
                // 使用 shader cross compiler 工具生成临时编译生成的 shader 文件
                String appPath = Dir::getAppPath() + Dir::getNativeSeparator() + "scc.exe";
                    
#if defined (T3D_OS_WINDOWS)
                String cmdLine =  filePath + " -t hlsl" + " -o " + outputPath;
#elif defined (T3D_OS_LINUX)
#elif defined (T3D_OS_MAC)
#endif
                    
                Process process;
                ret = process.start(appPath, cmdLine);
                if (T3D_FAILED(ret))
                {
                    BGEN_LOG_ERROR("Start scc.exe from source file %s failed ! ERROR [%d]", filePath.c_str(), ret);
                    continue;
                }

                // 等待编译结束
                ret = process.wait();
                if (T3D_FAILED(ret))
                {
                    BGEN_LOG_ERROR("Wait process exit from source file %s failed ! ERROR [%d]", filePath.c_str(), ret);
                    continue;
                }

                uint32_t exitCode = process.getExitCode();
                if (exitCode != 0)
                {
                    // 编译出错了，只能退出
                    BGEN_LOG_ERROR("Compile shader %s failed ! ERROR [%d]", filePath.c_str(), ret);
                    continue;
                }

                String path, title, ext;
                Dir::parsePath(filePath, path, title, ext);
                
                // 读取编译后的 shader
                ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(outputPath, "FileSystem", Archive::AccessMode::kRead);
                T3D_ASSERT(archive != nullptr);
                String filename = title + ".t3d";
                ShaderPtr shader = T3D_SHADER_MGR.loadShader(archive, filename);
                T3D_ASSERT(shader != nullptr);
                
                mShaders.emplace(title, shader);

                archive = T3D_ARCHIVE_MGR.loadArchive(outputPath, "FileSystem", Archive::AccessMode::kTruncate);
                T3D_ASSERT(archive != nullptr);
                T3D_SHADER_MGR.saveShader(archive, shader);
                
                BGEN_LOG_INFO("Completed compiling shader (%s) !", filePath.c_str());
            }

            working = dir.findNextFile();
        }

        dir.close();

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
