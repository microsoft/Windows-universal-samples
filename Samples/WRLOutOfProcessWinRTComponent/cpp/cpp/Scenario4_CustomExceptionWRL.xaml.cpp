//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"
#include "Scenario4_CustomExceptionWRL.xaml.h"
#include "MainPage.xaml.h"
#include "..\Server\Microsoft.SDKSamples.Kitchen.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Microsoft::SDKSamples::Kitchen;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace SDKTemplate::WRLOutOfProcessWinRTComponent;

CustomExceptionWRL::CustomExceptionWRL()
{
    InitializeComponent();
}

void CustomExceptionWRL::Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IOven> myOven;
    HRESULT hr = Windows::Foundation::ActivateInstance(HStringReference(
        RuntimeClass_Microsoft_SDKSamples_Kitchen_Oven).Get(), &myOven);
    if (SUCCEEDED(hr)) 
    {
        // Intentionally pass an invalid value
        hr = myOven->ConfigurePreheatTemperature((ABI::Microsoft::SDKSamples::Kitchen::OvenTemperature)5);
        if (hr == E_INVALIDARG)
        {
            CustomExceptionWRLOutput->Text += L"Error handled. Please attach a debugger and enable first chance native exceptions to view exception details.\n";
        }
    }
}
