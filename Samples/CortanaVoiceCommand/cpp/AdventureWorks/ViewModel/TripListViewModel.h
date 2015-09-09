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
    public ref class TripListViewModel sealed :
        public ViewModelBase
    {
    public:
        TripListViewModel(AdventureWorks_Shared::TripStore^ store);

        property Windows::Foundation::Collections::IObservableVector<AdventureWorks_Shared::Trip^>^ Trips
        {
            Windows::Foundation::Collections::IObservableVector<AdventureWorks_Shared::Trip^>^ get();
        }

        property AdventureWorks_Shared::Trip^ SelectedTrip
        {
            AdventureWorks_Shared::Trip^ get();
            void set(AdventureWorks_Shared::Trip^ value);
        }

        property Windows::UI::Xaml::Input::ICommand^ AddTripCommand
        {
            Windows::UI::Xaml::Input::ICommand^ get();
        }

        void LoadTrips();
        void SelectionChanged();
    private:
        Windows::UI::Xaml::Input::ICommand^ addTripCommand;
        Windows::Foundation::Collections::IObservableVector<AdventureWorks_Shared::Trip^>^ trips;
        AdventureWorks_Shared::Trip^ selectedTrip;
        AdventureWorks_Shared::TripStore^ store;

        void AddTrip();


    };
}
