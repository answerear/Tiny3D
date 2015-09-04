
#include "T3DLogger.h"


T3D_INIT_SINGLETON(Tiny3D::T3DLogger);


namespace Tiny3D
{
	T3DLogger::T3DLogger()
	{

	}

	T3DLogger::~T3DLogger()
	{

	}

	bool T3DLogger::startup(uint32_t unAppID, const TString &strVersion, const TString &strTag, bool bForceOutput /* = false */)
	{
		return true;
	}

	void T3DLogger::trace(ELevel eLevel, const char *pszFileName, int32_t nLine, const char *pszFmt, ...)
	{
		
	}

	void T3DLogger::shutdown()
	{

	}

	void T3DLogger::enterBackground()
	{

	}

	void T3DLogger::enterForeground()
	{

	}
}