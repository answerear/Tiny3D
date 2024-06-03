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

// For debug
// $(SolutionDir)../System/Source/ $(SolutionDir)../System/Generated/ -x c++ -std=c++11 -I$(SolutionDir)../System/Include/ -I$(SolutionDir)../Platform/Include/ -I$(SolutionDir)../External/rttr/src/
// $(SolutionDir)../System/Include/T3DObject.h $(SolutionDir)../System/Generated -x c++ -std=c++14 -fsyntax-only -DRTTR_DLL -DT3DSYSTEM_EXPORT -DT3DSystem_EXPORTS -DT3D_ERR_CORE=0x00004000 -DT3D_ERR_FRAMEWORK=0x00003000 -DT3D_ERR_LOG=0x00002000 -DT3D_ERR_PLATFORM=0x00001000 -DT3D_ERR_PLUGINS=0x00010000 -DT3D_OS_DESKTOP -DT3D_OS_WINDOWS -DT3D_OS_X64 -D_HAS_EXCEPTIONS=0 -D_USRDLL -I$(SolutionDir)../External/rttr/src -I$(SolutionDir)../Platform/Include -I$(SolutionDir)../System/Include -I$(SolutionDir)External/rttr/src -I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\include" -I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt"
// $(SolutionDir)../Core/Include/Bound/T3DBound.h $(SolutionDir)../Core/Generated -x c++ -std=c++14 -fsyntax-only -DRTTR_DLL -DT3DCORE_EXPORT -DT3DCore_EXPORTS -DT3D_ERR_CORE=0x00004000 -DT3D_ERR_FRAMEWORK=0x00003000 -DT3D_ERR_LOG=0x00002000 -DT3D_ERR_PLATFORM=0x00001000 -DT3D_ERR_PLUGINS=0x00010000 -DT3D_OS_DESKTOP -DT3D_OS_WINDOWS -DT3D_OS_X64 -D_HAS_EXCEPTIONS=0 -D_USRDLL -I$(SolutionDir)..\\Core\\..\\Platform\\Include -I$(SolutionDir)..\\Core\\..\\System\\Include -I$(SolutionDir)..\\Core\\..\\Math\\Include -I$(SolutionDir)..\\Core\\..\\Log\\Include -I$(SolutionDir)..\\Core\\..\\Utils\\Include -I$(SolutionDir)..\\Core\\..\\Framework\\Include -I$(SolutionDir)..\\Core\\Include -I$(SolutionDir)..\\Core\\Source -I$(SolutionDir)..\\Core -IE:\\workspace\\private\\Tiny3D\\dependencies\\SDL2\\include\\SDL2 -I$(SolutionDir)..\\External\\rttr\\src -I$(SolutionDir)External\\rttr\\src -I$(SolutionDir)..\\External\\rapidjson -I$(SolutionDir)..\\External\\protobuf\\src -I$(SolutionDir)..\\Core\\..\\..\\dependencies\\zlib\\include -I$(SolutionDir)External\\rttr\\src\\rttr\\.. -I$(SolutionDir)..\\External\\rttr\\src\\rttr\\.. -I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\include" -I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt"
// $(SolutionDir)..\nmake\System $(SolutionDir)..\System -r


//-----------------------------------------------------------------------------
// 1. CMake 生成 ReflectionPreprocessor 及其依赖的 Platform、Log 三个不具备反射功能的基础工程
// 2. 编译 ReflectionPreprocessor 工程，生成可执行文件 ReflectionPreprocessor.exe
// 3. CMake 用 ninja 生成不具备反射功能工程以外的其他工程对应的 compile_commands.json
// 4. ReflectionPreprocessor 根据 compile_commands.json 生成反射代码
// 5. CMake 生成引擎工程
// https://github.com/microsoft/vswhere
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 1. 用 CMake 生成各工程 compile_commands.json 文件
//      1.1 Windows 平台用 nmake 生成 compile_commands.json
//      1.2 Mac & Linux 平台用 make 生成 compile_commands.json
// 2. 用工具提取 compile_commands.json 中 -I 和 -D 的参数，生成反射配置文件 ReflectionSettings.json 文件
// 3. CMake 生成 ReflectionPreprocessor 及其依赖的 Platform、Log、Utils 三个不具备反射功能的基础工程
// 4. 编译 ReflectionPreprocessor 工程，生成可执行文件 ReflectionPreprocessor.exe
// 5. ReflectionPreprocessor 读取 ReflectionSettings.json 文件里面关于 clang 生辰反射代码需要的参数并生成反射代码到输出目录
// 6. 修改 CMake 文件，重新生成所有支持反射的源码工程
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
    
    Platform* platform = new Platform();
    Logger *logger = new Logger();
    T3D_LOG_STARTUP(1000, LOG_TAG, true, true);

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
    
    T3D_LOG_SHUTDOWN();

    delete logger;
    delete platform;

    return ret;
}

