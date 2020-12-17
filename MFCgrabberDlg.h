
// MFCgrabberDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"
#include <iostream>
#include <sstream>

// CMFCgrabberDlg ��ȭ ����
class CMFCgrabberDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CMFCgrabberDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCGRABBER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickeddisplay();
	CStatic m_display;
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	Mat mat_frame;
	CImage cimage_mfc;
	afx_msg void OnBnClickedload();
public:

	BITMAPINFO *m_pBitmapInfo; // Bitmap ������ ��� �ִ� ����ü.
	BITMAPINFO *m_pBitmapInfo2; // Bitmap ������ ��� �ִ� ����ü.
	afx_msg UINT CMFCgrabberDlg::GrabLoop();
	afx_msg UINT CMFCgrabberDlg::GrabLoop2();
	void CreateBitmapInfo(int w, int h, int bpp); // Bitmap ������ �����ϴ� �Լ�.
	void CreateBitmapInfo2(int w, int h, int bpp); // Bitmap ������ �����ϴ� �Լ�.
	void DrawImage(int id); // �׸��� �۾��� �����ϴ� �Լ�.
	
	afx_msg void OnStnClickedtdisplay();
	afx_msg void OnBnClickedtload();
	afx_msg void OnBnClickedstop();
	CWinThread*pThread1;
	CWinThread*pThread2;
	CButton m_stop;
	LPVOID IParam;
	LPVOID IParam2;
	afx_msg void OnBnClickedsync();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedstop2();
	afx_msg void OnBnClickedcapture();
	afx_msg void OnBnClickedstoprec();
	afx_msg void OnBnClickeddecode();
};

UINT ThreadProc(LPVOID IParam);
UINT ThreadProc2(LPVOID IParam);
