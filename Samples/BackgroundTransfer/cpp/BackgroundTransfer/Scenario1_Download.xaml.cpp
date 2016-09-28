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
// Scenario1_Download.xaml.cpp
// Implementation of the Scenario1_Download class
//

#include "pch.h"
#include "Scenario1_Download.xaml.h"

using namespace BackgroundTransfer;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace SDKTemplate;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Networking::BackgroundTransfer;
using namespace Windows::Web;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;

Scenario1_Download::Scenario1_Download()
{
    InitializeComponent();
    cancellationToken = new cancellation_token_source();
}

Scenario1_Download::~Scenario1_Download()
{
    if (cancellationToken != nullptr)
    {
        delete cancellationToken;
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1_Download::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
    DiscoverActiveDownloads();
}

// Enumerate the downloads that were going on in the background while the app was closed.
void Scenario1_Download::DiscoverActiveDownloads()
{
    create_task(BackgroundDownloader::GetCurrentDownloadsAsync()).then([this] (IVectorView<DownloadOperation^>^ downloads)
    {
        Log("Loading background downloads: " + downloads->Size);

        for (unsigned int i = 0; i < downloads->Size; i++)
        {
            DownloadOperation^ download = downloads->GetAt(i);
            Log("Discovered background download: " + download->Guid + ", Status: " + download->Progress.Status.ToString());

            // Attach progress and completion handlers.
            HandleDownloadAsync(download, false);
        }
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

void Scenario1_Download::HandleDownloadAsync(DownloadOperation^ download, boolean start)
{
    IAsyncOperationWithProgress<DownloadOperation^, DownloadOperation^>^ async;
    activeDownloads[download->Guid.GetHashCode()] = download;

    LogStatus("Running: " + download->Guid, NotifyType::StatusMessage);

    if (start)
    {
        async = download->StartAsync();
    }
    else
    {
        async = download->AttachAsync();
    }

    async->Progress = ref new AsyncOperationProgressHandler<DownloadOperation^, DownloadOperation^>(this, &Scenario1_Download::DownloadProgress);
    create_task(async, cancellationToken->get_token()).then([this] (DownloadOperation^ download)
    {
        ResponseInformation^ response = download->GetResponseInformation();

        // GetResponseInformation() returns null for non-HTTP transfers (e.g., FTP).
        String^ statusCode = response != nullptr ? response->StatusCode.ToString() : "";

        LogStatus("Completed: " + download->Guid + ", Status Code: " + statusCode, NotifyType::StatusMessage);
    }).then([this, download] (task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            LogException("Handle download", ex);
        }
        catch (const task_canceled&)
        {
            LogStatus("Canceled: " + download->Guid, NotifyType::StatusMessage);
        }

        // It is important to note, that this method executes on UI thread. This guarantees that
        // access to the activeDownloads is synchronized.
        activeDownloads.erase(download->Guid.GetHashCode());
    });
}

void Scenario1_Download::DownloadProgress(IAsyncOperationWithProgress<DownloadOperation^, DownloadOperation^>^ operation, DownloadOperation^ download)
{
    // DownloadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
    // we must make a local copy so that we can have a consistent view of that ever-changing state
    // throughout this method's lifetime.
    BackgroundDownloadProgress currentProgress = download->Progress;

    MarshalLog("Progress: " + download->Guid + ", Status: " + currentProgress.Status.ToString());

    UINT64 percent = 0;
    if (currentProgress.TotalBytesToReceive > 0)
    {
        percent = currentProgress.BytesReceived * 100 / currentProgress.TotalBytesToReceive;
    }

    MarshalLog(" - Transfered bytes: " + currentProgress.BytesReceived + " of " +
        currentProgress.TotalBytesToReceive + ", " + percent + "%");

    if (currentProgress.HasRestarted)
    {
        MarshalLog(" - Download restarted");
    }

    if (currentProgress.HasResponseChanged)
    {
        // We have received new response headers from the server.
        // Be aware that GetResponseInformation() returns null for non-HTTP transfers (e.g., FTP).
        ResponseInformation^ response = download->GetResponseInformation();
        int headersCount = response != nullptr ? response->Headers->Size : 0;

        MarshalLog(" - Response updated; Header count: " + headersCount);

        // If you want to stream the response data this is a good time to start.
        // download->GetResultStreamAt(0);
    }
}

void Scenario1_Download::StartDownload(BackgroundTransferPriority priority)
{
    // By default 'serverAddressField' is disabled and URI validation is not required. When enabling the text box
    // validating the URI is required since it was received from an untrusted source (user input).
    // The URI is validated by calling TryGetUri() that will return 'false' for strings that are not valid URIs.
    // Note that when enabling the text box users may provide URIs to machines on the intrAnet that require the
    // "Home or Work Networking" capability.
    Uri^ source;
    if (!rootPage->TryGetUri(serverAddressField->Text, &source))
    {
        return;
    }

    String^ destination = StringTrimmer::Trim(fileNameField->Text);

    if (destination == "")
    {
        rootPage->NotifyUser("A local file name is required.", NotifyType::ErrorMessage);
        return;
    }

    create_task(KnownFolders::PicturesLibrary->CreateFileAsync(destination, CreationCollisionOption::GenerateUniqueName))
        .then([this, source, priority] (StorageFile^ destinationFile)
    {
        BackgroundDownloader^ downloader = ref new BackgroundDownloader();
        DownloadOperation^ download = downloader->CreateDownload(source, destinationFile);

        Log("Downloading " + source->AbsoluteUri + " to " + destinationFile->Name + " with " +
            ((priority == BackgroundTransferPriority::Default) ? "default" : "high") +
            " priority, " + download->Guid);

        download->Priority = priority;

        // Attach progress and completion handlers.
        HandleDownloadAsync(download, true);
    }).then([this] (task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            LogException("Error", ex);
        }
    });
}

void Scenario1_Download::StartDownload_Click(Object^ sender, RoutedEventArgs^ e)
{
    StartDownload(BackgroundTransferPriority::Default);
}

void Scenario1_Download::StartHighPriorityDownload_Click(Object^ sender, RoutedEventArgs^ e)
{
    StartDownload(BackgroundTransferPriority::High);
}

void Scenario1_Download::PauseAll_Click(Object^ sender, RoutedEventArgs^ e)
{
    Log("Downloads: " + activeDownloads.size());
    for (auto iterator = activeDownloads.begin(); iterator != activeDownloads.end(); iterator++)
    {
        DownloadOperation^ download = iterator->second;

        // DownloadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
        // we must make a local copy so that we can have a consistent view of that ever-changing state
        // throughout this method's lifetime.
        BackgroundDownloadProgress currentProgress = download->Progress;

        if (currentProgress.Status == BackgroundTransferStatus::Running)
        {
            download->Pause();
            Log("Paused: " + download->Guid);
        }
        else
        {
            Log("Skipped: " + download->Guid + ", Status: " + currentProgress.Status.ToString());
        }
    }
}

void Scenario1_Download::ResumeAll_Click(Object^ sender, RoutedEventArgs^ e)
{
    Log("Downloads: " + activeDownloads.size());
    for (auto iterator = activeDownloads.begin(); iterator != activeDownloads.end(); iterator++)
    {
        DownloadOperation^ download = iterator->second;

        // DownloadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
        // we must make a local copy so that we can have a consistent view of that ever-changing state
        // throughout this method's lifetime.
        BackgroundDownloadProgress currentProgress = download->Progress;

        if (currentProgress.Status == BackgroundTransferStatus::PausedByApplication)
        {
            download->Resume();
            Log("Resumed: " + download->Guid);
        }
        else
        {
            Log("Skipped: " + download->Guid + ", Status: " + currentProgress.Status.ToString());
        }
    }
}

void Scenario1_Download::CancelAll_Click(Object^ sender, RoutedEventArgs^ e)
{
    Log("Cancelling Downloads: " + activeDownloads.size());

    cancellationToken->cancel();
    cancellationToken = new cancellation_token_source();
    activeDownloads.clear();
}

void Scenario1_Download::LogException(String^ title, Exception^ ex)
{
    WebErrorStatus error = BackgroundTransferError::GetStatus(ex->HResult);
    if (error == WebErrorStatus::Unknown)
    {
        LogStatus(title + ": " + ex, NotifyType::ErrorMessage);
    }
    else
    {
        LogStatus(title + ": " + error.ToString(), NotifyType::ErrorMessage);
    }
}

// When operations happen on a background thread we have to marshal UI updates back to the UI thread.
void Scenario1_Download::MarshalLog(String^ value)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, value] ()
    {
        Log(value);
    }));
}

void Scenario1_Download::Log(String^ message)
{
    outputField->Text += message + "\r\n";
}

void Scenario1_Download::LogStatus(String^ message, NotifyType type)
{
    rootPage->NotifyUser(message, type);
    outputField->Text += message + "\r\n";
}
