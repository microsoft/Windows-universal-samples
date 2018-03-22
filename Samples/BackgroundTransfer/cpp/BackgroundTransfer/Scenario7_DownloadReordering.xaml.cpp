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
#include "Scenario7_DownloadReordering.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Windows::Networking;
using namespace Windows::Networking::BackgroundTransfer;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Windows::Web;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Core;
using namespace Windows::Storage;
using namespace Platform::Collections;
using namespace Platform;

DownloadOperationItem::DownloadOperationItem(DownloadOperation^ download)
{
    _download = download;
    _percentComplete = 0;
    _stateText = "Idle";
}

DownloadOperation^ DownloadOperationItem::download::get()
{
    return _download;
}

int DownloadOperationItem::percentComplete::get()
{
    return _percentComplete;
}

void DownloadOperationItem::percentComplete::set(int value)
{
    if (_percentComplete != value)
    {
        _percentComplete = value;
        PropertyChanged(this, ref new PropertyChangedEventArgs("percentComplete"));
    }
}

String^ DownloadOperationItem::stateText::get()
{
    return _stateText;
}

void DownloadOperationItem::stateText::set(String^ value)
{
    if (_stateText != value)
    {
        _stateText = value;
        PropertyChanged(this, ref new PropertyChangedEventArgs("stateText"));
    }
}

Scenario7_DownloadReordering::Scenario7_DownloadReordering()
{
    reorderGroup = BackgroundTransferGroup::CreateGroup("{7421B969-18D4-4532-B6BD-22BDABF71C08}");
    reorderGroup->TransferBehavior = BackgroundTransferBehavior::Serialized;
    _downloadCollection = ref new Vector<DownloadOperationItem^>();

    DataContext = this;
    InitializeComponent();
}

IObservableVector<DownloadOperationItem^>^ Scenario7_DownloadReordering::downloadCollection::get()
{
    return _downloadCollection;
}

void Scenario7_DownloadReordering::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage = MainPage::Current;
    CancelActiveDownloadsAsync().then([this](task<std::vector<DownloadOperation^>> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            if (!IsWebException("Discovery error", ex))
            {
                throw;
            }
        }
    });
}

task<std::vector<DownloadOperation^>> Scenario7_DownloadReordering::CancelActiveDownloadsAsync()
{
    // Only the downloads that belong to the transfer group used by this sample scenario will be
    // canceled.
    return create_task(BackgroundDownloader::GetCurrentDownloadsForTransferGroupAsync(reorderGroup))
        .then([this](IVectorView<DownloadOperation^>^ downloads)
    {
        std::vector<task<DownloadOperation^>> tasks;

        // If previous instances of this scenario started transfers that haven't completed yet,
        // cancel them now so that we can start this scenario cleanly.
        if (downloads->Size > 0)
        {
            cancellation_token_source cancellationToken;

            for (DownloadOperation^ download : downloads) {
                tasks.push_back(create_task(download->AttachAsync(), cancellationToken.get_token()));
            }

            cancellationToken.cancel();
        }

        return when_all(tasks.begin(), tasks.end());
    });
}

void Scenario7_DownloadReordering::StartDownload_Click(Object^ sender, RoutedEventArgs^ e)
{
    Button^ startButton = safe_cast<Button^>(sender);
    startButton->IsEnabled = false;

    // Create and start downloads.
    RunDownloadsAsync().then([this, startButton]()
    {
        // After all downloads are complete, let the user start new ones again.
        startButton->IsEnabled = true;
    }, task_continuation_context::use_current());
}

void Scenario7_DownloadReordering::MakeCurrent_Click(Object^ sender, RoutedEventArgs e)
{
    Button^ button = safe_cast<Button^>(sender);
    DownloadOperationItem^ item = safe_cast<DownloadOperationItem^>(button->DataContext);

    // Make the selected operation current.
    item->download->MakeCurrentInTransferGroup();
}

