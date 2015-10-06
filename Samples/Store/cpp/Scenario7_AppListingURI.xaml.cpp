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
#include "Scenario7_AppListingURI.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::UI::Xaml::Navigation;

Scenario7_AppListingURI::Scenario7_AppListingURI()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario7_AppListingURI::OnNavigatedTo(NavigationEventArgs^ e)
{
    ConfigureSimulatorAsync("app-listing-uri.xml").then([this]()
    {
    });
}

void Scenario7_AppListingURI::DisplayLink()
{
    Windows::System::Launcher::LaunchUriAsync(ref new Windows::Foundation::Uri(CurrentAppSimulator::LinkUri->AbsoluteUri));
}
