

#ifndef __T3D_LOGGER_H__
#define __T3D_LOGGER_H__



#include "T3DSingleton.h"
#include "T3DLoggerMacro.h"


namespace Tiny3D
{
	class T3D_LOG_API Logger : public Singleton<Logger>
	{
		T3D_DISABLE_COPY(Logger);

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
		Logger();
		virtual ~Logger();

		bool startup(uint32_t unAppID, const TString &strTag, bool bForceOutput = false);

		void trace(ELevel eLevel, const char *pszFileName, int32_t nLine, const char *pszFmt, ...);

		void shutdown();

		void enterBackground();

		void enterForeground();
	};
}

#define T3D_LOGGER			Tiny3D::Logger::getInstance()


#endif	/*__T3D_LOGGER_H__*/
