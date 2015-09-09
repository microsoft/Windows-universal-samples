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

#include "View\TripDetails.g.h"
#include "ViewModel\TripViewModel.h"
#include "Common\CalendarConverter.h"
#include "Common\BoolToVisibilityConverter.h"

namespace AdventureWorks
{
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class TripDetails sealed
    {
    public:
        TripDetails();

        property TripViewModel^ DefaultViewModel
        {
            TripViewModel^ get();
        }

    protected:
        void Footer_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        TripViewModel^ defaultViewModel;
    };
}
