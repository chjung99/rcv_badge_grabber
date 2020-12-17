
// MFCgrabberDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"
#include <iostream>
#include <sstream>

// CMFCgrabberDlg 대화 상자
class CMFCgrabberDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCgrabberDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCGRABBER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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

	BITMAPINFO *m_pBitmapInfo; // Bitmap 정보를 담고 있는 구조체.
	BITMAPINFO *m_pBitmapInfo2; // Bitmap 정보를 담고 있는 구조체.
	afx_msg UINT CMFCgrabberDlg::GrabLoop();
	afx_msg UINT CMFCgrabberDlg::GrabLoop2();
	void CreateBitmapInfo(int w, int h, int bpp); // Bitmap 정보를 생성하는 함수.
	void CreateBitmapInfo2(int w, int h, int bpp); // Bitmap 정보를 생성하는 함수.
	void DrawImage(int id); // 그리는 작업을 수행하는 함수.
	
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
