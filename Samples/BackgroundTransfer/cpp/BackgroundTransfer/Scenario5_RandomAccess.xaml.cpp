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
#include "Scenario5_RandomAccess.xaml.h"

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

Scenario5_RandomAccess::Scenario5_RandomAccess()
{
    InitializeComponent();
}

void Scenario5_RandomAccess::OnNavigatedTo(NavigationEventArgs^ e)
{
    // An application must enumerate downloads when it gets started to prevent stale downloads/uploads.
    // Typically this can be done in the App class by overriding OnLaunched() and checking for
    // "args.Kind == ActivationKind.Launch" to detect an actual app launch.
    // We do it here in the sample to keep the sample code consolidated.
    // Note that for this specific scenario we are not interested in downloads from previous instances, so
    // we just enumerate all downloads from previous instances and cancel them immediately.
    CancelActiveDownloadsAsync();
}

void Scenario5_RandomAccess::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // Cancel any outstanding reads.
    readCancellationTokenSource.cancel();

}

task<void> Scenario5_RandomAccess::CancelActiveDownloadsAsync()
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

void Scenario5_RandomAccess::PauseDownload_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (download != nullptr)
    {
        download->Pause();
        pauseDownloadButton->IsEnabled = false;
        resumeDownloadButton->IsEnabled = true;
    }
}

void Scenario5_RandomAccess::ResumeDownload_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (download != nullptr)
    {
        download->Resume();
        pauseDownloadButton->IsEnabled = true;
        resumeDownloadButton->IsEnabled = false;
    }
}

void Scenario5_RandomAccess::SeekDownload_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Cancel the ongoing read operation (if any).
    if (readOperation != task<IBuffer^>())
    {
        readCancellationTokenSource.cancel();

        // Re-create the cancellation_token_source for future reads.
        readCancellationTokenSource = cancellation_token_source();
    }

    if (randomAccessStream != nullptr)
    {
        // Update the stream's seek position to the user-selected one.
        // The new seek position will take effect on the next ReadAsync operation.
        auto seekPositionInMegaBytes = static_cast<unsigned>(seekSlider->Value);
        unsigned seekPositionInBytes = seekPositionInMegaBytes * BytesPerMegaByte;
        randomAccessStream->Seek(seekPositionInBytes);
    }
}

String^ Scenario5_RandomAccess::FormatDownloadedRanges(IIterable<BackgroundTransferFileRange>^ ranges)
{
    String^ result = "";
    for (const BackgroundTransferFileRange& range : ranges)
    {
        if (result != "")
        {
            result += ", ";
        }

        result += range.Offset.ToString() + "-" + (range.Offset + range.Length - 1).ToString();
    }

    if (result == "")
    {
        result = "None";
    }
    return result;
}

void Scenario5_RandomAccess::OnDownloadProgress(IAsyncOperationWithProgress<DownloadOperation^, DownloadOperation^>^ sender, DownloadOperation^ progress)
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
    }));
}

void Scenario5_RandomAccess::OnRangesDownloaded(DownloadOperation^ sender, BackgroundTransferRangesDownloadedEventArgs^ args)
{
    // BackgroundTransfer will wait for the app's RangesDownloaded event handlers to finish executing before
    // triggering the next RangesDownloaded event. Any download progress made while the app's handlers are
    // executing get coalesced into a single RangesDownloaded event.
    //
    // When calling async APIs inside this handler, we must first take a deferral and then complete it once we
    // are done processing the event. Otherwise, BackgroundTransfer will assume that the handler is done executing
    // as soon as the synchronous portion of the handler returns, even if there are still tasks running.
    auto deferral = args->GetDeferral();

    // AddedRanges tells you which ranges were downloaded since the last RangesDownloaded event.
    // To get the full list of downloaded ranges, use the GetDownloadedRanges method.
    String^ message = " Newly-downloaded ranges: " + FormatDownloadedRanges(args->AddedRanges);

    // A download may restart if the server contents changed while the download is in progress.
    if (args->WasDownloadRestarted)
    {
        message += " (download was restarted)";
    }

    // Wait for the UI update to happen before completing the deferral. That way, we can be certain that
    // the next RangesDownloaded event will not get triggered midway through the UI update.
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, sender, message, deferral]()
    {
        DownloadedRangesText->Text = FormatDownloadedRanges(sender->GetDownloadedRanges());
        rootPage->NotifyUser(message, NotifyType::StatusMessage);
        deferral->Complete();
    }));
}

void Scenario5_RandomAccess::StartDownload_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Reset the output whenever a new download attempt begins.
    DownloadedInfoText->Text = "";
    DownloadedStatusText->Text = "";
    DownloadedRangesText->Text = "None";
    PreviousReadText->Text = "";
    CurrentReadText->Text = "";

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

        // Apps need to set the IsRandomAccessRequired boolean property to TRUE (default is FALSE) before they
        // call StartAsync() on a DownloadOperation object if they want to use the random access feature. After
        // opting into "random access" mode, GetResultRandomAccessStreamReference() can be called.
        //
        // Setting IsRandomAccessRequired to TRUE before starting the download is necessary because the
        // server-side requirements for Random Access functionality are stricter than for regular (sequential)
        // downloads, so BackgroundTransfer needs to know the app's intentions in advance. That way, it can fail
        // the transfer up front if the server does not satisfy those additional requirements (e.g., HTTP range
        // header support).
        download->IsRandomAccessRequired = true;

        // Subscribe to RangesDownloaded notifications.
        download->RangesDownloaded += ref new TypedEventHandler<DownloadOperation^, BackgroundTransferRangesDownloadedEventArgs^>(this, &Scenario5_RandomAccess::OnRangesDownloaded);

        DownloadedInfoText->Text = "Downloading to " + destinationFile->Name + ", " + download->Guid.ToString();

        // Start the download and read partially-downloaded content while the transfer is still ongoing.
        return StartDownloadAndReadContentsAsync();
    });
}

