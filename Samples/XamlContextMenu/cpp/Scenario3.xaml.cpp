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
#include "Scenario3.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;

Scenario3::Scenario3()
{
    InitializeComponent();
    AllItems = SampleDataModel::GetSampleData();
    sharedFlyout = safe_cast<MenuFlyout^>(Resources->Lookup("SampleContextMenu"));
}

void Scenario3::ListView_ContextRequested(UIElement^ sender, ContextRequestedEventArgs^ e)
{
    // Walk up the tree to find the ListViewItem.
    // There may not be one if the click wasn't on an item.
    auto requestedElement = safe_cast<FrameworkElement^>(e->OriginalSource);
    while ((requestedElement != sender) && !dynamic_cast<ListViewItem^>(requestedElement))
    {
        requestedElement = safe_cast<FrameworkElement^>(VisualTreeHelper::GetParent(requestedElement));
    }
    if (requestedElement != sender)
    {
        // The context menu request was indeed for a ListViewItem.
        auto model = safe_cast<SampleDataModel^>(ItemListView->ItemFromContainer(requestedElement));
        Point point;
        if (e->TryGetPosition(requestedElement, &point))
        {
            rootPage->NotifyUser("Showing flyout for " + model->Title + " at " + point.X.ToString() + "," + point.Y.ToString(), NotifyType::StatusMessage);
            sharedFlyout->ShowAt(requestedElement, point);
        }
        else
        {
            // Not invoked via pointer, so let XAML choose a default location.
            rootPage->NotifyUser("Showing flyout for " + model->Title + " at default location", NotifyType::StatusMessage);
            sharedFlyout->ShowAt(requestedElement);
        }

        e->Handled = true;
    }
}

void Scenario3::ListView_ContextCanceled(UIElement^ sender, RoutedEventArgs^ e)
{
    sharedFlyout->Hide();
}

SampleDataModel^ Scenario3::GetDataModelForCurrentListViewFlyout()
{
    // Obtain the ListViewItem for which the user requested a context menu.
    auto listViewItem = sharedFlyout->Target;

    // Get the data model for the ListViewItem.
    return safe_cast<SampleDataModel^>(ItemListView->ItemFromContainer(listViewItem));
}

void Scenario3::OpenMenuItem_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    SampleDataModel^ model = GetDataModelForCurrentListViewFlyout();
    rootPage->NotifyUser("Item: " + model->Title + ", Action: Open", NotifyType::StatusMessage);
}

void Scenario3::PrintMenuItem_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    SampleDataModel^ model = GetDataModelForCurrentListViewFlyout();
    rootPage->NotifyUser("Item: " + model->Title + ", Action: Print", NotifyType::StatusMessage);
}

void Scenario3::DeleteMenuItem_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    SampleDataModel^ model = GetDataModelForCurrentListViewFlyout();
    rootPage->NotifyUser("Item: " + model->Title + ", Action: Delete", NotifyType::StatusMessage);
}

