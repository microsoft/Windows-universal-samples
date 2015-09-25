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
#include "Trip.h"

namespace AdventureWorks_Shared
{
    [Windows::UI::Xaml::Data::Bindable]
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class TripStore sealed
    {
    public:
        TripStore();

        property Windows::Foundation::Collections::IObservableVector<Trip^>^ Trips
        {
            Windows::Foundation::Collections::IObservableVector<Trip^>^ get();
        }

        void LoadTrips();
        void LoadTripsSync();
        Windows::Foundation::IAsyncAction^ DeleteTripAsync(Trip^ trip);
        Windows::Foundation::IAsyncAction^ SaveTripAsync(Trip^ trip);
        Windows::Foundation::IAsyncAction^ LoadTripsAsync();

    private:
        Windows::Foundation::Collections::IObservableVector<Trip^>^ trips;
        void WriteTrips();
        HRESULT ReadXml(Windows::Storage::Streams::IRandomAccessStream^ randomAccessReadStream);

        Concurrency::task<void> LoadTripsTask();

        bool loaded;
    };
}

