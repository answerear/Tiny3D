

#include "T3DConsole.h"
#include "T3DConsoleInterface.h"
#include "T3DFactoryInterface.h"
#include <windows.h>


T3D_INIT_SINGLETON(Tiny3D::Console);


namespace Tiny3D
{
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
// 			char *pszBuffer = new char[MAX_CONTENT_SIZE];
// 			memset(pszBuffer, 0, MAX_CONTENT_SIZE);
			char szBuffer[MAX_CONTENT_SIZE] = {0};

			va_list vl;
			va_start(vl, pszFmt);
			vsnprintf(szBuffer, MAX_CONTENT_SIZE, pszFmt, vl);
			va_end(vl);

			m_pconsoleAdapter->print(szBuffer);

// 			T3D_SAFE_DELETE_ARRAY(pszBuffer);
		}
	}
}
