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


#include "LauncherPrerequisites.h"


namespace Tiny3D
{
    NS_BEGIN(Launcher)

    enum LauncherWidgetID
    {
        ID_MAIN_WINDOW = 1,

        ID_MAIN_MENU,

        ID_MAIN_TAB_BAR,
        ID_TAB_ITEM_LOCAL_PROJECT,
        ID_TAB_ITEM_STORE_PROJECT,

        ID_LOCAL_PROJECT_VIEW,
        ID_LOCAL_PROJECT_TOP_VIEW,
        ID_LOCAL_PROJECT_BL_VIEW,
        ID_LOCAL_PROJECT_BR_VIEW,

        ID_DIALOG_NEW_PROJECT,
    };
    
    NS_END
}

