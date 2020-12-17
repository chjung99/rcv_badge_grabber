#pragma once
#include "stdafx.h"

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream>


using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

class spin
{
public:
	SystemPtr system;
	CameraList camList;
	INodeMap& nodeMap;
	INodeMap& nodeMapTLDevice;
	CameraPtr pCam;
	ImagePtr pResultImage;
	ImagePtr convertedImage;
	Mat m_matImage;


	int DisableHeartbeat(INodeMap& nodeMap, INodeMap& nodeMapTLDevice);
	int AcquireImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice);
	int PrintDeviceInfo(INodeMap& nodeMap);
	int RunSingleCamera(CameraPtr pCam);
	int spinstart(int id);
};