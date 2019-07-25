// LogFile.cpp : 实现文件
//

#include "stdafx.h"
#include "LogFile.h"
#include <locale>

// CLogFile

CLogFile::CLogFile()
	: m_strFilePath(_T(""))
	, m_strErrorMessage(_T(""))
{
	InitLogFile();
}

CLogFile::~CLogFile()
{
	if (m_hMutex != NULL)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
}


// CLogFile 成员函数


BOOL CLogFile::InitLogFile(void)
{
	TCHAR szFilePath[MAX_PATH];
	TCHAR szPathName[MAX_PATH];
	TCHAR szDriver[MAX_PATH];
	TCHAR szDirectory[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	TCHAR szExt[MAX_PATH];
	TCHAR szMutexName[MAX_PATH];

	ZeroMemory(szFilePath, MAX_PATH * sizeof(TCHAR));
	ZeroMemory(szPathName, MAX_PATH * sizeof(TCHAR));
	ZeroMemory(szDriver, MAX_PATH * sizeof(TCHAR));
	ZeroMemory(szDirectory, MAX_PATH * sizeof(TCHAR));
	ZeroMemory(szFileName, MAX_PATH * sizeof(TCHAR));
	ZeroMemory(szExt, MAX_PATH * sizeof(TCHAR));
	ZeroMemory(szMutexName, MAX_PATH * sizeof(TCHAR));

	GetModuleFileName(NULL, szPathName, MAX_PATH);
	_tsplitpath_s(szPathName, szDriver, _MAX_DRIVE, szDirectory, _MAX_DIR, szFileName, _MAX_FNAME, szExt, _MAX_EXT);

	wsprintf(szFilePath, _T("%s%slog"), szDriver, szDirectory);

	CreateDirectory(szFilePath, NULL);

	m_strFilePath = szFilePath;

	m_hMutex = CreateMutex(NULL, FALSE, _T("Mutex{66656B0B-232C-49A6-B268-084EFA164E3B}"));
	if (m_hMutex == NULL)
	{
		DWORD dwError = GetLastError();
		m_strErrorMessage.Format(_T("CreateMutex Error. Error code : %d"), dwError);
		return FALSE;
	}

	return TRUE;
}


BOOL CLogFile::WriteLog(DWORD dwError, CString strExtraInfo)
{
	CString strErrorCode;
	strErrorCode.Format(_T(". Error Code : %d"), dwError);

	CString strErrorMessage;
	strErrorMessage.Empty();
	strErrorMessage = strExtraInfo + strErrorCode;

	return WriteLog(strErrorMessage);
}


BOOL CLogFile::WriteLog(CString strMessage)
{
	CStdioFile File;
	SYSTEMTIME SysTime;
	DWORD dwError = 0;

	CString strFileName;
	GetLocalTime(&SysTime);
	strFileName.Format(_T("%s\\%d%02d%02d.txt"), m_strFilePath.GetBuffer(0), SysTime.wYear, SysTime.wMonth, SysTime.wDay);

	WaitForSingleObject(m_hMutex, INFINITE);

	try
	{
		if (!File.Open(strFileName, CFile::modeReadWrite | CFile::modeNoTruncate | CFile::modeCreate))
		{
			dwError = GetLastError();
			ReleaseMutex(m_hMutex);
			m_strErrorMessage.Empty();
			m_strErrorMessage.Format(_T("CreateFile Error! Error Code : %d"), dwError);
			return FALSE;
		}

		File.SeekToEnd();

		CString strMessageTemp;
		strMessageTemp.Format(_T("%02d:%02d:%02d : %s\r\n"), SysTime.wHour, SysTime.wMinute, SysTime.wSecond, strMessage.GetBuffer(0));

		char *pOldLocale = _strdup(setlocale(LC_CTYPE, NULL));
		setlocale(LC_CTYPE, "chs");
		File.WriteString(strMessageTemp);
		setlocale(LC_CTYPE, pOldLocale);
		free(pOldLocale);
	}
	catch (...)
	{
		dwError = GetLastError();
		if (File.m_hFile != INVALID_HANDLE_VALUE)
		{
			File.Close();
		}
		ReleaseMutex(m_hMutex);
		m_strErrorMessage.Format(_T("WriteLog Exception! Exception Code : %d"), dwError);
		return FALSE;
	}

	if (File.m_hFile != INVALID_HANDLE_VALUE)
	{
		File.Close();
	}
	ReleaseMutex(m_hMutex);

	return TRUE;
}


CString CLogFile::GetLastErrorMessage(void)
{
	return m_strErrorMessage;
}
