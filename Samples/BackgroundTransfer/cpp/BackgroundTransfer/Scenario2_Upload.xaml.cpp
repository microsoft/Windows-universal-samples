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
// Scenario2_Upload.xaml.cpp
// Implementation of the Scenario2_Upload class
//

#include "pch.h"
#include "Scenario2_Upload.xaml.h"

using namespace BackgroundTransfer;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace SDKTemplate;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Networking::BackgroundTransfer;
using namespace Windows::Web;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::FileProperties;

Scenario2_Upload::Scenario2_Upload()
{
    InitializeComponent();
    cancellationToken = new cancellation_token_source();
}

Scenario2_Upload::~Scenario2_Upload()
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
void Scenario2_Upload::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}


// Enumerate the uploads that were going on in the background while the app was closed.
void Scenario2_Upload::DiscoverActiveUploads()
{
    create_task(BackgroundUploader::GetCurrentUploadsAsync()).then([this] (IVectorView<UploadOperation^>^ uploads)
    {
        Log("Loading background downloads: " + uploads->Size);

        for (unsigned int i = 0; i < uploads->Size; i++)
        {
            UploadOperation^ upload = uploads->GetAt(i);
            Log("Discovered background upload: " + upload->Guid + ", Status: " + upload->Progress.Status.ToString());

            // Attach progress and completion handlers.
            HandleUploadAsync(upload, false);
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

void Scenario2_Upload::StartUpload_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Validating the URI is required since it was received from an untrusted source (user input).
    // The URI is validated by calling TryGetUri() that will return 'false' for strings that are not valid URIs.
    // Note that when enabling the text box users may provide URIs to machines on the intrAnet that require the
    // "Home or Work Networking" capability.
    Uri^ uri;
    if (!rootPage->TryGetUri(serverAddressField->Text, &uri))
    {
        return;
    }

    FileOpenPicker^ picker = ref new FileOpenPicker();
    picker->FileTypeFilter->Append("*");

    create_task(picker->PickSingleFileAsync()).then([this, uri](StorageFile^ file)
    {
        return UploadSingleFileAsync(uri, file);
    }).then([this] (task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            LogException("Upload Error", ex);
        }
        catch (task_canceled&)
        {
            // Do nothing - we canceled the task.
        }
    }, task_continuation_context::use_current());
}

void Scenario2_Upload::StartMultipartUpload_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Validating the URI is required (like TryGetUri) since it was received from an untrusted source (user input).
    // The URI is validated by calling TryGetUri() that will return 'false' for strings that are not valid URIs.
    // Note that when enabling the text box users may provide URIs to machines on the intrAnet that require the
    // "Home or Work Networking" capability.
    Uri^ uri;
    if (!rootPage->TryGetUri(serverAddressField->Text, &uri))
    {
        return;
    }

    FileOpenPicker^ picker = ref new FileOpenPicker();
    picker->FileTypeFilter->Append("*");

    create_task(picker->PickMultipleFilesAsync()).then([this, uri](IVectorView<StorageFile^>^ files)
    {
        return UploadMultipleFilesAsync(uri, files);
    }).then([this] (task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            LogException("Upload Error", ex);
        }
        catch (task_canceled&)
        {
            // Do nothing - we canceled the task.
        }
    }, task_continuation_context::use_current());
}

task<void> Scenario2_Upload::UploadSingleFileAsync(Uri^ uri, IStorageFile^ file)
{
    if (file == nullptr)
    {
        rootPage->NotifyUser("Error: No file selected.", NotifyType::ErrorMessage);
        cancel_current_task();
    }

    return create_task(file->GetBasicPropertiesAsync()).then([this, uri, file](BasicProperties^ properties)
    {
        if (properties->Size > maxUploadFileSize)
        {
            rootPage->NotifyUser(
                "Selected file exceeds max. upload file size (" + (maxUploadFileSize / (1024 * 1024)) + " MB).",
                NotifyType::ErrorMessage);
            cancel_current_task();
        }

        BackgroundUploader^ uploader = ref new BackgroundUploader();
        uploader->SetRequestHeader("Filename", file->Name);

        UploadOperation^ upload = uploader->CreateUpload(uri, file);
        Log("Uploading " + file->Name + " to " + uri->AbsoluteUri + ", " + upload->Guid);

        // Attach progress and completion handlers.
        HandleUploadAsync(upload, true);
    });
}

task<void> Scenario2_Upload::UploadMultipleFilesAsync(Uri^ uri, IVectorView<StorageFile^>^ files)
{
    return create_task(CreatePartsAsync(files)).then(
        [this, uri, files](Vector<BackgroundTransferContentPart^>^ parts)
    {
        BackgroundUploader^ uploader = ref new BackgroundUploader();
        return create_task(uploader->CreateUploadAsync(uri, parts)).then(
            [this, uri, files](UploadOperation^ upload)
        {
            String^ fileNames = files->GetAt(0)->Name;
            for (unsigned int i = 1; i < files->Size; i++)
            {
                fileNames += ", " + files->GetAt(i)->Name;
            }

            Log("Uploading " + fileNames + " to " + uri->AbsoluteUri + ", " + upload->Guid);

            // Attach progress and completion handlers.
            HandleUploadAsync(upload, true);
        });
    });
}