task<void> Scenario5_RandomAccess::StartDownloadAndReadContentsAsync()
{
    // Retrieve a random access stream from the download operation. Every OpenReadAsync() operation returns
    // a new stream instance that is independent of previous ones (i.e., the seek position of one stream
    // isn't affected by calls on another stream).
    //
    // This sample demonstrates the direct usage of a DownloadOperation's random access stream and its
    // effects on the ongoing transfer. However, bear in mind that there are a variety of operations
    // that can manage the stream on the app's behalf for specific scenarios. For instance, a
    // DownloadOperation pointing to a video URL can be consumed by the MediaPlayer class in four easy
    // steps:
    //
    // auto randomAccessStreamReference = download->GetResultRandomAccessStreamReference();
    // create_task(randomAccessStreamReference->OpenReadAsync()).then([](IRandomAccessStream^ stream)
    // {
    //     auto mediaPlayer = ref new Windows::Media::Playback::MediaPlayer();
    //     mediaPlayer->Source = Windows::Media::Core::MediaSource::CreateFromStream(stream, stream->ContentType);
    // }

    auto randomAccessStreamReference = download->GetResultRandomAccessStreamReference();
    return create_task(randomAccessStreamReference->OpenReadAsync()).then([this](IRandomAccessStreamWithContentType^ stream)
    {
        randomAccessStream = stream;

        // Start the download. If the server doesn't support random access, the download will fail
        // with WebErrorStatus.InsufficientRangeSupport or WebErrorStatus.MissingContentLengthSupport.
        IAsyncOperationWithProgress<DownloadOperation^, DownloadOperation^>^ downloadOperation = download->StartAsync();
        downloadOperation->Progress = ref new AsyncOperationProgressHandler<DownloadOperation^, DownloadOperation^>(this, &Scenario5_RandomAccess::OnDownloadProgress);

        task<DownloadOperation^> downloadTask = create_task(downloadOperation);

        startDownloadButton->IsEnabled = false;
        pauseDownloadButton->IsEnabled = true;

        // Read data while the download is still ongoing. Use a 1 MB read buffer for that purpose.
        auto readBuffer = ref new Buffer(BytesPerMegaByte);

        return ReadStreamAsync(downloadTask, readBuffer).then([this, downloadTask]()
        {
            // Wait for the download to complete.
            return downloadTask;
        });
    }).then([this](DownloadOperation^)
    {
        rootPage->NotifyUser("Download completed successfully", NotifyType::StatusMessage);
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
    }).then([this]()
    {
        download = nullptr;
        randomAccessStream = nullptr;
        readOperation = task<IBuffer^>();

        startDownloadButton->IsEnabled = true;
        pauseDownloadButton->IsEnabled = false;
        resumeDownloadButton->IsEnabled = false;
    });
}

task<void> Scenario5_RandomAccess::ReadStreamAsync(task<DownloadOperation^> downloadTask, Buffer^ readBuffer)
{
    if (downloadTask.is_done())
    {
        return task_from_result();
    }

    uint64_t readOffsetInBytes = randomAccessStream->Position;

    PreviousReadText->Text = CurrentReadText->Text;
    CurrentReadText->Text = "Reading from offset " + readOffsetInBytes.ToString();

    readOperation = create_task(randomAccessStream->ReadAsync(readBuffer, readBuffer->Capacity, InputStreamOptions::None),
        readCancellationTokenSource.get_token());

    // Update the UI to show the current read's position.
    currentPositionSlider->Value = static_cast<double>(readOffsetInBytes / BytesPerMegaByte);

    // Wait for the read to complete.
    return readOperation.then([this](IBuffer^ bytesRead)
    {
        CurrentReadText->Text += ", completed with " + bytesRead->Length.ToString() + " bytes";

        // At this point, a real app would process the 'bytesRead' data to do something interesting
        // with it (e.g., display video and/or play audio).

        if (randomAccessStream->Position >= randomAccessStream->Size)
        {
            // We have reached EOF. Wrap around to the beginning while we wait for the download to complete.
            randomAccessStream->Seek(0);
        }
    }).then([this, downloadTask, readBuffer](task<void> precedingTask)
    {
        try
        {
            precedingTask.get();
        }
        catch (task_canceled&)
        {
            // The ongoing read was canceled by SeekDownload_Click(...) in order for a new download
            // position to take effect immediately.
            CurrentReadText->Text += ", cancelled.";
        }
        return ReadStreamAsync(downloadTask, readBuffer);
    });
}

bool Scenario5_RandomAccess::IsWebException(String^ title, Exception^ ex)
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
