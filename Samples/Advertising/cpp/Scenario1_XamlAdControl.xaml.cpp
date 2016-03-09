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
#include "Scenario1_XamlAdControl.xaml.h"

using namespace SDKTemplate;

using namespace Microsoft::Advertising::WinRT::UI;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario1_XamlAdControl::Scenario1_XamlAdControl()
{
    InitializeComponent();
}

// This is an error handler for the ad control.
void Scenario1_XamlAdControl::OnErrorOccurred(Object^ sender, AdErrorEventArgs^ e)
{
    rootPage->NotifyUser("An error occurred. " + e->ErrorCode.ToString() + ": " + e->ErrorMessage, NotifyType::ErrorMessage);
}

// This is an event handler for the ad control. It's invoked when the ad is refreshed.
void Scenario1_XamlAdControl::OnAdRefreshed(Object^ sender, RoutedEventArgs^ e)
{
    // We increment the ad count so that the message changes at every refresh.
    adCount++;
    rootPage->NotifyUser("Advertisement #" + adCount.ToString(), NotifyType::StatusMessage);
}