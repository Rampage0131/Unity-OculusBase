#include "OculusManager.h"
#include <iostream>

using namespace std;

static OVR::SensorDevice* pSensor;
static OVR::SensorFusion fusion;
static OVR::DeviceManager* pManager;

OculusManager::OculusManager()
{
	cout << "Constructing OculusManager" << endl;
	OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));
	pManager = OVR::DeviceManager::Create();
	if(pManager)
	{
		cout << "Created a pManager" << endl;
		OVR::DeviceEnumerator<OVR::SensorDevice> dEnum = pManager->EnumerateDevices<OVR::SensorDevice>();
		while(dEnum)
		{
			OVR::DeviceInfo info;
			if (dEnum.GetDeviceInfo(&info))
			{
				if(strstr(info.ProductName, "Tracker"))
				{
					break;
				}
			}
			dEnum.Next();
		}
		if(dEnum)
		{
			pSensor = dEnum.CreateDevice();
		}
		if (pSensor)
		{
			cout << "Created pSensor" << endl;
			bool bWaitForSuccessfulRange = true;
			float maxAcceleration = 4 * 9.81f;
			float maxRotationRate = 8 * OVR::Math<float>::Pi;
			float maxMagneticField = 1.0f;
			pSensor->SetRange(OVR::SensorRange(maxAcceleration,maxRotationRate,maxMagneticField), bWaitForSuccessfulRange);
		}
		if(dEnum)
		{
			dEnum.Clear();
		}
		if(pSensor)
		{
			fusion.AttachToSensor(pSensor);
		}
	}
}

OculusQuaternion OculusManager::GetOrientation()
{
	OVR::Quatf q = fusion.GetOrientation();
	OculusQuaternion quat = OculusQuaternion();
	quat.x = q.x;
	quat.y = q.y;
	quat.z = q.z;
	quat.w = q.w;
	return quat;
}

OculusManager::~OculusManager()
{
	cout << "Destructing Oculus Manager" << endl;
	if(pSensor)
	{
		pSensor->Release();
		pSensor = NULL;
	}
	if(pManager)
	{
		pManager->Release();
		pManager = NULL;
	}
	OVR::System::Destroy();
}