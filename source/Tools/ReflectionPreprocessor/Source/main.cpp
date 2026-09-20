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

//-----------------------------------------------------------------------------
// 反射生成链路（CMake 原生）：
// 1. CMake configure 从各 target 的 include / 宏写出 ReflectionSettings.json
//    （file(GENERATE)，按 $<CONFIG> 各一份）
// 2. 同一次 configure 把 rpp 与引擎模块放进同一个工程；产物清单按源文件预声明
// 3. 构建期先编 rpp，再对每个模块跑 rpp <settings-dir> <source-dir>
//    rpp 只读 ReflectionSettings.json，不读 compile_commands.json
// 4. 各模块编译预声明的 *.generated.cpp
//
// 调试参数示例：
//   <binary>/System <source>/System -r
//   <binary>/Core/Runtime <source>/Core -r -j 8 -W Tiny3D::TAabb;...
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 限制 :
// 1. 非 template 类的 *.h/*.hpp 头文件，一定要有对应同名的 *.cpp/*.cxx/*.c 文件
// 2. 不支持 template 函数里面嵌套 enum
// 3. 不支持默认参数
// 4. 函数内定义的类、结构体、枚举均无法生成反射代码，即使加了标签也无法生成
//-----------------------------------------------------------------------------


#include "T3DRPPrerequisites.h"
#include "T3DPreprocessorCommand.h"
#include "T3DReflectionGenerator.h"
#include "T3DReflectionPreprocessor.h"


#if defined(T3D_DEBUG)
// #define T3D_RP_DEBUG_GENERATOR
#endif

int main(int argc, char *argv[])
{
    using namespace Tiny3D;

    Dir::setCachePathInfo("Tiny3D", "Tiny3D");
    
    Platform* platform = T3D_NEW Platform();
    Logger *logger = T3D_NEW Logger();
    T3D_LOG_STARTUP(1000, LOG_TAG, true, true);

    int64_t startTime = DateTime::currentMSecsSinceEpoch();

    int ret = 0;
    
    do 
    {
        if (argc == 1)
        {
            RP_LOG_ERROR("Not enough parameters !");
            ret = -1;
            break;
        }

#if defined(T3D_RP_DEBUG_GENERATOR)
        String srcPath, dstPath;
        ClangArgs args;
        PreprocessorCommand cmd;
        cmd.parse4Debug(argc, argv, srcPath, dstPath, args);
        Dir::makeDir(dstPath);
        String path, title, ext;
        Dir::parsePath(srcPath, path, title, ext);
        String dumpPath = path + Dir::getNativeSeparator() + title + ".json";
        dstPath = dstPath + Dir::getNativeSeparator() + title + ".generated.cpp"; 
        ReflectionGenerator generator;
        ret = generator.generateAST(srcPath, args);        
        generator.dumpReflectionInfo(dumpPath);
#else
        ReflectionPreprocessor processor;
        ret = processor.execute(argc, argv);
#endif
    } while (false);

    int64_t elapsed = DateTime::currentMSecsSinceEpoch() - startTime;
    DateTime dt(elapsed);
    RP_LOG_INFO("ReflectionPreprocessor completed in %s",
        dt.timeToString(DateTime::TimeFormat::HH_MM_SS_XXX).c_str());
    
    T3D_LOG_SHUTDOWN();

    T3D_SAFE_DELETE(logger);

    MemoryManager::getInstance().exit();
    T3D_SAFE_DELETE(platform);

    return ret;
}

