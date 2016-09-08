#include "stdafx.h"
#include "log.h"

CxLog& CxLog::Instance()
{
	static bool alive = false;
	static CxLog log(alive);

	if (!alive)
	{
		OutputDebugString(_T("CxLog has destroy!"));
		throw std::runtime_error("CxLog has destroy!");
	}

	return log;
}

VOID CxLog::SetLogFileName(LPCTSTR filename)
{
	TCHAR szLogPath[MAX_PATH];

	TCHAR szAppDataDirectory[MAX_PATH];
	SHGetFolderPath(0, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, 0, 0, szAppDataDirectory);

	if (_bCashier)
	{
		wsprintf(szLogPath, _T("C:\\Program Files\\Yazuo\\"));
		if (PathFileExists(szLogPath) == 0)
		{
			if (CreateDirectory(szLogPath, NULL) == 0)		// 如果无法创建目录，说明权限不足，使用标准路径
			{
				wsprintf(szLogPath, _T("%s\\Yazuo\\"), szAppDataDirectory);
			}
		}
	}
	else
	{
		wsprintf(szLogPath, _T("%s\\Rich\\"), szAppDataDirectory);
	}

	if (PathFileExists(szLogPath) == 0)
	{
		CreateDirectory(szLogPath, NULL);
	}

	if (_bLogByDay)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		wsprintf(_szFileName, _T("%s%s%04d-%02d-%02d.log"), szLogPath, filename, st.wYear, st.wMonth, st.wDay);
	}
	else
	{
		wsprintf(_szFileName, _T("%s%s"), szLogPath, filename);
	}

}

VOID CxLog::SetCashierMode()
{
	_bCashier = true;
}

VOID CxLog::SetLogByDay()
{
	_bLogByDay = true;
}

CxLog::CxLog(bool &alive) : _bAlive(alive)
{
	_bAlive = true;
	_bCashier = false;
	_bLogByDay = false;
	_TerminateEvent = CreateEvent(0, TRUE, FALSE, NULL);
	_LogHandle = CreateEvent(0, FALSE, FALSE, NULL);
	::InitializeCriticalSection(&_LogMutex);

#ifdef _MT
	unsigned int id;
	_hThreadHandle = (HANDLE)::_beginthreadex(NULL, 0, StaticThreadProc, this, 0, &id);
#else
	DWORD id;
	_hThreadHandle = ::CreateThread(NULL, 0, StaticThreadProc, this, 0, &id);
#endif	
}

CxLog::~CxLog()
{
	Destroy();
}


VOID CxLog::Destroy()
{
	_bAlive = false;
	if (_hThreadHandle)
	{
		SetEvent(_TerminateEvent);
		if (::WaitForSingleObject(_hThreadHandle, 500) != WAIT_OBJECT_0)
			::TerminateThread(_hThreadHandle, 0);
		CloseHandle(_hThreadHandle);
	}
	::DeleteCriticalSection(&_LogMutex);

	FILETIME createTime;
	HANDLE hFile = NULL;
	SYSTEMTIME stFile;
	SYSTEMTIME stUTC;
	SYSTEMTIME st;
	GetLocalTime(&st);

	hFile = CreateFile(_szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);

	if (NULL == hFile || INVALID_HANDLE_VALUE == hFile)
	{
		hFile = NULL;
	}
	else
	{
		GetFileTime(hFile, &createTime, NULL, NULL);
		CloseHandle(hFile);
		hFile = NULL;
		FileTimeToSystemTime(&createTime, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stFile);
		// 日志2个月清理一次
		if (st.wYear >= stFile.wYear)
		{
			WORD currentMonth = st.wMonth + (st.wYear - stFile.wYear) * 12;
			if (currentMonth - stFile.wMonth >= 2)
				DeleteFile(_szFileName);
		}
	}
}

VOID CxLog::Lock()
{
	::EnterCriticalSection(&_LogMutex);
}

VOID CxLog::Unlock()
{
	::LeaveCriticalSection(&_LogMutex);
}

VOID CxLog::Log(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, EnumType eType,
	LPCTSTR szDesc, LPVOID pBin, ULONG uSize)
{
	Item* p = new Item(szSrc, szFunc, uLine, eType, szDesc, pBin, uSize);

	Lock();
	_ItemList.push_back(p);
	Unlock();

	SetEvent(_LogHandle);
}

VOID CxLog::LogBin(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, EnumType eType, LPVOID pBin, ULONG uSize)
{
	Log(szSrc, szFunc, uLine, eType, NULL, pBin, uSize);
}

VOID CxLog::LogN(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, EnumType eType, LPCTSTR szFormat, ...)
{
	TCHAR szBuffer[CX_LOG_DEFAULT_FORMAT_SIZE] = { 0 };

	va_list va;
	va_start(va, szFormat);
	_vsntprintf_s(szBuffer, CX_LOG_DEFAULT_FORMAT_SIZE - 1, szFormat, va);
	va_end(va);

	Log(szSrc, szFunc, uLine, eType, szBuffer);
}

