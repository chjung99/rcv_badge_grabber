// MFCgrabberDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MFCgrabber.h"
#include "MFCgrabberDlg.h"
#include "afxdialogex.h"

#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"
#include <iostream>
#include <sstream>

#include <direct.h>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;
volatile bool g_display1;
volatile bool g_display2;
volatile bool g_capture;


FILE *FileStream;

Mat m_matImage; // �̹��� ������ ��� �ִ� ��ü.
Mat m_matImage2; // �̹��� ������ ��� �ִ� ��ü.
Mat m_matImage3; // �̹��� ������ ��� �ִ� ��ü.

#ifdef _DEBUG
#define new DEBUG_NEW
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

void CMFCgrabberDlg::CreateBitmapInfo(int w, int h, int bpp)
{

	if (m_pBitmapInfo != NULL)
	{
		delete[]m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}

	if (bpp == 8)
		m_pBitmapInfo = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	else // 24 or 32bit
		m_pBitmapInfo = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFO)];

	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = bpp;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = 0;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	if (bpp == 8)
	{
		for (int i = 0; i < 256; i++)
		{
			m_pBitmapInfo->bmiColors[i].rgbBlue = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbGreen = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbRed = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
		}
	}

	m_pBitmapInfo->bmiHeader.biWidth = w;
	m_pBitmapInfo->bmiHeader.biHeight = -h;
}
void CMFCgrabberDlg::CreateBitmapInfo2(int w, int h, int bpp)
{

	if (m_pBitmapInfo2 != NULL)
	{
		delete[]m_pBitmapInfo2;
		m_pBitmapInfo2 = NULL;
	}

	if (bpp == 8)
		m_pBitmapInfo2 = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	else // 24 or 32bit
		m_pBitmapInfo2 = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFO)];

	m_pBitmapInfo2->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo2->bmiHeader.biPlanes = 1;
	m_pBitmapInfo2->bmiHeader.biBitCount = bpp;
	m_pBitmapInfo2->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo2->bmiHeader.biSizeImage = 0;
	m_pBitmapInfo2->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo2->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo2->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo2->bmiHeader.biClrImportant = 0;

	if (bpp == 8)
	{
		for (int i = 0; i < 256; i++)
		{
			m_pBitmapInfo2->bmiColors[i].rgbBlue = (BYTE)i;
			m_pBitmapInfo2->bmiColors[i].rgbGreen = (BYTE)i;
			m_pBitmapInfo2->bmiColors[i].rgbRed = (BYTE)i;
			m_pBitmapInfo2->bmiColors[i].rgbReserved = 0;
		}
	}

	m_pBitmapInfo2->bmiHeader.biWidth = w;
	m_pBitmapInfo2->bmiHeader.biHeight = -h;
}
void CMFCgrabberDlg::DrawImage(int id)
{
	CClientDC dc(GetDlgItem(id));

	CRect rect;
	GetDlgItem(id)->GetClientRect(&rect);

	SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);
	if (id == IDC_display) {
		StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, m_matImage.cols, m_matImage.rows, m_matImage.data, m_pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	}
	else if (id == IDC_tdisplay) {
		StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, m_matImage2.cols, m_matImage2.rows, m_matImage2.data, m_pBitmapInfo2, DIB_RGB_COLORS, SRCCOPY);

	}
}
UINT CMFCgrabberDlg::GrabLoop2(void) {
	
	SystemPtr system = System::GetInstance();

	CameraList camList = system->GetCameras();

	const unsigned int numCameras = camList.GetSize();

	CameraPtr pCam = nullptr;

	pCam = camList.GetByIndex(1);

	INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
	pCam->Init();

	pCam->BeginAcquisition();
	int imageCnt = 0;

	gcstring deviceSerialNumber("");
	CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");

	deviceSerialNumber = ptrStringSerial->GetValue();
	while (1) {
		if (g_display2 == true) {
			
			
			ImagePtr pResultImage = pCam->GetNextImage(1000);
			//ImagePtr convertedImage = pResultImage->Convert(PixelFormat_BGR8, NEAREST_NEIGHBOR);
			ImagePtr convertedImage = pResultImage->Convert(PixelFormat_Mono8, NEAREST_NEIGHBOR);
			unsigned int XPadding = convertedImage->GetXPadding();
			unsigned int YPadding = convertedImage->GetYPadding();
			unsigned int rowsize = convertedImage->GetWidth();
			unsigned int colsize = convertedImage->GetHeight();
			
			//image data contains padding. When allocating Mat container size, you need to account for the X,Y image data padding. 
			
			m_matImage2 = cv::Mat(colsize + YPadding, rowsize + XPadding, CV_8UC1, convertedImage->GetData(), convertedImage->GetStride());
			
			//m_matImage3 = m_matImage2.clone();
			
			
			
			if (g_capture == true) {
			
				const size_t width = pResultImage->GetWidth();

				const size_t height = pResultImage->GetHeight();
				
			
				
				fwrite(m_matImage.data, 1, 1280 *960 * 3, FileStream);
				
				fwrite(m_matImage2.data, 1, width*height, FileStream);
				
			}
		

			CreateBitmapInfo2(m_matImage2.cols, m_matImage2.rows, m_matImage2.channels() * 8);
			DrawImage(IDC_tdisplay);
			
		}
		else {


			pCam->EndAcquisition();
			break;
		}

	}
	
	return 0;
}
UINT CMFCgrabberDlg::GrabLoop(void) {


	SystemPtr system = System::GetInstance();

	CameraList camList = system->GetCameras();

	const unsigned int numCameras = camList.GetSize();

	CameraPtr pCam = nullptr;
	

	pCam = camList.GetByIndex(0);
	

	INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
	
	
	pCam->Init();
	
	
	pCam->BeginAcquisition();
	
	/*
	int imageCnt = 0;
	
	gcstring deviceSerialNumber("");
	CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");

	deviceSerialNumber = ptrStringSerial->GetValue();
	*/
	while (1) {
		if (g_display1 == true) {
			
			ImagePtr pResultImage = pCam->GetNextImage(1000);
			
			
			ImagePtr convertedImage = pResultImage->Convert(PixelFormat_BGR8, NEAREST_NEIGHBOR);
			

			unsigned int XPadding = convertedImage->GetXPadding();
			unsigned int YPadding = convertedImage->GetYPadding();
			unsigned int rowsize = convertedImage->GetWidth();
			unsigned int colsize = convertedImage->GetHeight();

			
			
			m_matImage = cv::Mat(colsize + YPadding, rowsize + XPadding, CV_8UC3, convertedImage->GetData(), convertedImage->GetStride());
			
			/*

			if (g_capture == true) {

				const size_t width = pResultImage->GetWidth();
				const size_t height = pResultImage->GetHeight();

				
				fwrite(m_matImage2.data, 1, width2*height2, FileStream);
				fwrite(m_matImage.data, 1, width*height*3, FileStream);
				
				//fwrite(m_matImage3.data, 1, 320*256, FileStream);
				
			}
			*/
			CreateBitmapInfo(m_matImage.cols, m_matImage.rows, m_matImage.channels() * 8);
			DrawImage(IDC_display);
			
			
		}
		else {
			pCam->EndAcquisition();
			break;
		}



	}
	return 0;
	
}
UINT ThreadProc(LPVOID IParam) {
	return ((CMFCgrabberDlg*)IParam)->GrabLoop();
}

