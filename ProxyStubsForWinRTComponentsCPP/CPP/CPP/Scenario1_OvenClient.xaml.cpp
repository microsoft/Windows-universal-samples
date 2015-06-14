//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"
#include "Scenario1_OvenClient.xaml.h"
#include "MainPage.xaml.h"
#include "..\Server\Microsoft.SDKSamples.Kitchen.h"

using namespace SDKSample;
using namespace SDKSample::ProxyStubsForWinRTComponents;

using namespace Microsoft::SDKSamples::Kitchen;
using namespace Windows::Foundation;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

OvenClient::OvenClient()
{
    InitializeComponent();
}

void SDKSample::ProxyStubsForWinRTComponents::OvenClient::Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Dimensions dimensions;
    dimensions.Width = 2;
    dimensions.Height = 2;
    dimensions.Depth = 2;

    // Component Creation
    Oven^ myOven = ref new Oven(dimensions);

    // Getters and setters are accessed using property syntax
    OvenClientOutput->Text += "Oven volume is: " + (myOven->Volume).ToString() + "\n";

    // Registering event listeners
    myOven->BreadBaked += ref new TypedEventHandler<Oven^, Bread^>(this, &OvenClient::BreadCompletedHandler1);
    myOven->BreadBaked += ref new TypedEventHandler<Oven^, Bread^>(this, &OvenClient::BreadCompletedHandler2);

    // C++ projections expose the tokens used to register and remove event handlers
    auto eventRegistrationToken = (myOven->BreadBaked += ref new TypedEventHandler<Oven^, Bread^>(this, &OvenClient::BreadCompletedHandler3));
    myOven->BreadBaked -= eventRegistrationToken;

    // Bake a loaf of bread. This will trigger the BreadBaked event.
    myOven->BakeBread("Sourdough");

    // Trigger the event again with a different preheat time
    myOven->ConfigurePreheatTemperature(OvenTemperature::High);
    myOven->BakeBread("Wheat");
}

void OvenClient::BreadCompletedHandler1(Oven^ oven, Bread^ bread)
{
    auto updateOutputText = [this, oven, bread]() -> void
    {
        OvenClientOutput->Text += "Event Handler 1: Invoked\n";
        OvenClientOutput->Text += "Event Handler 1: Oven volume is: " + oven->Volume.ToString() + "\n";
        OvenClientOutput->Text += "Event Handler 1: Bread flavor is: " + bread->Flavor + "\n";
    };

    if (OvenClientOutput->Dispatcher->HasThreadAccess)
    {
        // If the current thread is the UI thread then execute the lambda.
        updateOutputText();
    }
    else
    {
        // If the current thread is not the UI thread use the dispatcher to execute the lambda on the UI thread.
        OvenClientOutput->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler(updateOutputText));
    }
}

void OvenClient::BreadCompletedHandler2(Oven^ oven, Bread^ bread)
{
    auto updateOutputText = [this, oven, bread]() -> void
    {
        OvenClientOutput->Text += "Event Handler 2: Invoked\n";
    };

    if (OvenClientOutput->Dispatcher->HasThreadAccess)
    {
        // If we are running on the UI thread then just execute the lambda.
        updateOutputText();
    }
    else
    {
        // If the current thread is not the UI thread use the dispatcher to execute the lambda on the UI thread.
        OvenClientOutput->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler(updateOutputText));
    }
}

void OvenClient::BreadCompletedHandler3(Oven^ oven, Bread^ bread)
{
    // Event handler 3 was removed and will not be invoked
    auto updateOutputText = [this, oven, bread]() -> void
    {
        OvenClientOutput->Text += "Event Handler 3: Invoked\n";
    };

    if (OvenClientOutput->Dispatcher->HasThreadAccess)
    {
        // If running on the UI thread then just execute the lambda.
        updateOutputText();
    }
    else
    {
        // If the current thread is not the UI thread use the dispatcher to execute the lambda on the UI thread.
        OvenClientOutput->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler(updateOutputText));
    }
}

