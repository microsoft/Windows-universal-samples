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
#include "ViewModel\TripListViewModel.h"

#include "Common\RelayCommand.h"
#include "View\TripDetails.xaml.h"

using namespace AdventureWorks;

using namespace AdventureWorks_Shared;
using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

/// <summary>
/// Sets up list view and command binding references.
/// </summary>
/// <param name="store">Shared reference to trip store.</param>
TripListViewModel::TripListViewModel(AdventureWorks_Shared::TripStore ^ store)
{
    this->store = store;
    this->trips = store->Trips;

    addTripCommand = ref new RelayCommand(
        [this](Object^ parameter) {return true; },
        [this](Object^ parameter) {this->AddTrip(); });
}

/// <summary> Command executed when new trip button is clicked </summary>
void TripListViewModel::AddTrip()
{
    auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);
    rootFrame->Navigate(TripDetails::typeid);
}

Trip^ TripListViewModel::SelectedTrip::get()
{
    return this->selectedTrip;
}

void TripListViewModel::SelectedTrip::set(Trip^ value)
{
    this->selectedTrip = value;
    NotifyPropertyChanged("SelectedTrip");
}

ICommand^ TripListViewModel::AddTripCommand::get()
{
    return this->addTripCommand;
}

/// <summary> Triggers asynchronous initialization of trip store assets </summary>
void TripListViewModel::LoadTrips()
{
    store->LoadTrips();
}

/// <summary> Handle clicking on a different trip to navigate to that trip </summary>
void TripListViewModel::SelectionChanged()
{
    if (this->SelectedTrip != nullptr)
    {
        auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);
        rootFrame->Navigate(TripDetails::typeid, this->SelectedTrip);
    }
}

IObservableVector<AdventureWorks_Shared::Trip^>^ TripListViewModel::Trips::get()
{
    return this->trips;
}
