//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"
#include "Scenario1_OvenClient.xaml.h"
#include "MainPage.xaml.h"
#include "..\Server\Microsoft.SDKSamples.Kitchen.h"

using namespace Microsoft::SDKSamples::Kitchen;
using namespace SDKTemplate::WRLOutOfProcessWinRTComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

OvenClient::OvenClient()
{
    InitializeComponent();
}

void OvenClient::Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Dimensions dimensions;
    dimensions.Width = 2;
    dimensions.Height = 2;
    dimensions.Depth = 2;

    // Component Creation
    if (!_myOven)
    {
        _myOven = ref new Oven(dimensions);
    }

    // Getters and setters are accessed using property syntax
    OvenClientOutput->Text += "Oven volume is: " + _myOven->Volume.ToString() + "\n";

    // Registering event listeners
    _myOven->BreadBaked += ref new TypedEventHandler<Oven^, BreadBakedEventArgs^>(this, &OvenClient::BreadCompletedHandler1);
    _myOven->BreadBaked += ref new TypedEventHandler<Oven^, BreadBakedEventArgs^>(this, &OvenClient::BreadCompletedHandler2);

    // C++ projections expose the tokens used to register and remove event handlers
    auto eventRegistrationToken = (_myOven->BreadBaked += ref new TypedEventHandler<Oven^, BreadBakedEventArgs^>(this, &OvenClient::BreadCompletedHandler3));
    _myOven->BreadBaked -= eventRegistrationToken;

    // Bake a loaf of bread. This will trigger the BreadBaked event.
    _myOven->BakeBread("Sourdough");

    // Trigger the event again with a different preheat time
    _myOven->ConfigurePreheatTemperature(OvenTemperature::High);
    _myOven->BakeBread("Wheat");
}

void OvenClient::BreadCompletedHandler1(Oven^ oven, BreadBakedEventArgs^ args)
{
    // Event comes in on a background thread, need to use dispatcher to get to the UI thread.
    OvenClientOutput->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, 
        ref new DispatchedHandler([this, oven, args]() -> void
    {
        OvenClientOutput->Text += "Event Handler 1: Invoked\n";
        OvenClientOutput->Text += "Event Handler 1: Oven volume is: " + oven->Volume.ToString() + "\n";
        OvenClientOutput->Text += "Event Handler 1: Bread flavor is: " + args->Bread->Flavor + "\n";
    }));
}

void OvenClient::BreadCompletedHandler2(Oven^ oven, BreadBakedEventArgs^ args)
{
    OvenClientOutput->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, 
        ref new DispatchedHandler([this, oven, args]() -> void
    {
        OvenClientOutput->Text += "Event Handler 2: Invoked\n";
    }));
}

void OvenClient::BreadCompletedHandler3(Oven^ oven, BreadBakedEventArgs^ args)
{
    // Event handler 3 was removed and will not be invoked
    OvenClientOutput->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, 
        ref new DispatchedHandler([this, oven, args]() -> void
    {
        OvenClientOutput->Text += "Event Handler 3: Invoked\n";
    }));
}

