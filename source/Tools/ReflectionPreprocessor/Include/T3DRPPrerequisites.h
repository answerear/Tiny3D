/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
