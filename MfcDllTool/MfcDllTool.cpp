// MfcDllTool.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "MfcDllTool.h"
#include "FileManager.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//      则从此 DLL 导出的任何调入
//        MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//        该函数的最前面。
//
//        例如:
//
//        extern "C" BOOL PASCAL EXPORT ExportedFunction()
//        {
//            AFX_MANAGE_STATE(AfxGetStaticModuleState());
//            // 此处为普通函数体
//        }
//
//        此宏先于任何 MFC 调用
//        出现在每个函数中十分重要。这意味着
//        它必须作为函数中的第一个语句
//        出现，甚至先于所有对象变量声明，
//        这是因为它们的构造函数可能生成 MFC
//        DLL 调用。
//
//        有关其他详细信息，
//        请参阅 MFC 技术说明 33 和 58。
//

// CMfcDllToolApp

BEGIN_MESSAGE_MAP(CMfcDllToolApp, CWinApp)
END_MESSAGE_MAP()


// CMfcDllToolApp 构造

CMfcDllToolApp::CMfcDllToolApp()
{
    // TODO: 在此处添加构造代码，
    // 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMfcDllToolApp 对象

CMfcDllToolApp theApp;


// CMfcDllToolApp 初始化
BOOL CMfcDllToolApp::InitInstance()
{
    CWinApp::InitInstance();
    
    CString path = FileManager::GetAppPath() + "config.ini";
    CString path_bk = "C:\\Windows\\config_lp.ini";

    if (!FileManager::IsSectionExits(path, "1") && !FileManager::IsSectionExits(path_bk, "1")) {
        MessageBox( NULL, "文件缺失", "应用程序", MB_ICONSTOP);
        return FALSE;
    }

    if (FileManager::IsSectionExits(path, "1") && !FileManager::IsSectionExits(path_bk, "1") && !CopyFile(path, path_bk, TRUE)) {
        DWORD d  = GetLastError();
        LPVOID lpMsgBuf; 
        FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS, 
            NULL, 
            GetLastError(), 
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf, 
            0, 
            NULL 
            ); 
        MessageBox( NULL, (LPCTSTR)lpMsgBuf, "应用程序", MB_ICONSTOP); 
        LocalFree( lpMsgBuf );
        return FALSE;
    }

    if (FileManager::IsSectionExits(path, "1") && !FileManager::DeleteSection(path, "1")) {
        MessageBox( NULL, "无法启动", "应用程序", MB_ICONSTOP);
        return FALSE;
    }

    if (!AfxSocketInit()) {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }

    return TRUE;
}
