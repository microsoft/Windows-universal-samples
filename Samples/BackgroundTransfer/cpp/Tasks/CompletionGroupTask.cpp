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
#include "CompletionGroupTask.h"

using namespace Tasks;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::BackgroundTransfer;
using namespace Windows::UI::Notifications;

CompletionGroupTask::CompletionGroupTask()
{
}

// Completion groups allow us to immediately take action after a set of downloads completes.
// In this sample, the server intentionally replies with an error status for some of the downloads.
// Based on the trigger details, we can determine which of the downloads have failed and try them again
// using a new completion group.
//
// Apps generally do not need to implement basic retry logic themselves for normal network errors,
// connectivity loss, etc. because Background Transfer does it internally.
//
// Typically, you would only want to retry transfers from a CompletionGroup task if your app has
// gained new information about how to perform the transfer compared to the first time you started it.
// For example, if the URI you were connecting to expired and you have obtained a new URI from the
// server, or if you would like to try the transfer again with freshly updated credentials.
void CompletionGroupTask::Run(IBackgroundTaskInstance ^ taskInstance)
{
    BackgroundTransferCompletionGroupTriggerDetails^ details =
        dynamic_cast<BackgroundTransferCompletionGroupTriggerDetails^>(taskInstance->TriggerDetails);

    if (details == nullptr)
    {
        // We should never arrive here, otherwise, it means this background task was registered
        // for a type of trigger different from BackgroundTransferCompletionGroupTrigger.
        return;
    }

    Vector<DownloadOperation^>^ badRequestDownloads = ref new Vector<DownloadOperation^>();
    int succeeded = 0;
    int failed = 0;
    IVectorView<DownloadOperation^>^ downloads = details->Downloads;
    for (IIterator<DownloadOperation^>^ iterator = downloads->First(); iterator->HasCurrent; iterator->MoveNext())
    {
        DownloadOperation^ download = iterator->Current;
        if (Succeeded(download))
        {
            succeeded++;
        }
        else if (IsBadRequest(download))
        {
            badRequestDownloads->Append(download);
        }
        else
        {
            failed++;
        }
    }

    if (badRequestDownloads->Size > 0)
    {
        RetryDownloads(badRequestDownloads);
    }

    InvokeSimpleToast(succeeded, badRequestDownloads->Size, failed);
}


BackgroundTransferGroup^ CompletionGroupTask::CreateTransferGroup()
{
    return BackgroundTransferGroup::CreateGroup("Scenario4");
}

BackgroundDownloader^ CompletionGroupTask::CreateBackgroundDownloader()
{
    BackgroundTransferCompletionGroup^ completionGroup = ref new BackgroundTransferCompletionGroup();

    BackgroundTaskBuilder^ builder = ref new BackgroundTaskBuilder();

    // In addition, TaskEntryPoint must be specified in the <Extensions> section of the application's manifest as
    // <Extension Category="windows.backgroundTasks" EntryPoint="Tasks.CompletionGroupTask">
    // For more information see:
    // https://msdn.microsoft.com/en-us/library/windows/apps/windows.applicationmodel.background.backgroundtaskbuilder.taskentrypoint.aspx
    builder->TaskEntryPoint = "Tasks.CompletionGroupTask";

    builder->SetTrigger(completionGroup->Trigger);

    BackgroundTaskRegistration^ taskRegistration = builder->Register();

    BackgroundDownloader^ downloader = ref new BackgroundDownloader(completionGroup);

    // You may (but are not required to) use transfer groups along with completion groups.
    downloader->TransferGroup = CreateTransferGroup();

    return downloader;
}

bool CompletionGroupTask::Succeeded(DownloadOperation^ download)
{
    BackgroundTransferStatus status = download->Progress.Status;
    if (status == BackgroundTransferStatus::Error || status == BackgroundTransferStatus::Canceled)
    {
        return true;
    }

    ResponseInformation^ response = download->GetResponseInformation();
    if (response != nullptr && response->StatusCode == 200)
    {
        return true;
    }

    return false;
}

bool CompletionGroupTask::IsBadRequest(DownloadOperation^ download)
{
    ResponseInformation^ response = download->GetResponseInformation();
    if (response != nullptr && response->StatusCode == 400)
    {
        return true;
    }

    return false;
}

void CompletionGroupTask::RetryDownloads(IIterable<DownloadOperation^>^ downloads)
{
    BackgroundDownloader^ downloader = CompletionGroupTask::CreateBackgroundDownloader();

    for (IIterator<DownloadOperation^>^ iterator = downloads->First(); iterator->HasCurrent; iterator->MoveNext())
    {
        DownloadOperation^ download = iterator->Current;
        DownloadOperation^ download2 = downloader->CreateDownload(download->RequestedUri, download->ResultFile);

        // It is not necessary neither recommended to wait for completion of DownloadOperation::StartAsync() within
        // a background task. Waiting for the completion would mean to wait until the download is complete,
        // but that is not necessary. The download will continue executing after the background task finishes.
        //
        // If the a download is initiated from a background task, you need to call
        // BackgroundDownloader::GetCurrentDownloadsAsync() when the application restarts,
        // as demonstrated in Scenario4_CompletionGroups::AttachDownloads.
        auto ignore = download2->StartAsync();
    }

    downloader->CompletionGroup->Enable();
}

void CompletionGroupTask::InvokeSimpleToast(int succeeded, int badRequests, int failed)
{
    XmlDocument^ toastXml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastText04);

    XmlNodeList^ stringElements = toastXml->GetElementsByTagName("text");
    stringElements->Item(0)->AppendChild(toastXml->CreateTextNode(succeeded.ToString() + " downloads succeeded."));
    stringElements->Item(1)->AppendChild(toastXml->CreateTextNode(badRequests.ToString() + " downloads were bad requests, retrying."));
    stringElements->Item(2)->AppendChild(toastXml->CreateTextNode(failed.ToString() + " downloads failed."));

    ToastNotification^ toast = ref new ToastNotification(toastXml);
    ToastNotificationManager::CreateToastNotifier()->Show(toast);
}
