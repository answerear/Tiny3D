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

#pragma once


#include <Tiny3D.h>
#include <imgui.h>
#include <TinyImGui.h>
#include "Language/Language.h"


#define NS_BEGIN(name)  namespace name {
#define NS_END    }

namespace Tiny3D
{
    NS_BEGIN(Editor)

    #define LOG_TAG_EDITOR "Editor"

    #define EDITOR_LOG_ERROR(fmt, ...)   T3D_LOG_ERROR(LOG_TAG_EDITOR, fmt, ##__VA_ARGS__)
    #define EDITOR_LOG_WARNING(fmt, ...) T3D_LOG_WARNING(LOG_TAG_EDITOR, fmt, ##__VA_ARGS__)
    #define EDITOR_LOG_INFO(fmt, ...)    T3D_LOG_INFO(LOG_TAG_EDITOR, fmt, ##__VA_ARGS__)
    #define EDITOR_LOG_DEBUG(fmt, ...)   T3D_LOG_DEBUG(LOG_TAG_EDITOR, fmt, ##__VA_ARGS__)

    #define ARCHIVE_TYPE_METAFS     "MetaFileSystem"
    #define ARCHIVE_TYPE_FS         "FileSystem"

    NS_END

    class ImGuiImpl;
    class EditorScene;
}

