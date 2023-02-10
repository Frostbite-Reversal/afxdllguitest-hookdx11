// afxdllguitest.h: afxdllguitest DLL 的主标头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// 主符号


// CafxdllguitestApp
// 有关此类实现的信息，请参阅 afxdllguitest.cpp
//

class CafxdllguitestApp : public CWinApp
{
public:
	CafxdllguitestApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	//void UiThread(void * lparam);

	
};
