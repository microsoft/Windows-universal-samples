// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
	{ "Connecting To Device", "CustomHidDeviceAccess.DeviceConnect" },
	{ "Feature Reports", "CustomHidDeviceAccess.FeatureReports" },
	{ "Input Report Events", "CustomHidDeviceAccess.InputReportEvents" },
	{ "Input and Output Reports", "CustomHidDeviceAccess.InputOutputReports" }
};