UINT ThreadProc2(LPVOID IParam2) {
	
	return ((CMFCgrabberDlg*)IParam2)->GrabLoop2();
}
// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCgrabberDlg ��ȭ ����



CMFCgrabberDlg::CMFCgrabberDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCGRABBER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCgrabberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_display, m_display);
	
}

BEGIN_MESSAGE_MAP(CMFCgrabberDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_STN_CLICKED(IDC_display, &CMFCgrabberDlg::OnStnClickeddisplay)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_load, &CMFCgrabberDlg::OnBnClickedload)
	ON_STN_CLICKED(IDC_tdisplay, &CMFCgrabberDlg::OnStnClickedtdisplay)
	ON_BN_CLICKED(IDC_tload, &CMFCgrabberDlg::OnBnClickedtload)
	ON_BN_CLICKED(IDC_stop, &CMFCgrabberDlg::OnBnClickedstop)
	
	ON_BN_CLICKED(IDC_sync, &CMFCgrabberDlg::OnBnClickedsync)
	
	ON_BN_CLICKED(IDC_stop2, &CMFCgrabberDlg::OnBnClickedstop2)
	ON_BN_CLICKED(IDC_capture, &CMFCgrabberDlg::OnBnClickedcapture)
	ON_BN_CLICKED(IDC_stoprec, &CMFCgrabberDlg::OnBnClickedstoprec)
	ON_BN_CLICKED(IDC_decode, &CMFCgrabberDlg::OnBnClickeddecode)
END_MESSAGE_MAP()


// CMFCgrabberDlg �޽��� ó����

