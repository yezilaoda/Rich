#ifndef _CX_LOG_H_
#define	_CX_LOG_H_

#include <stdio.h>
#include <time.h>
#include <assert.h>

#include <list>
#include <iterator>
#include <stdexcept>
#include <exception>

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)  
	#include <comdef.h>
#endif

#include <tchar.h>
#include <process.h>
#include <windows.h>
#include <shlobj.h>
#include <Shlwapi.h>		// PathFileExists

//////////////////////////////////////////////////////////////////////////
///文件型多线程日志类 class for log file

#ifndef CX_LOG_DEFAULT_FORMAT
	#if (defined(_MSC_VER) && _MSC_VER<=1310) || defined(__BORLANDC__)  || (defined(__GNUC__) && defined(_UNICODE))
		#define CX_LOG_DEFAULT_FORMAT _T("%s %s %s(%d): %s\r\n")
	#else
		#define CX_LOG_DEFAULT_FORMAT _T("%s%s%s%s %s\r\n")
	#endif
#endif

#ifndef CX_LOG_DEFAULT_FORMAT_SIZE
	#define CX_LOG_DEFAULT_FORMAT_SIZE 1024
#endif


class CxLog
{
public:
	enum EnumType{
		CX_LOG_MESSAGE = 0,
		CX_LOG_WARNING,
		CX_LOG_EXCEPTION,
		CX_LOG_ERROR
	};
	struct Item
	{
		SYSTEMTIME _Time;//>time stamp
		TCHAR _szTime[30];

		LPTSTR _szSrc;//>source file name
		LPTSTR _szFunc;//>founction name
		ULONG _uLine;//>line number

		EnumType _eType;//
		LPTSTR _szDesc;//>content

		LPBYTE _pBin;//>binary data szBuffer
		ULONG _uBinSize;//>the size of binary data szBuffer

		Item()
		{
			InitItem(NULL, NULL, 0, CX_LOG_MESSAGE, NULL, NULL, 0);
		}

		Item(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, EnumType eType,
			LPCTSTR szDesc, LPVOID pBin = NULL, ULONG uSize = 0)
		{
			InitItem(szSrc, szFunc, uLine, eType, szDesc, pBin, uSize);
		}

		~Item()
		{
			ReleaseStringBuffer(_szSrc);
			ReleaseStringBuffer(_szFunc);
			ReleaseStringBuffer(_szDesc);

			if (_pBin)
			{
				delete[]_pBin;
				_pBin = NULL;
			}
		}

		VOID InitItem(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, EnumType eType,
			LPCTSTR szDesc, LPVOID pBin, ULONG uSize)
		{
			GetLocalTime(&_Time);
			wsprintf(_szTime, _T("[%d/%02d/%02d %02d:%02d:%02d:%03d]"),
				_Time.wYear,
				_Time.wMonth,
				_Time.wDay,
				_Time.wHour,
				_Time.wMinute,
				_Time.wSecond,
				_Time.wMilliseconds
				);

			_eType = eType;
			_uBinSize = _uLine = 0;
			_szSrc = _szFunc = _szDesc = NULL;
			_pBin = NULL;

			if (szSrc)
			{
				LPCTSTR p = szSrc;

#ifndef CX_LOG_FULL_SOURCE_NAME
				p = szSrc + _tcslen(szSrc);
				while (p > szSrc && *p != _T('\\'))
					p--;

				if (*p == _T('\\'))
					p++;
#endif

				AllocStringBuffer(_szSrc, p);
				_uLine = uLine;
			}

			AllocStringBuffer(_szFunc, szFunc);
			AllocStringBuffer(_szDesc, szDesc);

			if (pBin && uSize)
			{
				_pBin = new BYTE[uSize];
				assert(_pBin);
				memcpy(_pBin, pBin, uSize);
				_uBinSize = uSize;
			}
		}

		VOID AllocStringBuffer(LPTSTR& szDest, LPCTSTR& szSrc)
		{
			if (szSrc)
			{
				ULONG uSize = _tcslen(szSrc) + 1;
				szDest = new TCHAR[uSize];
				assert(szDest);
				memcpy(szDest, szSrc, uSize*sizeof(TCHAR));
			}
		}

		VOID ReleaseStringBuffer(LPTSTR& lpDest)
		{
			if (lpDest)
			{
				delete[]lpDest;
				lpDest = NULL;
			}
		}

