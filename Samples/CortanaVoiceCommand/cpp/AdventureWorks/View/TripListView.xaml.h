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

#include "View\TripListview.g.h"
#include "ViewModel\TripListViewModel.h"

namespace AdventureWorks
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class TripListView sealed
    {
    public:
        TripListView();

        property TripListViewModel^ DefaultViewModel {
            TripListViewModel^ get();
        }

    protected:
        void Footer_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void tripListBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ args);


        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        TripListViewModel^ defaultViewModel;
        bool tripsLoaded = false;
    };
}
