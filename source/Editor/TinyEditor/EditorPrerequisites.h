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
#include "SDL.h"


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

    #define ARCHIVE_TYPE_METAFS         "MetaFileSystem"
    #define ARCHIVE_TYPE_FS             "FileSystem"

    #define ICON_NAME_FAVORITE          "Editor/icons/d_Favorite@32.png"
    #define ICON_NAME_SEARCH            "Editor/icons/d_Search@32.png"
    #define ICON_NAME_FOLDER            "Editor/icons/d_Folder@32.png"
    #define ICON_NAME_FOLDER_OPENED     "Editor/icons/d_FolderOpened@32.png"
    #define ICON_NAME_PREFAB            "Editor/icons/d_Prefab@64.png"
    #define ICON_NAME_SCENE             "Editor/icons/d_SceneAsset On@64.png"
    #define ICON_NAME_MATERIAL          "Editor/icons/d_Material@64.png"
    #define ICON_NAME_MESH              "Editor/icons/d_PrefabModel@64.png"
    #define ICON_NAME_SHADER            "Editor/icons/d_Shader@64.png"
    #define ICON_NAME_TEXTURE           "Editor/icons/d_Texture@64.png"
    #define ICON_NAME_TEXT              "Editor/icons/d_TextAsset@64.png"
    #define ICON_NAME_DYLIB             "Editor/icons/d_Assembly@64.png"
    #define ICON_NAME_FILE              "Editor/icons/d_DefaultAsset@64.png"
    #define ICON_NAME_BIN               ICON_NAME_TEXT

    #define ICON_NAME_ADD               "Editor/icons/d_CreateAddNew@2x.png"
    #define ICON_NAME_DROPDOWN          "Editor/icons/dropdown-arrow@2x.png"
    #define ICON_NAME_SEARCH            "Editor/icons/d_Search@32.png"
    #define ICON_NAME_SEARCH_JUMP       "Editor/icons/d_SearchJump@32.png"

    #define ICON_NAME_GAMEOBJECT        "Editor/icons/d_GameObject@32.png"

    #define ICON_NAME_SCENE_32          "Editor/icons/d_SceneAsset On@32.png"
    
    // #define TEST_SCENE_ENABLE
    
    class EditorSceneImpl;
    class AssetNode;

    class ImCreateButton;
    class ImSearchInputText;
    
    NS_END

    class ImGuiImpl;
}

