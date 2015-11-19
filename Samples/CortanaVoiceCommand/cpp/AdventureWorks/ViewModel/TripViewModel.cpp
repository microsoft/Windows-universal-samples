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
#include "TripViewModel.h"
#include "Common\RelayCommand.h"
#include "View\TripListView.xaml.h"

using namespace AdventureWorks;

using namespace Concurrency;
using namespace AdventureWorks_Shared;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::VoiceCommands;
using namespace Windows::Globalization;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

/// <summary> Handle initialization of command data binding </summary>
TripViewModel::TripViewModel(TripStore^ store)
{
    this->store = store;
    saveTripCommand = ref new RelayCommand(
        [this](Object^ parameter) {return true; },
        [this](Object^ parameter) {this->SaveTrip(); });

    deleteTripCommand = ref new RelayCommand(
        [this](Object^ parameter) {return true; },
        [this](Object^ parameter) {this->DeleteTrip(); });

}

AdventureWorks_Shared::Trip^ TripViewModel::Trip::get()
{
    return this->trip;
}

void TripViewModel::Trip::set(AdventureWorks_Shared::Trip^ value)
{
    this->trip = value;
    NotifyPropertyChanged("Trip");
}

String^ TripViewModel::DestinationValidationError::get()
{
    return this->destinationValidationError;
}

void TripViewModel::setDestinationValidation(String^ value)
{
    this->destinationValidationError = value;
    NotifyPropertyChanged("DestinationValidationError");
}

bool TripViewModel::ShowDestinationValidation::get()
{
    return this->showDestinationValidation;
}

void TripViewModel::setShowDestinationValidation(bool value)
{
    this->showDestinationValidation = value;
    NotifyPropertyChanged("ShowDestinationValidation");
}

bool TripViewModel::ShowDelete::get()
{
    return this->showDelete;
}

void TripViewModel::setShowDelete(bool value)
{
    this->showDelete = value;
    NotifyPropertyChanged("ShowDelete");
}

ICommand^ TripViewModel::SaveTripCommand::get()
{
    return this->saveTripCommand;
}

ICommand^ TripViewModel::DeleteTripCommand::get()
{
    return this->deleteTripCommand;
}

/// <summary> 
/// Load trip data (if required), and then search the store for the specified trip.
/// Does not disambiguate, just displays the first match, for simplicity in this sample.
/// Navigates back to the main page if it can't be found (covers the case where a trip was removed
/// but the phrase list was not updated.)
/// <summary>
/// <param name="destination">The destination to search for</param>
void TripViewModel::LoadTripFromStore(String^ destination)
{
    create_task(store->LoadTripsAsync()).then([this, destination]() {

        for (unsigned int i = 0; i < store->Trips->Size; i++)
        {
            AdventureWorks_Shared::Trip^ t = store->Trips->GetAt(i);
            if (t->Destination == destination)
            {
                this->ShowTrip(t);
                return;
            }
        }

        String^ errorMsg = ref new String(L"Sorry, couldn't find a trip with Destination ") + destination;
        auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);
        rootFrame->Navigate(TripListView::typeid, errorMsg);
    });
}

/// <summary>Set the data-bound trip (and show delete button)</summary>
/// <param name="trip">The trip to show</param>
void TripViewModel::ShowTrip(AdventureWorks_Shared::Trip^ trip)
{
    setShowDelete(true);
    this->Trip = trip;
}

/// <summary>Create a new trip, hide the delete button.</summary>
void TripViewModel::NewTrip()
{
    setShowDelete(false);
    Trip = ref new AdventureWorks_Shared::Trip();
}

///<summary> Delete the currently displayed trip from the trip store,
/// trigger update of phrase list.</summary>
void TripViewModel::DeleteTrip()
{
    store->DeleteTripAsync(this->Trip);

    this->UpdateDestinationPhraseList();
    auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);
    rootFrame->Navigate(TripListView::typeid);

}

/// <summary> Update the phrase list, issued on initial startup of the app, and 
/// whenever a trip is modified and saved.</summary>
void TripViewModel::UpdateDestinationPhraseList()
{
    VoiceCommandDefinition^ commandDefinitions;
	auto languages = Windows::Globalization::ApplicationLanguages::Languages;
	String^ countryCode = L"en-US";
	if (languages->Size > 0)
	{
		// Languages is ordered by the preference order that's specified in settings.
		countryCode = languages->GetAt(0);
	}

    if (VoiceCommandDefinitionManager::InstalledCommandDefinitions->HasKey(L"AdventureWorksCommandSet_" + countryCode))
    {
        commandDefinitions = VoiceCommandDefinitionManager::InstalledCommandDefinitions->Lookup(L"AdventureWorksCommandSet_" + countryCode);

        Vector<String^>^ destinations = ref new Vector<String^>();
        int size = this->store->Trips->Size;
        for (int i = 0; i < size; i++)
        {
            AdventureWorks_Shared::Trip^ t = this->store->Trips->GetAt(i);
            String^ destination = t->Destination;
            unsigned int unused;
            if (!destinations->IndexOf(destination, &unused))
            {
                destinations->Append(destination);
            }
        }

        create_task(commandDefinitions->SetPhraseListAsync(L"destination", destinations))
            .then([](Concurrency::task<void> t) {
            try
            {
                t.get();
            }
            catch (Platform::Exception^ ex)
            {
                OutputDebugStringW(L"Failed to install phrase set into AdventureWorksCommandSet_en-us\n");
                OutputDebugStringW(ex->Message->Data());
            }
        });
    }
}

/// <summary> Save a trip to the store, trigger an update of the phrase list, 
/// then navigate back to the main page </summary>
void TripViewModel::SaveTrip()
{
    setShowDestinationValidation(false);
    bool valid = true;

    if ((Trip->Destination->IsEmpty()))
    {
        valid = false;
        setShowDestinationValidation(true);
        setDestinationValidation(L"Destination cannot be blank");
    }

    if (valid)
    {
        create_task(store->SaveTripAsync(this->Trip), task_continuation_context::use_current()).then(
            [this](task<void> saveTripTask)
        {
            saveTripTask.get();
            this->UpdateDestinationPhraseList();
            auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);
            rootFrame->Navigate(TripListView::typeid);
        });
    }
}