		LPTSTR Format(LPTSTR szBuffer, ULONG uSize, ULONG* pSize = NULL)
		{
			static LPCTSTR szType[] = { _T(""), _T("Warning"), _T("Exception"), _T(" Error") };

			if (!szBuffer)
				return szBuffer;

			int iLen;

#if (defined(_MSC_VER) && _MSC_VER<=1310) || defined(__BORLANDC__)  || (defined(__GNUC__) && defined(_UNICODE))
			iLen = _sntprintf(szBuffer, uSize,
				CX_LOG_DEFAULT_FORMAT,
				_szTime, szType[_eType],
				_szSrc ? _szSrc : _T(""), _uLine,
				_szDesc ? _szDesc : _T(""));
#else
			iLen = _sntprintf_s(szBuffer, uSize, _TRUNCATE,
				CX_LOG_DEFAULT_FORMAT,
				_szTime, szType[_eType],
				_szSrc ? _szSrc : _T(""), _szFunc ? _szFunc : _T(""),
				_szDesc ? _szDesc : _T(""));
#endif

			if (iLen > 4 && !_tcsncmp(szBuffer + iLen - 4, _T("\r\n"), 2))
				*(szBuffer + iLen - 2) = TCHAR(NULL), iLen -= 2;

			if (pSize)
				*pSize = iLen;

			return szBuffer;
		}
	};
	static CxLog& Instance();

	VOID SetLogFileName(LPCTSTR filename);
	VOID SetCashierMode();
	VOID SetLogByDay();

public:
	CxLog(bool &alive);
	~CxLog();
	VOID Destroy();
	VOID Lock();
	VOID Unlock();

	VOID Log(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, EnumType eType,
		LPCTSTR szDesc, LPVOID pBin = NULL, ULONG uSize = 0);

	VOID LogBin(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, EnumType eType, LPVOID pBin, ULONG uSize);

	VOID LogN(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, EnumType eType, LPCTSTR szFormat, ...);

	VOID LogLastError(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, DWORD dwError, LPCTSTR szFormat, ...);

#ifdef _INC_COMDEF
	VOID Log(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, _com_error &e);
#endif
	VOID Log(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, const std::exception *e);

	VOID Log(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, const std::exception &e);

protected:
	bool& _bAlive;
	bool  _bCashier;
	bool  _bLogByDay;
	CRITICAL_SECTION _LogMutex;
	std::list<Item*> _ItemList;
	TCHAR _szFileName[MAX_PATH];
	HANDLE _hThreadHandle, _LogHandle, _TerminateEvent;

	virtual VOID Run();

private:
	CxLog(const CxLog&);
	CxLog& operator=(CxLog&);

#ifdef _MT
	static UINT APIENTRY StaticThreadProc(LPVOID lpPara); //允许C多线程运行库
#else
	static DWORD WINAPI StaticThreadProc(LPVOID lpPara);
#endif
};

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#define __WFILE__ WIDEN(__FILE__)
#ifdef _UNICODE
	#define __TFILE__ __WFILE__
#else
	#define __TFILE__ __FILE__
#endif

#if (_MSC_VER && _MSC_VER<=1200) || (__BORLANDC__) 
	#define __FUNCTION__ NULL
	#define __WFUNCTION__ NULL
#else //_MSC_VER>1200 __GNUC__ __INTEL_COMPILER
	#define __WFUNCTION__ WIDEN(__FUNCTION__)
#endif

#ifdef _UNICODE
	#ifdef __GNUC__
		#define __TFUNCTION__ NULL
	#else
		#define __TFUNCTION__ __WFUNCTION__
	#endif
#else
	#define __TFUNCTION__ __FUNCTION__
#endif


#define LOGE(e) CxLog::Instance().Log(__TFILE__, __TFUNCTION__, __LINE__, (e))
//#define BASE_LOGN(type, msg, ...) CxLog::Instance().LogN(__TFILE__, __TFUNCTION__, __LINE__, (type), (msg), ##__VA_ARGS__)
#define BASE_LOGN(type, msg, ...) CxLog::Instance().LogN(_T(""), _T(""), 0, (type), (msg), ##__VA_ARGS__)
#define LOGINFO(msg, ...) BASE_LOGN(CxLog::CX_LOG_MESSAGE, _T(msg), ##__VA_ARGS__)
#define LOGWARNING(msg, ...) BASE_LOGN(CxLog::CX_LOG_WARNING, _T(msg), ##__VA_ARGS__)
#define LOGERROR(msg, ...) BASE_LOGN(CxLog::CX_LOG_ERROR, _T(msg), ##__VA_ARGS__)


#define LOG_LAST_ERROR(msg, ...) CxLog::Instance().LogLastError(_T(""), _T(""), 0, GetLastError(),_T(msg), ##__VA_ARGS__)

#endif //_CX_LOG_H_ 
