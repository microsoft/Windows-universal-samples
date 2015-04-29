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
// Scenario3_Notifications.xaml.cpp
// Implementation of the Scenario3_Notifications class
//

#include "pch.h"
#include "Scenario3_Notifications.xaml.h"

using namespace BackgroundTransfer;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace SDKTemplate;
using namespace Windows::UI::Core;
using namespace Windows::UI::Notifications;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Networking::BackgroundTransfer;
using namespace Windows::Web;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Data::Xml::Dom;

UINT32 Scenario3_Notifications::runId = 0;

Scenario3_Notifications::Scenario3_Notifications()
{
    // Use a unique group name so that no other component in the app uses the same group. The recommended way
    // is to generate a GUID and use it as group name as shown below.
    notificationsGroup = BackgroundTransferGroup::CreateGroup("{C3E919D3-1AE1-4EB8-9E2C-6E92CEF93E6B}");

    // When creating a group, we can optionally define the transfer behavior of transfers associated with the
    // group. A "parallel" transfer behavior allows multiple transfers in the same group to run concurrently
    // (default). A "serialized" transfer behavior allows at most one default priority transfer at a time for
    // the group.
    notificationsGroup->TransferBehavior = BackgroundTransferBehavior::Parallel;

    InitializeComponent();
}

Scenario3_Notifications::~Scenario3_Notifications()
{
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3_Notifications::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    // An application must enumerate downloads when it gets started to prevent stale downloads/uploads.
    // Typically this can be done in the App class by overriding OnLaunched() and checking for
    // "args->Kind == ActivationKind::Launch" to detect an actual app launch.
    // We do it here in the sample to keep the sample code consolidated.
    // Note that for this specific scenario we're not interested in downloads from previous instances:
    // We'll just enumerate downloads from previous instances and cancel them immediately.
    CancelActiveDownloads();
}

void Scenario3_Notifications::CancelActiveDownloads()
{
    // Note that we only enumerate transfers that belong to the transfer group used by this sample scenario.
    // We'll not enumerate transfers started by other sample scenarios in this app.
    create_task(BackgroundDownloader::GetCurrentDownloadsForTransferGroupAsync(notificationsGroup))
        .then([this] (IVectorView<DownloadOperation^>^ downloads)
    {
        if (downloads->Size > 0)
        {
            cancellation_token_source cancellationToken;

            for (unsigned int i = 0; i < downloads->Size; i++)
            {
                create_task(downloads->GetAt(i)->AttachAsync(), cancellationToken.get_token())
                    .then([this] (task<DownloadOperation^> previousTask)
                {
                    try
                    {
                        previousTask.get();
                    }
                    catch (Exception^ ex)
                    {
                        // Ignore errors: We're just interested in canceling all downloads from previous instances.
                    }
                });
            }

            // Now cancel all downloads that were started by a previous instance of this sample scenario.
            cancellationToken.cancel();

            Log("Canceled " + downloads->Size + " downloads from previous instances of this scenario.");
        }

        // After cleaning up downloads from previous scenarios, enable buttons to allow the user to run the sample.
        ToastNotificationButton->IsEnabled = true;
        TileNotificationButton->IsEnabled = true;

    }).then([this] (task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            LogException("Discovery error", ex);
        }
    });
}

void Scenario3_Notifications::ToastNotificationButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Create a downloader and associate all its downloads with the transfer group used for this scenario.
    BackgroundDownloader^ downloader = ref new BackgroundDownloader();
    downloader->TransferGroup = notificationsGroup;

    // Create a ToastNotification that should be shown when all transfers succeed.
    XmlDocument^ successToastXml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastText01);
    successToastXml->GetElementsByTagName("text")->Item(0)->InnerText =
        "All three downloads completed successfully.";
    ToastNotification^ successToast = ref new ToastNotification(successToastXml);
    downloader->SuccessToastNotification = successToast;

    // Create a ToastNotification that should be shown if at least one transfer fails.
    XmlDocument^ failureToastXml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastText01);
    failureToastXml->GetElementsByTagName("text")->Item(0)->InnerText =
        "At least one download completed with failure.";
    ToastNotification^ failureToast = ref new ToastNotification(failureToastXml);
    downloader->FailureToastNotification = failureToast;

    // Now create and start downloads for the configured BackgroundDownloader object.
    RunDownloads(downloader, ScenarioType::Toast);
}

