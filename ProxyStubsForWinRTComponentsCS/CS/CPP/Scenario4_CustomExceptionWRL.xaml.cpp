//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"
#include "Scenario4_CustomExceptionWRL.xaml.h"
#include "MainPage.xaml.h"
#include "..\Server\Microsoft.SDKSamples.Kitchen.h"

using namespace SDKSample;
using namespace SDKSample::ProxyStubsForWinRTComponents;

using namespace Microsoft::SDKSamples::Kitchen;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

CustomExceptionWRL::CustomExceptionWRL()
{
    InitializeComponent();
}

void SDKSample::ProxyStubsForWinRTComponents::CustomExceptionWRL::Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Microsoft::WRL::ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IOven> spMyOven;
    HRESULT hr = Windows::Foundation::ActivateInstance(Microsoft::WRL::Wrappers::HStringReference(RuntimeClass_Microsoft_SDKSamples_Kitchen_Oven).Get(), &spMyOven);
    if (SUCCEEDED(hr)) 
    {
        // Intentionally pass an invalid value
        hr = spMyOven->ConfigurePreheatTemperature((ABI::Microsoft::SDKSamples::Kitchen::OvenTemperature)5);
        if (hr == E_INVALIDARG)
        {
            CustomExceptionWRLOutput->Text += L"Error handled. Please attach a debugger and enable first chance native exceptions to view exception details.\n";
        }
    }
}
