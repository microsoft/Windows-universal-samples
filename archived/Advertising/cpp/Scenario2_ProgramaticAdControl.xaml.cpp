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
#include "Scenario2_ProgramaticAdControl.xaml.h"

using namespace SDKTemplate;

using namespace Microsoft::Advertising::WinRT::UI;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario2_ProgramaticAdControl::Scenario2_ProgramaticAdControl()
{
    InitializeComponent();
}

void Scenario2_ProgramaticAdControl::CreateAdControl_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto button = safe_cast<Button^>(sender);
    button->IsEnabled = false;

    // Programatically create an ad control. This must be done from the UI thread.
    auto adControl = ref new AdControl();

    // Set the application id and ad unit id
    // The application id and ad unit id can be obtained from Dev Center.
    // See "Monetize with Ads" at https ://msdn.microsoft.com/en-us/library/windows/apps/mt170658.aspx
    adControl->ApplicationId = L"d25517cb-12d4-4699-8bdc-52040c712cab";
    adControl->AdUnitId = L"10043134";

    // Set the dimensions
    adControl->Width = 160;
    adControl->Height = 600;

    // Add event handlers if you want
    adControl->ErrorOccurred += ref new EventHandler<AdErrorEventArgs^>(this, &Scenario2_ProgramaticAdControl::OnErrorOccurred);
    adControl->AdRefreshed += ref new EventHandler<RoutedEventArgs^>(this, &Scenario2_ProgramaticAdControl::OnAdRefreshed);

    // Add the ad control to the page
    auto parent = safe_cast<Panel^>(button->Parent);
    parent->Children->Append(adControl);
}

// This is an error handler for the ad control.
void Scenario2_ProgramaticAdControl::OnErrorOccurred(Object^ sender, AdErrorEventArgs^ e)
{
    rootPage->NotifyUser("An error occurred. " + e->ErrorCode.ToString() + ": " + e->ErrorMessage, NotifyType::ErrorMessage);
}

// This is an event handler for the ad control. It's invoked when the ad is refreshed.
void Scenario2_ProgramaticAdControl::OnAdRefreshed(Object^ sender, RoutedEventArgs^ e)
{
    // We increment the ad count so that the message changes at every refresh.
    adCount++;
    rootPage->NotifyUser("Advertisement #" + adCount.ToString(), NotifyType::StatusMessage);
}
