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


#include "ToolBar.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    ImTextureID ToolBar::loadTexture(const String &imageName)
    {
        ArchivePtr archive = T3D_ARCHIVE_MGR.getArchive(Dir::getAppPath(), Archive::AccessMode::kRead);
        T3D_ASSERT(archive != nullptr, "Archive must be not nullptr !");
        ImagePtr image = T3D_IMAGE_MGR.loadImage(archive, imageName);
        T3D_ASSERT(image != nullptr, "Load image failed !");
        Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D(imageName, image);
        return texture->getPixelBuffer()->getRHIResource()->getNativeObject();
    }

    //--------------------------------------------------------------------------

    TResult ToolBar::addImageButton(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, const String &tips, const String &shortcut)
    {
        ImTextureID texID = loadTexture(imageName);
        return ImToolBar::addImageButton(id, texID, queryEnabled, clicked, tips, shortcut);
    }

    //--------------------------------------------------------------------------
    
    TResult ToolBar::addImageButton(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, uint32_t clickedEvtID, const String &tips, const String &shortcut)
    {
        ImTextureID texID = loadTexture(imageName);
        return ImToolBar::addImageButton(id, texID, queryEnabled, clickedEvtID, tips, shortcut);
    }

    //--------------------------------------------------------------------------
    
    TResult ToolBar::addPushImageButton(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, const String &tips, const String &shortcut)
    {
        ImTextureID texID = loadTexture(imageName);
        return ImToolBar::addPushImageButton(id, texID, queryEnabled, queryChecked, clicked, tips, shortcut);
    }

    //--------------------------------------------------------------------------
    
    TResult ToolBar::addPushImageButton(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, uint32_t clickedEvtID, const String &tips, const String &shortcut)
    {
        ImTextureID texID = loadTexture(imageName);
        return ImToolBar::addPushImageButton(id, texID, queryEnabled, queryChecked, clickedEvtID, tips, shortcut);
    }
    
    //--------------------------------------------------------------------------
    
    NS_END
}
