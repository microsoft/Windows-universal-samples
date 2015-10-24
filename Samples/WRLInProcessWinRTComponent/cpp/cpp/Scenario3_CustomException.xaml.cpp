//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"
#include "Scenario3_CustomException.xaml.h"
#include "MainPage.xaml.h"
#include "..\Server\Microsoft.SDKSamples.Kitchen.h"

using namespace SDKTemplate::WRLInProcessWinRTComponent;

using namespace Microsoft::SDKSamples::Kitchen;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

CustomException::CustomException()
{
    InitializeComponent();
}

void CustomException::Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Component Creation
    auto myOven = ref new Oven();

    try 
    {
        // Intentionally pass an invalid value
        myOven->ConfigurePreheatTemperature(static_cast<Microsoft::SDKSamples::Kitchen::OvenTemperature>(5));
    }
    catch (Platform::InvalidArgumentException^ e)
    {
        CustomExceptionOutput->Text += L"Exception caught. Please attach a debugger and enable first chance native exceptions to view exception details.\n";
    }
}
