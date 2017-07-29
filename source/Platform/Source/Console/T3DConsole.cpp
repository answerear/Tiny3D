/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#include "Console/T3DConsole.h"
#include "Adapter/T3DConsoleInterface.h"
#include "Adapter/T3DFactoryInterface.h"


#if defined T3D_OS_WINDOWS
#include <windows.h>
#else
#endif


namespace Tiny3D
{
    T3D_INIT_SINGLETON(Console);

    const uint32_t Console::MAX_CONTENT_SIZE = 4096;

    Console::Console(FactoryInterface *pAdapterFactory)
        : m_pconsoleAdapter(nullptr)
    {
        if (pAdapterFactory != nullptr)
        {
            m_pconsoleAdapter = pAdapterFactory->createConsoleAdapter();
        }
    }

    Console::~Console()
    {
        T3D_SAFE_DELETE(m_pconsoleAdapter);
    }

    void Console::print(const char *pszFmt, ...)
    {
        if (nullptr != m_pconsoleAdapter)
        {
//          char *pszBuffer = new char[MAX_CONTENT_SIZE];
//          memset(pszBuffer, 0, MAX_CONTENT_SIZE);
            char szBuffer[MAX_CONTENT_SIZE] = {0};

            va_list vl;
            va_start(vl, pszFmt);
            vsnprintf(szBuffer, MAX_CONTENT_SIZE, pszFmt, vl);
            va_end(vl);

            m_pconsoleAdapter->print(szBuffer);

//          T3D_SAFE_DELETE_ARRAY(pszBuffer);
        }
    }
}
