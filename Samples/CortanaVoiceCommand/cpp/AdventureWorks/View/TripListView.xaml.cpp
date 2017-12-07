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
#include "TripListView.xaml.h"

using namespace AdventureWorks;

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

TripListView::TripListView()
{
    InitializeComponent();
}

void TripListView::tripListBox_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ args)
{
    this->DefaultViewModel->SelectionChanged();

    // Immediately deselect the item so that the list box returns to its initial state.
    DefaultViewModel->SelectedTrip = nullptr;

}

void TripListView::Footer_Click(Platform::Object^ sender, RoutedEventArgs^ args)
{
    create_task(Windows::System::Launcher::LaunchUriAsync(ref new Uri(((HyperlinkButton^)sender)->Tag->ToString())));
}

TripListViewModel^ TripListView::DefaultViewModel::get()
{
    return this->defaultViewModel;
}

/// <summary>
/// Handles initial launch, and errors if a trip detail view can't find the trip requested.
/// </summary>
/// <param name="args">Details about any errors that may have occurred, or null.</param>
void TripListView::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ args)
{
    defaultViewModel = (TripListViewModel^)this->DataContext;
    if (!tripsLoaded)
    {
        // On initial launch, load the trips.
        defaultViewModel->LoadTrips();
        tripsLoaded = true;
    }

    if (args->Parameter != nullptr)
    {
        String^ errorMsg = dynamic_cast<String^>(args->Parameter);
        if (errorMsg != nullptr)
        {
            auto messageDialog = ref new Windows::UI::Popups::MessageDialog(errorMsg, "Unable to open Trip");
            create_task(messageDialog->ShowAsync());
        }
    }
}