BOOL CMFCgrabberDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	

	
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CMFCgrabberDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMFCgrabberDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.
		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMFCgrabberDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCgrabberDlg::OnStnClickeddisplay()
{
	
	
	
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCgrabberDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCgrabberDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	
	
	

	CDialogEx::OnTimer(nIDEvent);
}


void CMFCgrabberDlg::OnBnClickedload()
{

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	g_display1 = true;
	g_capture = false;
	
	CWinThread*pThread1=AfxBeginThread(ThreadProc, this, THREAD_PRIORITY_ABOVE_NORMAL);
	
	
	
}


void CMFCgrabberDlg::OnStnClickedtdisplay()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCgrabberDlg::OnBnClickedtload()
{
	g_display2 = true;
	g_capture = false;
	
	CWinThread*pThread2=AfxBeginThread(ThreadProc2, this, THREAD_PRIORITY_ABOVE_NORMAL);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCgrabberDlg::OnBnClickedstop()
{
	
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	g_display1 = false;
}



void CMFCgrabberDlg::OnBnClickedsync()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SystemPtr system = System::GetInstance();

	CameraList camList = system->GetCameras();

	const unsigned int numCameras = camList.GetSize();

	CameraPtr pCam = nullptr;
	pCam = camList.GetByIndex(0);
	INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
	pCam->Init();
	INodeMap& nodeMap = pCam->GetNodeMap();
	
	CEnumerationPtr triggerMode = nodeMap.GetNode("TriggerMode");

	triggerMode->SetIntValue(triggerMode->GetEntryByName("Off")->GetValue());
	
	CEnumerationPtr exposureAuto = nodeMap.GetNode("ExposureAuto");
	exposureAuto->SetIntValue(exposureAuto->GetEntryByName("Off")->GetValue());
	
	CEnumerationPtr exposureMode = nodeMap.GetNode("ExposureMode");
	exposureMode->SetIntValue(exposureMode->GetEntryByName("Timed")->GetValue());

	CFloatPtr exposureTime = nodeMap.GetNode("ExposureTime");
	exposureTime->SetValue(1000);
	
	triggerMode->SetIntValue(triggerMode->GetEntryByName("On")->GetValue());
	
	CEnumerationPtr triggerSource = nodeMap.GetNode("TriggerSource");
	triggerSource->SetIntValue(triggerSource->GetEntryByName("Line3")->GetValue());
	
	CEnumerationPtr triggerActivation = nodeMap.GetNode("TriggerActivation");
	triggerActivation->SetIntValue(triggerActivation->GetEntryByName("RisingEdge")->GetValue());
	
	pCam = camList.GetByIndex(1);
	nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
	pCam->Init();
	INodeMap&nodeMap2 = pCam->GetNodeMap();
	
	CEnumerationPtr syncMode = nodeMap2.GetNode("SyncMode");
	syncMode->SetIntValue(syncMode->GetEntryByName("SelfSyncMaster")->GetValue());

}




void CMFCgrabberDlg::OnBnClickedstop2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	g_display2 = false;
}


void CMFCgrabberDlg::OnBnClickedcapture()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString mPathname;
	mPathname = "2thread.bin";
	FileStream = _tfopen(mPathname, _T("wb"));

	g_capture = true;
}


void CMFCgrabberDlg::OnBnClickedstoprec()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	g_capture = false;
}


void CMFCgrabberDlg::OnBnClickeddecode()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	cv::Mat Img[2];
	
	FILE *decorde;
	string TestPath;
	decorde = fopen("2thread.bin", "rb");
	int RgbCols = 960;
	int RgbRows = 1280;
	int RgbLen = RgbCols * RgbRows * 3;
	int ThrCols = 256;
	int ThrRows = 320;
	Img[0] = cv::Mat(RgbCols, RgbRows, CV_8UC3);
	Img[1] = cv::Mat(ThrCols, ThrRows, CV_8UC1);
	char strFolderPath1[]= { "./2thread_decode1" };
	char strFolderPath2[] = { "./2thread_decode2" };
	int nr = mkdir(strFolderPath1);
	nr = mkdir(strFolderPath2);
	int cnt1 = 0;
	int cnt2 = 0;
	while (1) {
		
		fread(Img[0].data, sizeof(uchar), RgbLen, decorde);
		TestPath = format("%s/RGB1_%d.png", strFolderPath1, cnt1++);
		imwrite(TestPath, Img[0]);

		TestPath = format("%s/Thermal1_%d.png", strFolderPath2, cnt2++);
		fread(Img[1].data, sizeof(uchar), ThrCols * ThrRows, decorde);

		imwrite(TestPath, Img[1]);
		
		if (feof(decorde)) break;
	}
	fclose(decorde);
}
