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

#ifndef __T3D_RP_PREREQUISITES_H__
#define __T3D_RP_PREREQUISITES_H__


#include <T3DPlatformLib.h>
#include <T3DLog.h>
#include <T3DUtils.h>

#define RAPIDJSON_HAS_STDSTRING 1
#include <prettywriter.h>
#include <document.h>
#include <rapidjson.h>
#include <error/en.h>

// #include "clang/Tooling/Tooling.h"
// #include "clang/Tooling/CommonOptionsParser.h"
// #include "clang/Lex/Preprocessor.h"
// #include "clang/AST/RecursiveASTVisitor.h"
// #include "clang/AST/ASTConsumer.h"
// #include "clang/Frontend/FrontendAction.h"
// #include "clang/Frontend/CompilerInstance.h"

#include <clang-c/Index.h>

#if defined(T3D_DEBUG)
#define T3D_RP_DEBUG
#endif

namespace Tiny3D
{
    #define LOG_TAG                 "ReflectionPreprocessor"

    #define RP_LOG_ERROR(fmt, ...)   \
        T3D_LOG_ERROR(LOG_TAG, fmt, ##__VA_ARGS__);
        // printf(fmt, ##__VA_ARGS__); \
        // printf("\n");   \
        //

    #define RP_LOG_WARNING(fmt, ...) \
        T3D_LOG_WARNING(LOG_TAG, fmt, ##__VA_ARGS__);
        // printf(fmt, ##__VA_ARGS__); \
        // printf("\n");   \
        //

    #define RP_LOG_INFO(fmt, ...)    \
        T3D_LOG_INFO(LOG_TAG, fmt, ##__VA_ARGS__);
        // printf(fmt, ##__VA_ARGS__); \
        // printf("\n");   \
        //

    #define RP_LOG_DEBUG(fmt, ...)   \
        T3D_LOG_DEBUG(LOG_TAG, fmt, ##__VA_ARGS__);
        // printf(fmt, ##__VA_ARGS__); \
        // printf("\n");   \
        //
    
    class JsonStream
    {
    public:
        typedef signed char Ch;

        JsonStream(DataStream& stream) 
            : mStream(stream)
        {
        }

        Ch Peek() const
        {
            Ch c;

            if (mStream.eof())
            {
                c = '\0';
            }
            else
            {
                mStream >> c;
                long_t len = sizeof(Ch);
                mStream.seek(-len, true);
            }

            return c;
        }

        Ch Take() const
        {
            Ch c;

            if (mStream.eof())
            {
                c = '\0';
            }
            else
            {
                mStream >> c;
            }

            return c;
        }

        size_t Tell() const
        {
            return (size_t)mStream.tell();
        }

        Ch* PutBegin()
        {
            return nullptr;
        }

        void Put(Ch c)
        {
            mStream << c;
        }

        void Flush()
        {
        }

        size_t PutEnd(Ch*)
        {
            return 0;
        }

    private:
        DataStream& mStream;
    };
}

#endif  /*__T3D_RP_PREREQUISITES_H__*/
