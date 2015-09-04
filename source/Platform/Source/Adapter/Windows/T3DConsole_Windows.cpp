
#include "T3DConsole_Windows.h"
#include <windows.h>


namespace Tiny3D
{
	Console_Windows::Console_Windows()
	{

	}

	void Console_Windows::print(const char *pText)
	{
		printf(pText);
		OutputDebugString(pText);
	}
}
