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
#pragma once
#include "ViewModel\ViewModelBase.h"

namespace AdventureWorks
{
    [Windows::UI::Xaml::Data::Bindable]
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class TripViewModel sealed : public ViewModelBase
    {
    public:
        TripViewModel(AdventureWorks_Shared::TripStore^ store);

        property AdventureWorks_Shared::Trip^ Trip
        {
            AdventureWorks_Shared::Trip^ get();
            void set(AdventureWorks_Shared::Trip^ value);
        }

        property Platform::String^ DestinationValidationError
        {
            Platform::String^ get();
        }

        property bool ShowDestinationValidation
        {
            bool get();
        }

        property bool ShowDelete
        {
            bool get();
        }

        property Windows::UI::Xaml::Input::ICommand^ SaveTripCommand
        {
            Windows::UI::Xaml::Input::ICommand^ get();
        }

        property Windows::UI::Xaml::Input::ICommand^ DeleteTripCommand
        {
            Windows::UI::Xaml::Input::ICommand^ get();
        }

        void LoadTripFromStore(Platform::String^ destination);
        void ShowTrip(AdventureWorks_Shared::Trip^ trip);
        void NewTrip();
        void UpdateDestinationPhraseList();

    private:
        AdventureWorks_Shared::Trip^ trip;
        AdventureWorks_Shared::TripStore^ store;

        Windows::UI::Xaml::Input::ICommand^ saveTripCommand;
        Windows::UI::Xaml::Input::ICommand^ deleteTripCommand;

        bool showDestinationValidation = true;
        Platform::String^ destinationValidationError;

        bool showDelete = false;

        void setDestinationValidation(Platform::String^ value);
        void setShowDestinationValidation(bool value);
        void setShowDelete(bool value);

        void DeleteTrip();
        void SaveTrip();

    };
}
