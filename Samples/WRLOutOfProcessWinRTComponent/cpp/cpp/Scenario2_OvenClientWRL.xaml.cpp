//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"
#include "Scenario2_OvenClientWRL.xaml.h"
#include "MainPage.xaml.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Microsoft::SDKSamples::Kitchen;
using namespace SDKTemplate::WRLOutOfProcessWinRTComponent;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation;

OvenClientWRL::OvenClientWRL()
{
    InitializeComponent();
}

void OvenClientWRL::Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        HRESULT hr = S_OK;

        if (!_myOven)
        {
            ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IOvenFactory> ovenFactory;
            // Component Creation: Get the activation factory
            hr = GetActivationFactory(HStringReference(RuntimeClass_Microsoft_SDKSamples_Kitchen_Oven).Get(), &ovenFactory);
            if (FAILED(hr)) throw ref new Platform::COMException(hr);

            ABI::Microsoft::SDKSamples::Kitchen::Dimensions dimensions;
            dimensions.Width = 2;
            dimensions.Height = 2;
            dimensions.Depth = 2;

            // Component Creation: Call the factory method to produce an oven object
            hr = ovenFactory->CreateOven(dimensions, &_myOven);
            if (FAILED(hr)) throw ref new Platform::COMException(hr);
        }

        ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IAppliance> myAppliance;
        hr = _myOven.As(&myAppliance);
        if (FAILED(hr)) throw ref new Platform::COMException(hr);

        // Getters and setters are accessed using the get_ or put_ methods
        double volume;
        hr = myAppliance->get_Volume(&volume);
        if (FAILED(hr)) throw ref new Platform::COMException(hr);

        OvenClientWRLOutput->Text += "Oven volume is: " + volume.ToString() + "\n";

        // Declare handlers for event callbacks
        auto handler1 = Callback<ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::SDKSamples::Kitchen::Oven*, ABI::Microsoft::SDKSamples::Kitchen::BreadBakedEventArgs*>>(
            [this](ABI::Microsoft::SDKSamples::Kitchen::IOven* pOven, ABI::Microsoft::SDKSamples::Kitchen::IBreakBakedEventArgs* args) -> HRESULT
        {
            HRESULT hr = S_OK;
            try
            {
                // To append to the output text a C++\CX call is made. Catch any Windows Runtime exceptions and turn them into HRESULTS.
                OvenClientWRLOutput->Text += "Event Handler 1: Invoked\n";

                ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IAppliance> appliance;
                hr = pOven->QueryInterface(IID_PPV_ARGS(&appliance));
                if (FAILED(hr)) throw ref new Platform::COMException(hr);

                double volume;
                hr = appliance->get_Volume(&volume);
                if (FAILED(hr)) throw ref new Platform::COMException(hr);

                OvenClientWRLOutput->Text += "Event Handler 1: Oven volume is: " + volume.ToString() + "\n";

                ComPtr<ABI::Microsoft::SDKSamples::Kitchen::IBread> bread;
                hr = args->get_Bread(&bread);
                if (FAILED(hr)) throw ref new Platform::COMException(hr);

                HString flavor;
                hr = bread->get_Flavor(flavor.GetAddressOf());
                if (FAILED(hr)) throw ref new Platform::COMException(hr);

                OvenClientWRLOutput->Text += "Event Handler 1: Bread flavor is: " + ref new Platform::String(flavor.Get()) + "\n";
            }
            catch (Platform::COMException^ e)
            {
                hr = e->HResult;
            }
            return hr;
        });

        auto handler2 = Callback<ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::SDKSamples::Kitchen::Oven*, ABI::Microsoft::SDKSamples::Kitchen::BreadBakedEventArgs*>>(
            [this](ABI::Microsoft::SDKSamples::Kitchen::IOven* oven, ABI::Microsoft::SDKSamples::Kitchen::IBreakBakedEventArgs* args) -> HRESULT
        {
            HRESULT hr = S_OK;
            try
            {
                // To append to the output text a C++\CX call is made. Catch any Windows Runtime exceptions and turn them into HRESULTS.
                OvenClientWRLOutput->Text += "Event Handler 2: Invoked\n";
            }
            catch (Platform::COMException^ e)
            {
                hr = e->HResult;
            }
            return hr;
        });

        auto handler3 = Callback<ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::SDKSamples::Kitchen::Oven*, ABI::Microsoft::SDKSamples::Kitchen::BreadBakedEventArgs*>>(
            [this](ABI::Microsoft::SDKSamples::Kitchen::IOven* oven, ABI::Microsoft::SDKSamples::Kitchen::IBreakBakedEventArgs* args) -> HRESULT
        {
            // Event handler 3 was removed and will not be invoked
            HRESULT hr = S_OK;
            try
            {
                // To append to the output text a C++\CX call is made. Catch any Windows Runtime exceptions and turn them into HRESULTS.
                OvenClientWRLOutput->Text += "Event Handler 3: Invoked\n";
            }
            catch (Platform::COMException^ e)
            {
                hr = e->HResult;
            }
            return hr;
        });

        // Registering event listeners
        ::EventRegistrationToken token1;
        ::EventRegistrationToken token2;
        ::EventRegistrationToken token3;
        hr = _myOven->add_BreadBaked(handler1.Get(), &token1);
        if (FAILED(hr)) throw ref new Platform::COMException(hr);
        hr = _myOven->add_BreadBaked(handler2.Get(), &token2);
        if (FAILED(hr)) throw ref new Platform::COMException(hr);
        hr = _myOven->add_BreadBaked(handler3.Get(), &token3);
        if (FAILED(hr)) throw ref new Platform::COMException(hr);

        // Unregister from an event using the registration token
        hr = _myOven->remove_BreadBaked(token3);
        if (FAILED(hr)) throw ref new Platform::COMException(hr);

        // Bake a loaf of bread. This will trigger the BreadBaked event.
        hr = _myOven->BakeBread(HStringReference(L"Sourdough").Get());
        if (FAILED(hr)) throw ref new Platform::COMException(hr);

        // Trigger the event again with a different preheat time
        hr = _myOven->ConfigurePreheatTemperature(ABI::Microsoft::SDKSamples::Kitchen::OvenTemperature_High);
        if (FAILED(hr)) throw ref new Platform::COMException(hr);

        hr = _myOven->BakeBread(HStringReference(L"Wheat").Get());
        if (FAILED(hr)) throw ref new Platform::COMException(hr);
    }
    catch (Platform::COMException^ e)
    {
        e->HResult;
    }
}

