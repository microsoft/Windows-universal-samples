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
#include "Scenario6_RecoverableErrors.xaml.h"
#include <assert.h>

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::BackgroundTransfer;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web;

Scenario6_RecoverableErrors::Scenario6_RecoverableErrors()
{
    InitializeComponent();
}

void Scenario6_RecoverableErrors::OnNavigatedTo(NavigationEventArgs^ e)
{
    // An application must enumerate downloads when it gets started to prevent stale downloads/uploads.
    // Typically this can be done in the App class by overriding OnLaunched() and checking for
    // "args.Kind == ActivationKind.Launch" to detect an actual app launch.
    // We do it here in the sample to keep the sample code consolidated.
    // Note that for this specific scenario we are not interested in downloads from previous instances, so
    // we just enumerate all downloads from previous instances and cancel them immediately.
    CancelActiveDownloadsAsync();
}

void Scenario6_RecoverableErrors::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // Cancel any active download.
    cancellationTokenSource.cancel();

}

task<void> Scenario6_RecoverableErrors::CancelActiveDownloadsAsync()
{
    return create_task(BackgroundDownloader::GetCurrentDownloadsAsync()).then([this](task<IVectorView<DownloadOperation^>^> downloadTask)
    {
        IVectorView<DownloadOperation^>^ downloads;
        try
        {
            downloads = downloadTask.get();
        }
        catch (Exception^ ex)
        {
            if (IsWebException("Discovery error", ex))
            {
                // Don't worry if the old downloads encountered web exceptions.
                return task_from_result();
            }
            throw;
        }

        // If previous sample instances/scenarios started transfers that haven't completed yet, cancel them now
        // so that we can start this scenario cleanly.
        if (downloads->Size > 0)
        {
            cancellation_token_source canceledToken;
            canceledToken.cancel();

            std::vector<task<void>> taskList;
            for (DownloadOperation^ downloadOperation : downloads)
            {
                taskList.push_back(create_task(downloadOperation->AttachAsync(), canceledToken.get_token()).then([](task<DownloadOperation^> previousTask)
                {
                    try
                    {
                        previousTask.get();
                    }
                    catch (task_canceled&)
                    {
                    }
                }));
            }

            return when_all(taskList.begin(), taskList.end()).then([this, downloads]()
            {
                rootPage->NotifyUser("Canceled " + downloads->Size.ToString() + " downloads.", NotifyType::StatusMessage);
            });
        }
        return task_from_result();
    });
}

void Scenario6_RecoverableErrors::OnDownloadProgress(IAsyncOperationWithProgress<DownloadOperation^, DownloadOperation^>^ sender, DownloadOperation^ progress)
{
    // We capture a snapshot of DownloadOperation.Progress because
    // it is is updated in real-time while the operation is ongoing.
    BackgroundDownloadProgress currentProgress = progress->Progress;

    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, currentProgress]()
    {
        // Prepare the progress message to display in the UI.
        uint64_t percent = 100;
        if (currentProgress.TotalBytesToReceive > 0)
        {
            percent = currentProgress.BytesReceived * 100 / currentProgress.TotalBytesToReceive;
        }
        DownloadedStatusText->Text = currentProgress.Status.ToString() + " - " + percent.ToString() + "% downloaded.";
    
        if (currentProgress.HasRestarted)
        {
            rootPage->NotifyUser("Download has restarted.", NotifyType::StatusMessage);
        }

        if (currentProgress.Status == BackgroundTransferStatus::PausedRecoverableWebErrorStatus)
        {
            // The only value we put in RecoverableWebErrorStatuses is WebErrorStatus.Forbidden,
            // so that will be the only value observed here.
            assert(download->CurrentWebErrorStatus->Value == WebErrorStatus::Forbidden);

            rootPage->NotifyUser("URL has expired.", NotifyType::ErrorMessage);

            // The URL expired. Ask the user for information so we can get a new URL.
            create_task(ReauthorizeDialog->ShowAsync()).then([this](ContentDialogResult result)
            {
                if (result == ContentDialogResult::Primary)
                {
                    // For the purpose of this sample, we simply remove "?shouldExpire=yes" from the URL
                    // to indicate that the sample server should treat it like a new, unexpired URL.
                    std::wstring originalUrlString(download->RequestedUri->AbsoluteUri->Data());
                    auto truncationPoint = originalUrlString.find(L'?');
                    if (truncationPoint == std::wstring::npos)
                    {
                        truncationPoint = originalUrlString.length();
                    }
                    auto newUrlString = ref new String(originalUrlString.c_str(), truncationPoint);

                    rootPage->NotifyUser("Updating URL and resuming the download.", NotifyType::StatusMessage);
                    download->RequestedUri = ref new Uri(newUrlString);
                    download->Resume();
                }
                else
                {
                    // Cancel the download.
                    cancellationTokenSource.cancel();

                    // Re-create the cancellation_token_source for future downloads.
                    cancellationTokenSource = cancellation_token_source();
                }
            });
        }
    }));
}

