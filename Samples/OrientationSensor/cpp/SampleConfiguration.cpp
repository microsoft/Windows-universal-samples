//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"
#include <sstream>

using namespace SDKTemplate;
using namespace Windows::Devices::Sensors;
using namespace Windows::UI::Xaml::Controls;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Choose orientation sensor", "SDKTemplate.Scenario0_Choose" },
    { "Data Events", "SDKTemplate.Scenario1_DataEvents" },
    { "Polling", "SDKTemplate.Scenario2_Polling" },
    { "Calibration", "SDKTemplate.Scenario3_Calibration" }
};

void MainPage::SetReadingText(TextBlock^ textBlock, OrientationSensorReading^ reading)
{
    std::wostringstream ss;
    ss.flags(std::ios::right | std::ios::fixed);
    ss.width(5);
    ss.precision(3);

    SensorQuaternion^ quaternion = reading->Quaternion;    // get a reference to the object to avoid re-creating it for each access
    ss << "Quaternion:" << std::endl;
    ss << "W: " << quaternion->W << ", X: " << quaternion->X << ", Y: " << quaternion->Y << ", Z: " << quaternion->Z << std::endl;

    ss << std::endl;

    SensorRotationMatrix^ rotationMatrix = reading->RotationMatrix;
    ss << "M11: " << rotationMatrix->M11 << ", M12: " << rotationMatrix->M12 << ", M13: " << rotationMatrix->M13 << std::endl;
    ss << "M21: " << rotationMatrix->M21 << ", M22: " << rotationMatrix->M22 << ", M23: " << rotationMatrix->M23 << std::endl;
    ss << "M31: " << rotationMatrix->M31 << ", M32: " << rotationMatrix->M32 << ", M33: " << rotationMatrix->M33 << std::endl;

    ss << std::endl;

    ss << "Yaw Accuracy:" << std::endl;
    switch (reading->YawAccuracy)
    {
    case MagnetometerAccuracy::Unknown:
        ss << "unknown";
        break;
    case MagnetometerAccuracy::Unreliable:
        ss << "unreliable";
        break;
    case MagnetometerAccuracy::Approximate:
        ss << "approximate";
        break;
    case MagnetometerAccuracy::High:
        ss << "high";
        break;
    default:
        ss << "other";
        break;
    }

    textBlock->Text = ref new Platform::String(ss.str().c_str());
}
