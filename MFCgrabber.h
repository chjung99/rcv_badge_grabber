
// MFCgrabber.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMFCgrabberApp:
// �� Ŭ������ ������ ���ؼ��� MFCgrabber.cpp�� �����Ͻʽÿ�.
//

class CMFCgrabberApp : public CWinApp
{
public:
	CMFCgrabberApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CMFCgrabberApp theApp;