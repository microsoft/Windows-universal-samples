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
#include "TripDetails.xaml.h"
#include "View\TripListView.xaml.h"
#include "ViewModel\TripVoiceCommand.h"

using namespace AdventureWorks;

using namespace AdventureWorks_Shared;
using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

TripDetails::TripDetails()
{
    InitializeComponent();
}

void TripDetails::Footer_Click(Object^ sender, RoutedEventArgs^ args)
{
    create_task(Windows::System::Launcher::LaunchUriAsync(ref new Uri(((HyperlinkButton^)sender)->Tag->ToString())));
}

/// <summary>
/// Invoked when navigating to view details about an individual trip, or create a new trip.
/// </summary>
/// <param name="e">Details about the trip to display, or null for a new trip.</param>
void TripDetails::OnNavigatedTo(NavigationEventArgs^ e)
{
    this->defaultViewModel = (TripViewModel^)this->DataContext;
    if (e->Parameter != nullptr)
    {
        Trip^ trip = dynamic_cast<Trip^>(e->Parameter);
        if (trip != nullptr)
        {
            this->DefaultViewModel->ShowTrip(trip);
            return;
        }

        TripVoiceCommand^ voiceCommand = dynamic_cast<TripVoiceCommand^>(e->Parameter);
        if (voiceCommand != nullptr)
        {
            DefaultViewModel->LoadTripFromStore(voiceCommand->Destination);
        }
    }
    else
    {
        DefaultViewModel->NewTrip();
    }
}

TripViewModel^ TripDetails::DefaultViewModel::get()
{
    return this->defaultViewModel;
}