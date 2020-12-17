// MFCgrabberDlg.cpp : 구현 파일
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

Mat m_matImage; // 이미지 정보를 담고 있는 객체.
Mat m_matImage2; // 이미지 정보를 담고 있는 객체.
Mat m_matImage3; // 이미지 정보를 담고 있는 객체.

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
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CMFCgrabberDlg 대화 상자



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


// CMFCgrabberDlg 메시지 처리기

BOOL CMFCgrabberDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	

	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCgrabberDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCgrabberDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCgrabberDlg::OnStnClickeddisplay()
{
	
	
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMFCgrabberDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CMFCgrabberDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	
	

	CDialogEx::OnTimer(nIDEvent);
}


void CMFCgrabberDlg::OnBnClickedload()
{

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	g_display1 = true;
	g_capture = false;
	
	CWinThread*pThread1=AfxBeginThread(ThreadProc, this, THREAD_PRIORITY_ABOVE_NORMAL);
	
	
	
}


void CMFCgrabberDlg::OnStnClickedtdisplay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMFCgrabberDlg::OnBnClickedtload()
{
	g_display2 = true;
	g_capture = false;
	
	CWinThread*pThread2=AfxBeginThread(ThreadProc2, this, THREAD_PRIORITY_ABOVE_NORMAL);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMFCgrabberDlg::OnBnClickedstop()
{
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	g_display1 = false;
}



void CMFCgrabberDlg::OnBnClickedsync()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	g_display2 = false;
}


void CMFCgrabberDlg::OnBnClickedcapture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString mPathname;
	mPathname = "2thread.bin";
	FileStream = _tfopen(mPathname, _T("wb"));

	g_capture = true;
}


void CMFCgrabberDlg::OnBnClickedstoprec()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	g_capture = false;
}


void CMFCgrabberDlg::OnBnClickeddecode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
