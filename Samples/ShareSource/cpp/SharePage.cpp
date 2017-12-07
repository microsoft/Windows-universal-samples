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
    DataTransferManager^ dataTransferManager = DataTransferManager::GetForCurrentView();

    // Register the current page as a share source.
    dataRequestedToken = dataTransferManager->DataRequested += ref new TypedEventHandler<DataTransferManager^, DataRequestedEventArgs^>(this, &SharePage::OnDataRequested);

    // Request to be notified when the user chooses a share target app.
    targetApplicationChosenToken = dataTransferManager->TargetApplicationChosen += ref new TypedEventHandler<DataTransferManager^, TargetApplicationChosenEventArgs^>(this, &SharePage::OnTargetApplicationChosen);
}

void SharePage::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // Unregister our event handlers.
    DataTransferManager^ dataTransferManager = DataTransferManager::GetForCurrentView();
    dataTransferManager->DataRequested -= dataRequestedToken;
    dataTransferManager->TargetApplicationChosen -= targetApplicationChosenToken;
}

// When share is invoked (by the user or programatically) the event handler we registered will be called to populate the datapackage with the
// data to be shared.
void SharePage::OnDataRequested(DataTransferManager^ sender, DataRequestedEventArgs^ e)
{
    // Register to be notified if the share operation completes.
    e->Request->Data->ShareCompleted += ref new TypedEventHandler<DataPackage^, ShareCompletedEventArgs^>(this, &SharePage::OnShareCompleted);

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

void SharePage::OnTargetApplicationChosen(DataTransferManager^ sender, TargetApplicationChosenEventArgs^ e)
{
    this->rootPage->NotifyUser("User chose " + e->ApplicationName, NotifyType::StatusMessage);
}

void SharePage::OnShareCompleted(DataPackage^ sender, ShareCompletedEventArgs^ e)
{
    String^ shareCompletedStatus = "Shared successfully. ";

    // Typically, this information is not displayed to the user because the
    // user already knows which share target was selected.
    if (!e->ShareTarget->AppUserModelId->IsEmpty())
    {
        // The user picked an app.
        shareCompletedStatus += "Target: App \"" + e->ShareTarget->AppUserModelId + "\"";
    }
    else if (e->ShareTarget->ShareProvider != nullptr)
    {
        // The user picked a ShareProvider.
        shareCompletedStatus += "Target: Share Provider \"" + e->ShareTarget->ShareProvider->Title + "\"";
    }

    this->rootPage->NotifyUser(shareCompletedStatus, NotifyType::StatusMessage);
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
