//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"
#include "Scenario2_OvenClientWRL.xaml.h"
#include "MainPage.xaml.h"
#include "..\Server\Microsoft.SDKSamples.Kitchen.h"

using namespace SDKSample;
using namespace SDKSample::ProxyStubsForWinRTComponents;

using namespace Microsoft::SDKSamples::Kitchen;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

OvenClientWRL::OvenClientWRL()
{
    InitializeComponent();
}

void SDKSample::ProxyStubsForWinRTComponents::OvenClientWRL::Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Microsoft::WRL::ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IOven> spMyOven;
    Microsoft::WRL::ComPtr<ABI::Microsoft::SDKSamples::Kitchen::__IOvenFactory> spMyOvenFactory;

    // Component Creation: Get the activation factory
    HRESULT hr = Windows::Foundation::GetActivationFactory(Microsoft::WRL::Wrappers::HStringReference(RuntimeClass_Microsoft_SDKSamples_Kitchen_Oven).Get(), &spMyOvenFactory);
    if (SUCCEEDED(hr))
    {
        ABI::Microsoft::SDKSamples::Kitchen::Dimensions dimensions;
        dimensions.Width = 2;
        dimensions.Height = 2;
        dimensions.Depth = 2;

        // Component Creation: Call the factory method to produce an oven object
        hr = spMyOvenFactory->CreateOven(dimensions, &spMyOven);
    }
    if (SUCCEEDED(hr))
    {
        Microsoft::WRL::ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IAppliance> spMyAppliance; 
        hr = spMyOven.As(&spMyAppliance);
        if (SUCCEEDED(hr))
        {
            // Getters and setters are accessed using the get_ or put_ methods
            double volume;
            hr = spMyAppliance->get_Volume(&volume);
            if (SUCCEEDED(hr))
            {
                // To append to the output text a C++\CX call is made. This call can throw an exception.
                // Catch any Windows Rutnime exceptions and turn them back into HRESULTS
                try
                {
                    OvenClientWRLOutput->Text += "Oven volume is: " + volume.ToString() + "\n";
                }
                catch (Platform::COMException^ e)
                {
                    hr = e->HResult;
                }
            }
        }

        // Declare handlers for event callbacks
        auto spHandler1 = Microsoft::WRL::Callback<ABI::Windows::Foundation::ITypedEventHandler<
            ABI::Microsoft::SDKSamples::Kitchen::Oven*,
            ABI::Microsoft::SDKSamples::Kitchen::Bread*>>(
            [this](ABI::Microsoft::SDKSamples::Kitchen::IOven *pOven, ABI::Microsoft::SDKSamples::Kitchen::IBread *pBread)->HRESULT
        {
            HRESULT hr = S_OK;
            Microsoft::WRL::ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IOven> spOven(pOven);
            Microsoft::WRL::ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IBread> spBread(pBread);
            Microsoft::WRL::ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IAppliance> spAppliance;

            try
            {
                // To append to the output text a C++\CX call is made. Catch any Windows Rutnime exceptions and turn them into HRESULTS.
                OvenClientWRLOutput->Text += "Event Handler 1: Invoked\n";
            }
            catch (Platform::COMException^ e)
            {
                hr = e->HResult;
            }

            if (SUCCEEDED(hr))
            {
                hr = spOven.As(&spAppliance);
            }
            if (SUCCEEDED(hr)) 
            {
                double volume;
                hr = spAppliance->get_Volume(&volume);
                if (SUCCEEDED(hr))
                {
                    try
                    {
                        // To append to the output text a C++\CX call is made. Catch any Windows Rutnime exceptions and turn them into HRESULTS.
                        OvenClientWRLOutput->Text += "Event Handler 1: Oven volume is: " + volume.ToString() + "\n";
                    }
                    catch (Platform::COMException^ e)
                    {
                        hr = e->HResult;
                    }
                }
            }
            HSTRING hstrBreadFlavor;
            hr = spBread->get_Flavor(&hstrBreadFlavor);
            if (SUCCEEDED(hr)) 
            {
                try
                {
                    // To append to the output text a C++\CX call is made. Catch any Windows Rutnime exceptions and turn them into HRESULTS.
                    OvenClientWRLOutput->Text += "Event Handler 1: Bread flavor is: " + ref new Platform::String(hstrBreadFlavor) + "\n";
                }
                catch (Platform::COMException^ e)
                {
                    hr = e->HResult;
                }
            }
            return hr; 
        });

        auto spHandler2 = Microsoft::WRL::Callback<ABI::Windows::Foundation::ITypedEventHandler<
            ABI::Microsoft::SDKSamples::Kitchen::Oven*,
            ABI::Microsoft::SDKSamples::Kitchen::Bread*>>(
            [this](ABI::Microsoft::SDKSamples::Kitchen::IOven *pCurOven, ABI::Microsoft::SDKSamples::Kitchen::IBread *pCurBread)->HRESULT
        {
            HRESULT hr = S_OK;
            try
            {
                // To append to the output text a C++\CX call is made. Catch any Windows Rutnime exceptions and turn them into HRESULTS.
                OvenClientWRLOutput->Text += "Event Handler 2: Invoked\n";
            }
            catch (Platform::COMException^ e)
            {
                hr = e->HResult;
            }
            return hr; 
        });

        auto spHandler3 = Microsoft::WRL::Callback<ABI::Windows::Foundation::ITypedEventHandler<
            ABI::Microsoft::SDKSamples::Kitchen::Oven*,
            ABI::Microsoft::SDKSamples::Kitchen::Bread*>>(
            [this](ABI::Microsoft::SDKSamples::Kitchen::IOven *pCurOven, ABI::Microsoft::SDKSamples::Kitchen::IBread *pCurBread)->HRESULT
        {
            // Event handler 3 was removed and will not be invoked
            HRESULT hr = S_OK;
            try
            {
                // To append to the output text a C++\CX call is made. Catch any Windows Rutnime exceptions and turn them into HRESULTS.
                OvenClientWRLOutput->Text += "Event Handler 3: Invoked\n";
            }
            catch (Platform::COMException^ e)
            {
                hr = e->HResult;
            }
            return hr; 
        });

        // Registering event listeners
        ::EventRegistrationToken regBread1;
        ::EventRegistrationToken regBread2;
        ::EventRegistrationToken regBread3;
        if (SUCCEEDED(hr))
        {
            hr = spMyOven->add_BreadBaked(spHandler1.Get(), &regBread1);
        }
        if (SUCCEEDED(hr))
        {
            hr = spMyOven->add_BreadBaked(spHandler2.Get(), &regBread2);
        }
        if (SUCCEEDED(hr))
        {
            hr = spMyOven->add_BreadBaked(spHandler3.Get(), &regBread3);
        }

        // Unregister from an event using the registration token
        if (SUCCEEDED(hr))
        {
            hr = spMyOven->remove_BreadBaked(regBread3);
        }

        // Bake a loaf of bread. This will trigger the BreadBaked event.
        if (SUCCEEDED(hr))
        {
            hr = spMyOven->BakeBread(Microsoft::WRL::Wrappers::HStringReference(L"Sourdough").Get());
        }

        // Trigger the event again with a different preheat time
        if (SUCCEEDED(hr))
        {
            hr = spMyOven->ConfigurePreheatTemperature(ABI::Microsoft::SDKSamples::Kitchen::OvenTemperature_High);
        }
        if (SUCCEEDED(hr))
        {
            hr = spMyOven->BakeBread(Microsoft::WRL::Wrappers::HStringReference(L"Wheat").Get());
        }
    }
}