void Scenario3_Notifications::TileNotificationButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Create a downloader and associate all its downloads with the transfer group used for this scenario.
    BackgroundDownloader^ downloader = ref new BackgroundDownloader();
    downloader->TransferGroup = notificationsGroup;

    FILETIME nowFileTime;
    GetSystemTimeAsFileTime(&nowFileTime);
    ULARGE_INTEGER now;
    now.LowPart = nowFileTime.dwLowDateTime;
    now.HighPart = nowFileTime.dwHighDateTime;
    DateTime nowPlusTenMinutes;
    nowPlusTenMinutes.UniversalTime = now.QuadPart + tenMinutesIn100NanoSecondUnits;

    // Create a TileNotification that should be shown when all transfers succeed.
    XmlDocument^ successTileXml = TileUpdateManager::GetTemplateContent(TileTemplateType::TileSquare150x150Text03);
    XmlNodeList^ successTextNodes = successTileXml->GetElementsByTagName("text");
    successTextNodes->Item(0)->InnerText = "All three";
    successTextNodes->Item(1)->InnerText = "downloads";
    successTextNodes->Item(2)->InnerText = "completed";
    successTextNodes->Item(3)->InnerText = "successfully.";
    TileNotification^ successTile = ref new TileNotification(successTileXml);
    successTile->ExpirationTime = nowPlusTenMinutes;
    downloader->SuccessTileNotification = successTile;

    // Create a TileNotification that should be shown if at least one transfer fails.
    XmlDocument^ failureTileXml = TileUpdateManager::GetTemplateContent(TileTemplateType::TileSquare150x150Text03);
    XmlNodeList^ failureTextNodes = failureTileXml->GetElementsByTagName("text");
    failureTextNodes->Item(0)->InnerText = "At least";
    failureTextNodes->Item(1)->InnerText = "one download";
    failureTextNodes->Item(2)->InnerText = "completed";
    failureTextNodes->Item(3)->InnerText = "with failure.";
    TileNotification^ failureTile = ref new TileNotification(failureTileXml);
    failureTile->ExpirationTime = nowPlusTenMinutes;
    downloader->FailureTileNotification = failureTile;

    // Now create and start downloads for the configured BackgroundDownloader object.
    RunDownloads(downloader, ScenarioType::Tile);
}

void Scenario3_Notifications::RunDownloads(BackgroundDownloader^ downloader, ScenarioType type)
{
    // Use a unique ID for every button click, to help the user associate downloads of the same run in the logs.
    runId++;

    std::vector<task<void>> createDownloadTasks;
    Vector<DownloadOperation^>^ downloadOperations = ref new Vector<DownloadOperation^>();

    // First we create three download operations: Note that we don't start downloads immediately. It is
    // important to first create all operations that should participate in the toast/tile update. Once all
    // operations have been created, we can start them.
    // If we start a download and create a second one afterwards, there is a race where the first download
    // may complete before we were able to create the second one. This would result in the toast/tile being
    // shown before we even create the second download.
    task<void> current;
    if (!TryCreateDownloadAsync(downloader, 1, L"FastDownload", type, downloadOperations, current))
    {
        return;
    }
    createDownloadTasks.push_back(current);

    if (!TryCreateDownloadAsync(downloader, 5, L"MediumDownload", type, downloadOperations, current))
    {
        return;
    }
    createDownloadTasks.push_back(current);

    if (!TryCreateDownloadAsync(downloader, 10, L"SlowDownload", type, downloadOperations, current))
    {
        return;
    }
    createDownloadTasks.push_back(current);

    when_all(createDownloadTasks.begin(), createDownloadTasks.end()).then([this, downloadOperations]
    {
        if (downloadOperations->Size != 3)
        {
            // At least one of the three downloads couldn't be created. Just return and don't start remaining downloads.
            return;
        }

        // Once all downloads participating in the toast/tile update have been created, start them.
        for (unsigned int i = 0; i < downloadOperations->Size; i++)
        {
            RunDownload(downloadOperations->GetAt(i));
        }
    }).then([this] (task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            LogException("Starting downloads", ex);
        }
    });
}

bool Scenario3_Notifications::TryCreateDownloadAsync(
    BackgroundDownloader^ downloader,
    UINT8 delaySeconds,
    String^ fileNameSuffix,
    ScenarioType type,
    Vector<DownloadOperation^>^ downloadOperations,
    task<void>& resultTask)
{
    String^ sourceString = serverAddressField->Text + "?delay=" + delaySeconds;

    Uri^ source;
    if (!rootPage->TryGetUri(sourceString, &source))
    {
        rootPage->NotifyUser("Unable to create the source URI.", NotifyType::ErrorMessage);
        return false;
    }

    String^ fileName = ((type == ScenarioType::Tile) ? L"Tile" : L"Toast") + "." + runId + "." + fileNameSuffix + ".txt";
    resultTask = create_task(KnownFolders::PicturesLibrary->CreateFileAsync(fileName, CreationCollisionOption::GenerateUniqueName))
        .then([this, source, downloader, downloadOperations] (StorageFile^ destinationFile)
    {
        downloadOperations->Append(downloader->CreateDownload(source, destinationFile));

    }).then([this] (task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            LogException("Create file error", ex);
        }
    });

    return true;
}

void Scenario3_Notifications::RunDownload(DownloadOperation^ download)
{
    Log("Downloading " + download->ResultFile->Name);

    create_task(download->StartAsync()).then([this] (DownloadOperation^ download)
    {
        LogStatus("Downloading " + download->ResultFile->Name + " completed.", NotifyType::StatusMessage);

    }).then([this, download] (task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            LogException("Execution error", ex, download);
        }
    });
}

void Scenario3_Notifications::LogException(String^ title, Exception^ ex, DownloadOperation^ download)
{
    String^ fullTitle;
    if (download != nullptr)
    {
        fullTitle = title + " - " + download->ResultFile->Name;
    }
    else
    {
        fullTitle = title;
    }

    WebErrorStatus error = BackgroundTransferError::GetStatus(ex->HResult);
    if (error == WebErrorStatus::Unknown)
    {
        LogStatus(fullTitle + ": " + ex, NotifyType::ErrorMessage);
    }
    else
    {
        LogStatus(fullTitle + ": " + error.ToString(), NotifyType::ErrorMessage);
    }
}

void Scenario3_Notifications::Log(String^ message)
{
    outputField->Text += message + "\r\n";
}

void Scenario3_Notifications::LogStatus(String^ message, NotifyType type)
{
    rootPage->NotifyUser(message, type);
    outputField->Text += message + "\r\n";
}
