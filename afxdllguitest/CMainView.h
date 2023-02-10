#pragma once

class Cheat;
// CMainView 对话框

class CMainView : public CDialog
{
	DECLARE_DYNAMIC(CMainView)

public:
	CMainView(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMainView();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	Cheat *d3d11;
};
