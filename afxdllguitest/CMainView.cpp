// CMainView.cpp: 实现文件
//

#include "pch.h"
#include "afxdllguitest.h"
#include "CMainView.h"
#include "afxdialogex.h"
#include"Cheat.h"

// CMainView 对话框

IMPLEMENT_DYNAMIC(CMainView, CDialog)

CMainView::CMainView(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG, pParent)
{

}
void InitConsole()
{
	AllocConsole();
	SetConsoleTitle(TEXT("调试窗口"));
	freopen("CONOUT$", "w+t", stdout);
}
void ReleaseConsole()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	FreeConsole();
}
CMainView::~CMainView()
{
	delete d3d11;
}

void CMainView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainView, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CMainView::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CMainView::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CMainView::OnBnClickedButton2)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMainView 消息处理程序


BOOL CMainView::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	SetTimer(1, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMainView::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	static bool bconsole = false;
	if (bconsole == false)
	{
		GetDlgItem(IDC_BUTTON1)->SetWindowText(TEXT("关闭控制台"));
		InitConsole();
		bconsole = true;
	}
	else
	{
		GetDlgItem(IDC_BUTTON1)->SetWindowText(TEXT("打开控制台"));
		ReleaseConsole();
		bconsole = false;
	}
}


void CMainView::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CMainView::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	
	
}


void CMainView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
		InitConsole();
		d3d11 = new Cheat();
		KillTimer(1);
		break;
	default:
		break;
	}
	CDialog::OnTimer(nIDEvent);
}
