

#ifndef __T3D_TEXT_CODEC_H__
#define __T3D_TEXT_CODEC_H__


#include "T3DSingleton.h"
#include "T3DPlatformMacro.h"


namespace Tiny3D
{
	class T3D_PLATFORM_API TextCodec : public Singleton<TextCodec>
	{
		T3D_DISABLE_COPY(TextCodec);
		
	public:
		TextCodec(const TString &strCodecFile);
		virtual ~TextCodec();
// 
// 		/** GBK字符串有效性检查，过滤掉被截断的不完整gbk码 */
// 		bool checkGBKString(char *pText, int32_t nTextLength);
// 
// 		/** 是否是gbk首字节 */
// 		bool IsGBKLeadByte(uchar_t ucChar) const;
// 
// 		/** 是否gbk字符 */
// 		bool IsGBKChar(uint16_t ushChar) const;
// 
// 		/** 是否gb2312字符 */
// 		bool IsGB2312Char(uint16_t ushChar) const;
// 
// 		/** UTF-8转换成Unicode */
// 		bool UTF8ToUnicode(wchar_t &wchar, const char szInString[3]);
// 
// 		/** Unicode转换成UTF-8 */
// 		bool UnicodeToUTF8(char szOutString[3], wchar_t wchar);
// 
// 		/** 把Unicode转换成GB2312 */
// 		uint32_t UnicodeToGB2312(char *pszOutString, const wchar_t *pwszInString, uint32_t unInSize = -1);
// 
// 		/** 把Unicode转换成GB2312 */
// 		uint32_t UnicodeToGB2312Ex(TString &strOut, const wchar_t *pwszInString, uint32_t unInSize = -1);
// 
// 		/** UTF-8转为GB2312 */
// 		uint32_t UTF8ToGB2312(char *pszOutString, const char *pszInString, uint32_t unInSize = -1);
// 
// 		/** UTF-8转为GB2312 */
// 		uint32_t UTF8ToGB2312Ex(TString &strOut, const char *pszInString, uint32_t unInSize = -1);
// 
// 		/** GB2312转换成Unicode */
// 		uint32_t GB2312ToUnicode(wchar_t *pwszOutString, const char *pszInString, uint32_t unInSize = -1);
// 
// 		/** GB2312转换成Unicode */
// 		uint32_t GB2312ToUnicodeEx(TWString &wstrOut, const char *pszInString, uint32_t unInSize = -1);
// 
// 		/** GB2312转为UTF-8 */
// 		uint32_t GB2312ToUTF8(char *pszOutString, const char *pszInString, uint32_t unInSize = -1);
// 
// 		/** GB2312转为UTF-8 */
// 		uint32_t GB2312ToUTF8Ex(TString &strOut, const char *pszInString, uint32_t unInSize = -1);
// 
// 		bool TrimString(const TString &strSrc, TString &strDst);
// 
// 		bool IsEmoji(int32_t codePoint);
// 		TString FilterEmoji(const TString &str);
// 
// 		char CharToHex(char input);
// 		TString StringToHexString(const TString &strSrc);
// 
// 		bool SubUTF8String(TString& sourceStr,int start,int size);
// 
// 	protected:
// 		bool InitCodeTableFromFile(const TString &strCodecFile);
// 
// 		int32_t DecodeUtfCodeTwoChar(uchar_t a, uchar_t b);
// 		int32_t DecodeUtfCodeThreeChar(uchar_t a, uchar_t b, uchar_t c);
// 		int32_t DecodeUtfCodeFourChar(uchar_t a, uchar_t b, uchar_t c , uchar_t d);
// 		int32_t DecodeUtfCodeFiveChar(uchar_t a, uchar_t b, uchar_t c , uchar_t d, uchar_t e);
// 		int32_t DecodeUtfCodeSixChar(uchar_t a, uchar_t b, uchar_t c , uchar_t d, uchar_t e, uchar_t f);
// 
	protected:
		uint16_t	m_ushCodeTable[65536][2];
	};
}


#endif	/*__T3D_TEXT_CODEC_H__*/