VOID CxLog::LogLastError(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, DWORD dwError, LPCTSTR szFormat, ...)
{
	TCHAR szBuffer[CX_LOG_DEFAULT_FORMAT_SIZE] = { 0 };

	va_list va;
	va_start(va, szFormat);
	int iLen1 = _vsntprintf_s(szBuffer, CX_LOG_DEFAULT_FORMAT_SIZE - 1, szFormat, va);
	va_end(va);

	int iLen2 = _stprintf_s(szBuffer + iLen1, CX_LOG_DEFAULT_FORMAT_SIZE, _T("(%d):"), dwError);
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		szBuffer + iLen1 + iLen2,
		CX_LOG_DEFAULT_FORMAT_SIZE - iLen1 - iLen2 - 3,
		NULL
		);

	Log(szSrc, szFunc, uLine, CxLog::CX_LOG_ERROR, szBuffer);
}

#ifdef _INC_COMDEF
VOID CxLog::Log(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, _com_error &e)
{
	TCHAR szBuffer[CX_LOG_DEFAULT_FORMAT_SIZE] = { 0 };
	_sntprintf_s(szBuffer, CX_LOG_DEFAULT_FORMAT_SIZE, _T("_com_error Code=%08X Meaning=%s Source=%s Description=%s"),
		e.Error(),
		(LPCSTR)_bstr_t(e.ErrorMessage()),
		(LPCSTR)e.Source(),
		(LPCSTR)e.Description());

	Log(szSrc, szFunc, uLine, CxLog::CX_LOG_EXCEPTION, szBuffer);
}
#endif

VOID CxLog::Log(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, const std::exception *e)
{
#ifdef _UNICODE
	TCHAR szBuffer[CX_LOG_DEFAULT_FORMAT_SIZE];
	MultiByteToWideChar(CP_ACP, 0, e->what(), -1, szBuffer, CX_LOG_DEFAULT_FORMAT_SIZE);
	Log(szSrc, szFunc, uLine, CxLog::CX_LOG_EXCEPTION, szBuffer);
#else
	Log(szSrc, szFunc, uLine, CxLog::CX_LOG_EXCEPTION, e->what());
#endif
}

VOID CxLog::Log(LPCTSTR szSrc, LPCTSTR szFunc, ULONG uLine, const std::exception &e)
{
#ifdef _UNICODE
	TCHAR szBuffer[CX_LOG_DEFAULT_FORMAT_SIZE];
	MultiByteToWideChar(CP_ACP, 0, e.what(), -1, szBuffer, CX_LOG_DEFAULT_FORMAT_SIZE);
	Log(szSrc, szFunc, uLine, CxLog::CX_LOG_EXCEPTION, szBuffer);
#else
	Log(szSrc, szFunc, uLine, CxLog::CX_LOG_EXCEPTION, e.what());
#endif
}

VOID CxLog::Run()
{
	HANDLE HandleArray[2];
	HandleArray[0] = _LogHandle;
	HandleArray[1] = _TerminateEvent;

	for (;;)
	{
		DWORD ret = ::WaitForMultipleObjects(2, HandleArray, false, INFINITE);
		if (ret == WAIT_OBJECT_0)
		{
			HANDLE hHandle = ::CreateFile(
				_szFileName,
				GENERIC_WRITE,
				FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL
				);

			if (!hHandle)
				return;

			DWORD NumberOfBytesWritten;
#ifdef _UNICODE
			DWORD dwSize = 0;
			dwSize = GetFileSize(hHandle, NULL);
			if (dwSize == 0)
			{
				TCHAR p = (TCHAR)0xfeff;//UNICODE文件开头标志
				WriteFile(hHandle, &p, sizeof(TCHAR), &NumberOfBytesWritten, NULL);
			}
#endif

			SetFilePointer(hHandle, 0, 0, FILE_END);

			try
			{
				Lock();
				while (_ItemList.size())
				{
					Item *p = _ItemList.front();

					TCHAR szBuffer[CX_LOG_DEFAULT_FORMAT_SIZE];
					ULONG uSize = 0;
					p->Format(szBuffer, CX_LOG_DEFAULT_FORMAT_SIZE, &uSize);
					WriteFile(hHandle, szBuffer, uSize*sizeof(TCHAR), &NumberOfBytesWritten, NULL);

					_ItemList.pop_front();
					delete p;
				}
				Unlock();
			}
			catch (...)
			{
				Unlock();
			}

			SetEndOfFile(hHandle);
			CloseHandle(hHandle);
		}

		if (ret == WAIT_OBJECT_0 + 1)
			break;
	}
}
#ifdef _MT
UINT APIENTRY CxLog::StaticThreadProc(LPVOID lpPara) //允许C多线程运行库
#else
DWORD WINAPI CxLog::StaticThreadProc(LPVOID lpPara)
#endif
{
	((CxLog*)(lpPara))->Run();

#ifdef _MT
	::_endthreadex(0);
#endif
	return 0;
}
