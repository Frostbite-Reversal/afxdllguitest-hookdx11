// afxdllguitest.cpp: 定义 DLL 的初始化例程。
//

#include "pch.h"
#include "framework.h"
#include "afxdllguitest.h"
#include"CMainView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为以下项中的第一个语句:
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CafxdllguitestApp

BEGIN_MESSAGE_MAP(CafxdllguitestApp, CWinApp)
END_MESSAGE_MAP()


// CafxdllguitestApp 构造

CafxdllguitestApp::CafxdllguitestApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CafxdllguitestApp 对象



CafxdllguitestApp theApp;

HANDLE g_UIthread;
HANDLE g_FreeThread;
// CafxdllguitestApp 初始化
CMainView *MainDlg;
HINSTANCE hResOld;
void UiThread(void * lparam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	hResOld = AfxGetResourceHandle();
	AfxSetResourceHandle(theApp.m_hInstance);

	MainDlg = new CMainView();
	MainDlg->DoModal();
	
	
	//g_FreeThread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, theApp.m_hInstance, 0, 0);
	//AfxBeginThread(FreeLibrary, theApp.m_hInstance);
	g_FreeThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, theApp.m_hInstance, 0, 0);
	
	//CloseHandle(g_FreeThread);
	AfxSetResourceHandle(hResOld);
	
}
BOOL CafxdllguitestApp::InitInstance()
{
	CWinApp::InitInstance();
	
	g_UIthread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UiThread, NULL, 0, 0);
	return TRUE;
}
//SaveBitmapToFile


int CafxdllguitestApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	
	MainDlg->DestroyWindow();
	CloseHandle(g_UIthread);
	
	
	//AfxBeginThread(FreeLibrary);
	CloseHandle(g_FreeThread);
	delete MainDlg;
	MainDlg = NULL;
	return CWinApp::ExitInstance();
}
