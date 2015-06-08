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

//
// SharePage.cpp
// Implementation of the SharePage class
//

#include "pch.h"
#include "SharePage.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel::DataTransfer;

void SharePage::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Register the current page as a share source.
    dataRequestedToken = DataTransferManager::GetForCurrentView()->DataRequested += ref new TypedEventHandler<DataTransferManager^, DataRequestedEventArgs^>(this, &SharePage::OnDataRequested);
}

void SharePage::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // Unregister the current page as a share source.
    DataTransferManager::GetForCurrentView()->DataRequested -= dataRequestedToken;
}

// When share is invoked (by the user or programatically) the event handler we registered will be called to populate the datapackage with the
// data to be shared.
void SharePage::OnDataRequested(DataTransferManager^ sender, DataRequestedEventArgs^ e)
{
    // Call the scenario specific function to populate the datapackage with the data to be shared.
    if (GetShareContent(e->Request))
    {
        // Out of the datapackage properties, the title is required. If the scenario completed successfully, we need
        // to make sure the title is valid since the sample scenario gets the title from the user.
        if (e->Request->Data->Properties->Title == nullptr)
        {
            e->Request->FailWithDisplayText(MainPage::MissingTitleError);
        }

        // Populate the datapackage properties with the content source application link for this scenario
        e->Request->Data->Properties->ContentSourceApplicationLink = GetApplicationLink(GetType()->FullName);
    }
}

void SharePage::ShowUIButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // If the user clicks the share button, invoke the share flow programatically.
    Windows::ApplicationModel::DataTransfer::DataTransferManager::ShowShareUI();
}

// This function is implemented by each scenario to share the content specific to that scenario (text, link, image, etc.).
bool SharePage::GetShareContent(DataRequest^ request)
{
    request->FailWithDisplayText("Scenario specific class must override GetShareContent");
    return false;
}

Windows::Foundation::Uri^ SharePage::GetApplicationLink(String^ sharePageFullName)
{
    return ref new Uri("ms-sdk-sharesourcecpp:navigate?page=" + sharePageFullName);
}