task<void> Scenario7_DownloadReordering::RunDownloadsAsync()
{
    // Create a downloader and associate all its downloads with the transfer group used for this
    // scenario.
    BackgroundDownloader^ downloader = ref new BackgroundDownloader();
    downloader->TransferGroup = reorderGroup;

    // Validating the URI is required since it was received from an untrusted source (user input).
    // The URI is validated by calling rootPage->TryGetUri() that will return 'false' for strings
    // that are not valid URIs.
    // Note that when enabling the text box users may provide URIs to machines on the intranet that
    // require the "Private Networks (Client and Server)" capability.
    String^ remoteAddress = StringTrimmer::Trim(remoteAddressField->Text);
    Uri^ tmpUri;
    if (!rootPage->TryGetUri(remoteAddress, &tmpUri))
    {
        return task_from_result();
    }

    String^ fileName = StringTrimmer::Trim(fileNameField->Text);
    if (fileName->IsEmpty())
    {
        rootPage->NotifyUser("A local file name is required.", NotifyType::ErrorMessage);
        return task_from_result();
    }

    // Try to create some downloads.
    std::vector<task<DownloadOperation^>> createDownloadTasks;
    for (int i = 0; i < NumDownloads; i++)
    {
        String^ currRemoteAddress = remoteAddress + "?id=" + i.ToString();
        String^ currFileName = i.ToString() + "." + fileName;
        createDownloadTasks.push_back(CreateDownloadAsync(downloader, currRemoteAddress, currFileName));
    }

    return when_all(createDownloadTasks.begin(), createDownloadTasks.end())
        .then([this](std::vector<DownloadOperation^> downloads)
    {
        // Once all downloads have been created, start them.
        _downloadCollection->Clear();
        std::vector<task<void>> downloadTasks;
        for (DownloadOperation^ download : downloads)
        {
            DownloadOperationItem^ item = ref new DownloadOperationItem(download);
            downloadTasks.push_back(DownloadAsync(item));
            _downloadCollection->Append(item);
        }
        return when_all(downloadTasks.begin(), downloadTasks.end());
    });
}

task<DownloadOperation^> Scenario7_DownloadReordering::CreateDownloadAsync(
    BackgroundDownloader^ downloader,
    String^ remoteAddress,
    String^ fileName)
{
    return create_task(KnownFolders::PicturesLibrary->CreateFileAsync(
            fileName,
            CreationCollisionOption::GenerateUniqueName))
        .then([this, downloader, remoteAddress](StorageFile^ destinationFile)
    {
        Uri^ source = ref new Uri(remoteAddress);
        return downloader->CreateDownload(source, destinationFile);
    });
}


task<void> Scenario7_DownloadReordering::DownloadAsync(DownloadOperationItem^ item)
{
    IAsyncOperationWithProgress<DownloadOperation^, DownloadOperation^>^ async = item->download->StartAsync();
    async->Progress = ref new AsyncOperationProgressHandler<DownloadOperation^, DownloadOperation^>(
        [this, item](
            IAsyncOperationWithProgress<DownloadOperation^, DownloadOperation^>^ operation,
            DownloadOperation^ download)
    {
        DownloadProgress(item);
    });
    return create_task(async).then([this, item](DownloadOperation^ download)
    {
        item->stateText = item->download->Progress.Status.ToString();
        rootPage->NotifyUser(
            "Downloading " + item->download->ResultFile->Name + " completed.",
            NotifyType::StatusMessage);
    }).then([this, item](task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            // Ignore canceled downloads since they are not displayed.
            if (item->download->Progress.Status != BackgroundTransferStatus::Canceled)
            {
                // Ensure that we reach 100% even for errors.
                item->percentComplete = 100;
                item->stateText = item->download->Progress.Status.ToString();
                if (!IsWebException("Execution error", ex, item->download))
                {
                    throw;
                }
            }
        }
    });
}

void Scenario7_DownloadReordering::DownloadProgress(DownloadOperationItem^ item)
{
    BackgroundDownloadProgress currentProgress = item->download->Progress;
    BackgroundTransferStatus status = currentProgress.Status;
    int percentComplete = 0;

    if (currentProgress.TotalBytesToReceive > 0)
    {
        percentComplete = (int)((currentProgress.BytesReceived * 100) / currentProgress.TotalBytesToReceive);
    }

    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
        [this, item, status, percentComplete]()
    {
        item->stateText = status.ToString();
        item->percentComplete = percentComplete;
    }));
}

bool Scenario7_DownloadReordering::IsWebException(String^ title, Exception^ ex, DownloadOperation^ download)
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
        rootPage->NotifyUser(download->ResultFile->Name + " - " + title + ": " + message, NotifyType::ErrorMessage);
    }

    return result;
}
