

#ifndef __T3D_LOGGER_H__
#define __T3D_LOGGER_H__



#include "T3DSingleton.h"
#include "T3DLoggerMacro.h"


namespace Tiny3D
{
	class T3D_LOG_API T3DLogger : public T3DSingleton<T3DLogger>
	{
		T3D_DISABLE_COPY(T3DLogger);

	public:
		enum ELevel
		{
			E_LEVEL_OFF = 0,		/// None log
			E_LEVEL_FATAL,			/// Fatal error
			E_LEVEL_CRITICAL,		/// Critical error
			E_LEVEL_ERROR,			/// Normal error
			E_LEVEL_WARNING,		/// Warning
			E_LEVEL_INFO,			/// Information
			E_LEVEL_DEBUG,			/// Debug
			E_LEVEL_MAX,			/// Max number of log level
		};

	public:
		T3DLogger();
		virtual ~T3DLogger();

		bool startup(uint32_t unAppID, const TString &strVersion, const TString &strTag, bool bForceOutput = false);

		void trace(ELevel eLevel, const char *pszFileName, int32_t nLine, const char *pszFmt, ...);

		void shutdown();

		void enterBackground();

		void enterForeground();
	};
}


#endif	/*__T3D_LOGGER_H__*/
