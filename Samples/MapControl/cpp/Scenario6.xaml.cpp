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
#include "Scenario6.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::UI::Xaml;

Scenario6::Scenario6()
{
    InitializeComponent();
}

void Scenario6::pfnLink_Click(Object^ sender, RoutedEventArgs^ e)
{
    Windows::Foundation::Uri^ uri = ref new Windows::Foundation::Uri("bingmaps:?rtp=adr.Mountain%20View,%20CA~adr.San%20Francisco,%20CA&amp;mode=d&amp;trfc=1");
    LauncherOptions^ launcherOptions = ref new LauncherOptions();
    launcherOptions->TargetApplicationPackageFamilyName = "Microsoft.WindowsMaps_8wekyb3d8bbwe";
    Launcher::LaunchUriAsync(uri, launcherOptions);
}