void Scenario6_RecoverableErrors::StartDownload_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Reset the output whenever a new download attempt begins.
    DownloadedInfoText->Text = "";
    DownloadedStatusText->Text = "";
    rootPage->NotifyUser("", NotifyType::StatusMessage);

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

    create_task(KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary))
        .then([this, destination](StorageFolder^ picturesLibrary) {
        return picturesLibrary->CreateFileAsync(destination, CreationCollisionOption::GenerateUniqueName);
    }).then([this, source](task<StorageFile^> previousTask)
    {
        StorageFile^ destinationFile;
        try
        {
            destinationFile = previousTask.get();
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser("Error while creating file: " + ex->Message, NotifyType::ErrorMessage);
            return task_from_result();
        }

        BackgroundDownloader^ downloader = ref new BackgroundDownloader();
        download = downloader->CreateDownload(source, destinationFile);

        // Opt into "random access" mode. Transfers configured this way have full support for resumable URL updates.
        // If the timestamp or file size of the updated URL is different from that of the previous URL, the download
        // will restart from scratch. Otherwise, the transfer will resume from the same position using the new URL.
        //
        // Due to OS limitations, downloads that don't opt into "random access" mode will always restart from scratch
        // whenever their URL is updated.
        download->IsRandomAccessRequired = true;

        if (configureRecoverableErrorsCheckBox->IsChecked->Value)
        {
            // Declare HTTP 403 (WebErrorStatus.Forbidden) as a recoverable error.
            download->RecoverableWebErrorStatuses->Append(WebErrorStatus::Forbidden);
        }

        DownloadedInfoText->Text = "Downloading to " + destinationFile->Name + ", " + download->Guid.ToString();

        // Start the download and wait for it to complete.
        return HandleDownloadAsync();
    });
}

task<void> Scenario6_RecoverableErrors::HandleDownloadAsync()
{
    startDownloadButton->IsEnabled = false;
    IAsyncOperationWithProgress<DownloadOperation^, DownloadOperation^>^ downloadOperation = download->StartAsync();
    downloadOperation->Progress = ref new AsyncOperationProgressHandler<DownloadOperation^, DownloadOperation^>(this, &Scenario6_RecoverableErrors::OnDownloadProgress);

    return create_task(downloadOperation, cancellationTokenSource.get_token()).then([this](DownloadOperation^ download)
    {
        rootPage->NotifyUser("Download completed successfully.", NotifyType::StatusMessage);
    }).then([this](task<void> precedingTask)
    {
        try
        {
            precedingTask.get();
        }
        catch (Exception^ ex)
        {
            // Abandon the operation if a web exception occurs.
            if (IsWebException("Execution error", ex))
            {
                return;
            }
            throw;
        }
        catch (const task_canceled&)
        {
            // Download was canceled.
        }
    }).then([this]()
    {
        download = nullptr;

        startDownloadButton->IsEnabled = true;
    });
}

bool Scenario6_RecoverableErrors::IsWebException(String^ title, Exception^ ex)
{
    WebErrorStatus error = BackgroundTransferError::GetStatus(ex->HResult);
    bool result = (error != WebErrorStatus::Unknown);
    String^ message = result ? error.ToString() : ex->Message;
    if (download == nullptr)
    {
        rootPage->NotifyUser(title + ": " + message, NotifyType::ErrorMessage);
    }
    else
    {
        rootPage->NotifyUser(download->Guid.ToString() + " - " + title + ": " + message, NotifyType::ErrorMessage);
    }

    return result;
}