task<Vector<BackgroundTransferContentPart^>^> Scenario2_Upload::CreatePartsAsync(IVectorView<StorageFile^>^ files)
{
    if (files->Size == 0)
    {
        rootPage->NotifyUser("Error: No file selected.", NotifyType::ErrorMessage);
        cancel_current_task();
    }

    std::vector<task<void>> getPropertiesTasks;
    SizeCounter^ sizeCounter = ref new SizeCounter();
    task<void> current;

    // Get file size of all files. If the sum exceeds the max. upload size, fail the operation.
    for (unsigned int i = 0; i < files->Size; i++)
    {
        current = create_task(files->GetAt(i)->GetBasicPropertiesAsync()).then([sizeCounter] (BasicProperties^ properties)
        {
            sizeCounter->Add(properties->Size);
        });

        getPropertiesTasks.push_back(current);
    }

    return when_all(getPropertiesTasks.begin(), getPropertiesTasks.end()).then([this, sizeCounter, files]
    {
        if (sizeCounter->GetSize() > maxUploadFileSize)
        {
            rootPage->NotifyUser(
                "Size of selected files exceeds max. upload file size (" + (maxUploadFileSize / (1024 * 1024)) + " MB).",
                NotifyType::ErrorMessage);
            cancel_current_task();
        }

        Vector<BackgroundTransferContentPart^>^ parts = ref new Vector<BackgroundTransferContentPart^>;
        for (unsigned int i = 0; i < files->Size; i++)
        {
            BackgroundTransferContentPart^ part = ref new BackgroundTransferContentPart("File" + i, files->GetAt(i)->Name);
            part->SetFile(files->GetAt(i));
            parts->Append(part);
        }

        return parts;
    });
}

void Scenario2_Upload::CancelAll_Click(Object^ sender, RoutedEventArgs^ e)
{
    Log("Cancelling all Uploads");

    cancellationToken->cancel();
    cancellationToken = new cancellation_token_source();
}

// Note that this event is invoked on a background thread, so we cannot access the UI directly.
void Scenario2_Upload::UploadProgress(IAsyncOperationWithProgress<UploadOperation^, UploadOperation^>^ operation, UploadOperation^ upload)
{
    // UploadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
    // we must make a local copy so that we can have a consistent view of that ever-changing state
    // throughout this method's lifetime.
    BackgroundUploadProgress currentProgress = upload->Progress;

    MarshalLog("Progress: " + upload->Guid + ", Status: " + currentProgress.Status.ToString());

    UINT64 percentSent = 100;
    if (currentProgress.TotalBytesToSend > 0)
    {
        percentSent = currentProgress.BytesSent * 100 / currentProgress.TotalBytesToSend;
    }

    MarshalLog(" - Sent bytes: " + currentProgress.BytesSent + " of " + currentProgress.TotalBytesToSend +
        " (" + percentSent + "%), Received bytes: " + currentProgress.BytesReceived + " of " +
        currentProgress.TotalBytesToReceive);

    if (currentProgress.HasRestarted)
    {
        MarshalLog(" - Upload restarted");
    }

    if (currentProgress.HasResponseChanged)
    {
        // We've received new response headers from the server.
        MarshalLog(" - Response updated; Header count: " + upload->GetResponseInformation()->Headers->Size);

        // If you want to stream the response data this is a good time to start.
        // upload.GetResultStreamAt(0);
    }
}

void Scenario2_Upload::HandleUploadAsync(UploadOperation^ upload, bool start)
{
    IAsyncOperationWithProgress<UploadOperation^, UploadOperation^>^ async;

    LogStatus("Running: " + upload->Guid, NotifyType::StatusMessage);

    if (start)
    {
        async = upload->StartAsync();
    }
    else
    {
        async = upload->AttachAsync();
    }

    async->Progress = ref new AsyncOperationProgressHandler<UploadOperation^, UploadOperation^>(this, &Scenario2_Upload::UploadProgress);
    create_task(async, cancellationToken->get_token()).then([this] (UploadOperation^ upload)
    {
        ResponseInformation^ response = upload->GetResponseInformation();
        LogStatus("Completed: " + upload->Guid + ", Status Code: " + response->StatusCode,
            NotifyType::StatusMessage);
    }).then([this, upload] (task<void> previousTask)
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
            LogStatus("Canceled: " + upload->Guid, NotifyType::StatusMessage);
        }
    });
}

void Scenario2_Upload::LogException(String^ title, Exception^ ex)
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
void Scenario2_Upload::MarshalLog(String^ value)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, value] ()
    {
        Log(value);
    }));
}

void Scenario2_Upload::Log(String^ message)
{
    outputField->Text += message + "\r\n";
}

void Scenario2_Upload::LogStatus(String^ message, NotifyType type)
{
    rootPage->NotifyUser(message, type);
    outputField->Text += message + "\r\n";
}

Scenario2_Upload::SizeCounter::SizeCounter() : size(0)
{
}

void Scenario2_Upload::SizeCounter::Add(INT64 sizeValue)
{
    // Use interlocked since multiple threads may try to increase the size concurrently.
    InterlockedAdd64(&this->size, sizeValue);
}

INT64 Scenario2_Upload::SizeCounter::GetSize()
{
    return this->size;
}

