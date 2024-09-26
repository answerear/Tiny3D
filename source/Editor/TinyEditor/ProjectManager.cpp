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


#include "ProjectManager.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)
    
    //--------------------------------------------------------------------------

    const char *ProjectManager::ASSETS = "Assets";
    const char *ProjectManager::SCENES = "Scenes";
    const char *ProjectManager::TEMP = "Temp";
    
    //--------------------------------------------------------------------------

    ProjectManager::ProjectManager()
    {
        mFSMonitor = new FileSystemMonitor();
    }
    
    //--------------------------------------------------------------------------

    ProjectManager::~ProjectManager()
    {
        T3D_SAFE_DELETE(mFSMonitor);
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::setupBuiltinAssets(const String &tempPath)
    {
        TResult ret = T3D_OK;

        do
        {
            // 复制 builtin assets 到 temp 文件夹
            String builtinPath = Dir::getAppPath() + Dir::getNativeSeparator() + "Editor" + Dir::getNativeSeparator() + "builtin";
            
            // textures
            String srcPath = builtinPath + Dir::getNativeSeparator() + "textures";
            String dstPath = tempPath + Dir::getNativeSeparator() + "builtin";
            bool rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                EDITOR_LOG_ERROR("Copy builtin textures from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }
            
            // shaders
            srcPath = builtinPath + Dir::getNativeSeparator() + "shaders";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                EDITOR_LOG_ERROR("Copy builtin shaders from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }
            
            // materials
            srcPath = builtinPath + Dir::getNativeSeparator() + "materials";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                EDITOR_LOG_ERROR("Copy builtin materials from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }

            // meshes
            srcPath = builtinPath + Dir::getNativeSeparator() + "meshes";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                EDITOR_LOG_ERROR("Copy builtin meshes from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }

            // Builtin 资源档案系统
            mBuiltinArchive = T3D_ARCHIVE_MGR.loadArchive(dstPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadTxtTruncate);
            if (mBuiltinArchive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load builtin fs archive [%s]", builtinPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::createProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            // 创建工程文件夹
            String projectPath = path + Dir::getNativeSeparator() + name;
            if (!Dir::makeDir(projectPath))
            {
                EDITOR_LOG_ERROR("Failed to create project folder (%s) !", projectPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 创建资产文件夹
            String assetsPath = projectPath + Dir::getNativeSeparator() + ASSETS;
            if (!Dir::makeDir(assetsPath))
            {
                EDITOR_LOG_ERROR("Failed to create assets folder (%s) !", assetsPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // Assets 档案系统
            mAssetsArchive = T3D_ARCHIVE_MGR.loadArchive(assetsPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadTxtTruncate);
            if (mAssetsArchive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load assets fs archive [%s]", assetsPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            // 创建 Temp 文件夹
            String tempPath = projectPath + Dir::getNativeSeparator() + TEMP;
            if (!Dir::makeDir(tempPath))
            {
                EDITOR_LOG_ERROR("Failed to create temporary folder (%s) !", tempPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 设置内置资产
            ret = setupBuiltinAssets(tempPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to setup builtin assets !");
                break;
            }

            // 预编译工程 shaders 和内置 shaders
            ret = compileAllShaders(tempPath, assetsPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to compile shaders !");
                break;
            }
            
            mPath = projectPath;
            mName = name;
            mAssetsPath = assetsPath;
            mTempPath = tempPath;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::openProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            // 工程目录
            String projectPath = path + Dir::getNativeSeparator() + name;
            if (!Dir::exists(projectPath))
            {
                // 工程目录不存在
                EDITOR_LOG_ERROR("Open project [%s] failed !", projectPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            // 资源目录
            String assetsPath = projectPath + Dir::getNativeSeparator() + ASSETS;
            if (!Dir::exists(assetsPath))
            {
                // 资源目录不存在
                EDITOR_LOG_ERROR("Assets folder [%s] did not exist !", assetsPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            // Assets 档案系统
            mAssetsArchive = T3D_ARCHIVE_MGR.loadArchive(assetsPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadTxtTruncate);
            if (mAssetsArchive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load assets fs archive [%s]", assetsPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            // 临时目录
            String tempPath = projectPath + Dir::getNativeSeparator() + TEMP;
            if (!Dir::exists(tempPath))
            {
                // 临时目录不存在，重新创建
                EDITOR_LOG_WARNING("Temporary folder [%s] did not exist ! Create that !", tempPath.c_str());
                if (!Dir::makeDir(tempPath))
                {
                    EDITOR_LOG_ERROR("Failed to create temporary folder [%s] !", tempPath.c_str());
                    break;
                }
            }

            // 设置内置资产
            ret = setupBuiltinAssets(tempPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to setup builtin assets !");
                break;
            }

            // 预编译工程 shaders 和内置 shaders
            ret = compileAllShaders(tempPath, assetsPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to compile shaders !");
                break;
            }

            mPath = projectPath;
            mName = name;
            mAssetsPath = assetsPath;
            mTempPath = tempPath;
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::closeProject()
    {
        mPath.clear();
        mName.clear();
        mAssetsPath.clear();
        mTempPath.clear();
        mCompiledShadersPath.clear();

        T3D_ARCHIVE_MGR.unloadArchive(mAssetsArchive);
        mAssetsArchive = nullptr;

        T3D_ARCHIVE_MGR.unloadArchive(mBuiltinArchive);
        mBuiltinArchive = nullptr;

        T3D_ARCHIVE_MGR.unloadArchive(mCompiledShadersArchive);
        mCompiledShadersArchive = nullptr;
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::compileAllShaders(const String &tempPath, const String &assetsPath)
    {
        TResult ret = T3D_OK;

        do
        {
            // 编译后 Shaders 存放路径
            String compiledShadersPath = tempPath + Dir::getNativeSeparator() + "shaders";
            if (!Dir::exists(compiledShadersPath))
            {
                if (!Dir::makeDir(compiledShadersPath))
                {
                    EDITOR_LOG_ERROR("Failed to create shaders folder (%s) !", compiledShadersPath.c_str());
                    ret = T3D_ERR_FAIL;
                    break;
                }
            }

            // 编译内置 shaders
            String builtinPath = tempPath + Dir::getNativeSeparator() + "builtin";
            ret = compileShaders(builtinPath, compiledShadersPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to compile builtin shaders !");
                break;
            }

            // 编译工程 shaders
            ret = compileShaders(assetsPath, compiledShadersPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to compile project shaders !");
                break;
            }

            // 编译后 shaders 档案系统
            mCompiledShadersArchive = T3D_ARCHIVE_MGR.loadArchive(compiledShadersPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadTxtTruncate);
            if (mCompiledShadersArchive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load compiled shaders fs archive [%s]", compiledShadersPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

            mCompiledShadersPath = compiledShadersPath;
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::compileShaders(const String &inputPath, const String &outputPath)
    {
        TResult ret = T3D_OK;

        String searchPath = inputPath + Dir::getNativeSeparator() + "*.shader";
        Dir dir;
        bool working = dir.findFile(searchPath);

        while (working)
        {
            if (dir.isDots())
            {
                // . or ..
            }
            else if (dir.isDirectory())
            {
                // directory
                ret = compileShaders(dir.getFilePath(), outputPath);
            }
            else
            {
                // file
                const String filePath = dir.getFilePath();
                ret = compileShader(filePath, outputPath);
            }

            if (T3D_FAILED(ret))
            {
                break;
            }
            
            working = dir.findNextFile();
        }

        dir.close();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::compileShader(const String &inputPath, const String &outputPath)
    {
        TResult ret = T3D_OK;

        do
        {
            EDITOR_LOG_INFO("Begin compiling shader %s ...", inputPath.c_str());
            
            // 从 meta 中读取出 UUID
            String metaPath = inputPath + ".meta";
            FileDataStream fs;
            if (!fs.open(metaPath.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
            {
                EDITOR_LOG_ERROR("Failed to open meta file [%s] !", metaPath.c_str());
                break;
            }

            MetaPtr meta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
            T3D_ASSERT(meta->getType() == Meta::Type::kShaderLab);
            
            fs.close();

            MetaShaderLab *metaShaderLab = (MetaShaderLab*)meta.get();
            
            // 使用 shader cross compiler 工具生成临时编译生成的 shader 文件
#if defined (T3D_OS_WINDOWS)
            String appPath = Dir::getAppPath() + Dir::getNativeSeparator() + "scc.exe";
            String cmdLine =  inputPath + " -t hlsl" + " -o " + outputPath + " -u " + metaShaderLab->getShaderUUID().toString();
#elif defined (T3D_OS_LINUX)
#elif defined (T3D_OS_MAC)
#endif

            Process process;
            ret = process.start(appPath, cmdLine);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to start scc.exe from source file (%s) ! ERROR [%d]", inputPath.c_str(), ret);
                break;
            }

            // 等待编译结束
            ret = process.wait();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to wait process exiting from source file (%s) ! ERROR [%d]", inputPath.c_str(), ret);
                break;
            }

            uint32_t exitCode = process.getExitCode();
            if (exitCode != 0)
            {
                // 编译出错了，只能退出
                EDITOR_LOG_ERROR("Failed to compile shader (%s) ! ERROR [%d]", inputPath.c_str(), ret);
                break;
            }

            EDITOR_LOG_INFO("Completed compiling shader !");
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
    
    NS_END
}

