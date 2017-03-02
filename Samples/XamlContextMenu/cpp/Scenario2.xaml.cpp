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
#include "Scenario2.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario2::Scenario2()
{
    InitializeComponent();
    AllItems = SampleDataModel::GetSampleData();
}

SampleDataModel^ Scenario2::GetDataModelForCurrentListViewFlyout()
{
    // Obtain the ListViewItem for which the user requested a context menu.
    auto listViewItem = SharedFlyout->Target;
    
    // Get the data model for the ListViewItem.
    return safe_cast<SampleDataModel^>(ItemListView->ItemFromContainer(listViewItem));
}

void Scenario2::OpenMenuItem_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    SampleDataModel^ model = GetDataModelForCurrentListViewFlyout();
    rootPage->NotifyUser("Item: " + model->Title + ", Action: Open", NotifyType::StatusMessage);
}

void Scenario2::PrintMenuItem_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    SampleDataModel^ model = GetDataModelForCurrentListViewFlyout();
    rootPage->NotifyUser("Item: " + model->Title + ", Action: Print", NotifyType::StatusMessage);
}

void Scenario2::DeleteMenuItem_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    SampleDataModel^ model = GetDataModelForCurrentListViewFlyout();
    rootPage->NotifyUser("Item: " + model->Title + ", Action: Delete", NotifyType::StatusMessage);
}

