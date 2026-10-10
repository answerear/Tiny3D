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

#include "T3DRmlCanvas.h"
#include "T3DRmlUiSystem.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RmlCanvas::RmlCanvas(const UUID &uuid)
        : Behaviour(uuid)
    {
    }

    //--------------------------------------------------------------------------

    void RmlCanvas::setDocuments(const TArray<String> &documents)
    {
        mDocuments = documents;
        mDocumentsLoaded = false;
    }

    //--------------------------------------------------------------------------

    void RmlCanvas::onAwake()
    {
        RmlUiSystem *system = RmlUiSystem::getInstance();
        if (system == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RMLUI, "RmlCanvas awoke before RmlUiSystem was ready.");
            return;
        }

        mContextName = getUUID().toString();
        mContext = Rml::CreateContext(mContextName, Rml::Vector2i(1, 1));
        if (mContext == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RMLUI, "Rml::CreateContext failed.");
            return;
        }

        system->registerCanvas(this);
    }

    //--------------------------------------------------------------------------

    void RmlCanvas::onDestroy()
    {
        releaseContext();
        if (RmlUiSystem *system = RmlUiSystem::getInstance())
        {
            system->unregisterCanvas(this);
        }
        Behaviour::onDestroy();
    }

    //--------------------------------------------------------------------------

    void RmlCanvas::releaseContext()
    {
        if (mContext == nullptr)
        {
            return;
        }

        Rml::RemoveContext(mContextName);
        mContext = nullptr;
        mDocumentsLoaded = false;
    }

    //--------------------------------------------------------------------------
}
