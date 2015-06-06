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
#include "RadioModel.h"

using namespace SDKTemplate;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::Devices::Radios;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace concurrency;

RadioModel::RadioModel(Radio^ radio, UIElement^ parent)
{
    this->radio = radio;
    this->parent = parent;
    this->radio->StateChanged += ref new TypedEventHandler<Radio^, Object^>(this, &RadioModel::Radio_StateChanged);
}

void RadioModel::NotifyPropertyChanged(Platform::String^ propertyName)
{
    PropertyChanged(this, ref new PropertyChangedEventArgs(propertyName));
}

void RadioModel::SetRadioState(bool isRadioOn)
{
    RadioState radioState = isRadioOn ? RadioState::On : RadioState::Off;
    Disable();

    auto setStateOperation = this->radio->SetStateAsync(radioState);
    create_task(setStateOperation).then([this](task<RadioAccessStatus> asyncInfo)
    {
        NotifyPropertyChanged("IsRadioOn");
        Enable();
    });
}

void RadioModel::Radio_StateChanged(Radio^ sender, Object^ args)
{
    Dispatch([this]()
    {
        NotifyPropertyChanged("IsRadioOn");
    });
}

void RadioModel::Enable()
{
    IsEnabled = true;
}

void RadioModel::Disable()
{
    IsEnabled = false;
}

// Execute the passed-in function in the UI thread
template<typename Func>
void RadioModel::Dispatch(Func function)
{
    this->parent->Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(function));
}