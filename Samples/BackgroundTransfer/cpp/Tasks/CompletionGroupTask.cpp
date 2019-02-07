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

using namespace Concurrency;
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
    // The background task carries out asynchronous work (in RetryDownloadsAsync). Take a deferral
    // to delay the background task from closing prematurely while the asynchronous code is still running.
    Platform::Agile<BackgroundTaskDeferral> deferral(taskInstance->GetDeferral());

    BackgroundTransferCompletionGroupTriggerDetails^ details =
        dynamic_cast<BackgroundTransferCompletionGroupTriggerDetails^>(taskInstance->TriggerDetails);

    if (details == nullptr)
    {
        // We should never arrive here, otherwise, it means this background task was registered
        // for a type of trigger different from BackgroundTransferCompletionGroupTrigger.
        return;
    }

    Vector<DownloadOperation^>^ failedDownloads = ref new Vector<DownloadOperation^>();
    int succeeded = 0;
    int failed = 0;
    for (DownloadOperation^ download : details->Downloads)
    {
        if (IsFailed(download))
        {
            failedDownloads->Append(download);
        }
        else
        {
            succeeded++;
        }
    }

    InvokeSimpleToast(succeeded, failedDownloads->Size);

    if (failedDownloads->Size > 0)
    {
        RetryDownloadsAsync(failedDownloads).then([deferral]()
        {
            deferral->Complete();
        });
    }
    else
    {
        deferral->Complete();
    }
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

    // The system automatically unregisters the BackgroundTransferCompletionGroup task when it triggers.
    // You do not need to unregister it explicitly.
    BackgroundTaskRegistration^ taskRegistration = builder->Register();

    BackgroundDownloader^ downloader = ref new BackgroundDownloader(completionGroup);

    // You may (but are not required to) use transfer groups along with completion groups.
    downloader->TransferGroup = CreateTransferGroup();

    return downloader;
}


bool CompletionGroupTask::IsFailed(DownloadOperation^ download)
{
    BackgroundTransferStatus status = download->Progress.Status;
    if (status == BackgroundTransferStatus::Error || status == BackgroundTransferStatus::Canceled)
    {
        return true;
    }

    ResponseInformation^ response = download->GetResponseInformation();
    if (response == nullptr || response->StatusCode != 200)
    {
        return true;
    }

    return false;
}

task<void> CompletionGroupTask::RetryDownloadsAsync(IVector<DownloadOperation^>^ downloadsToRetry)
{
    BackgroundDownloader^ downloader = CompletionGroupTask::CreateBackgroundDownloader();
    std::vector<task<void>> postRenameTasks;

    for (DownloadOperation^ downloadToRetry : downloadsToRetry)
    {
        // Retry with the same uri, but save to a different file name.
        std::wstring originalName(downloadToRetry->ResultFile->Name->Data());
        std::wstring newName = originalName.insert(originalName.find_last_of(L'.'), L"_retried");

        auto renameTask = create_task(downloadToRetry->ResultFile->RenameAsync(
            ref new String(newName.c_str()),
            Windows::Storage::NameCollisionOption::ReplaceExisting));

        task<void> postRenameTask = renameTask.then([downloadToRetry, downloader]()
        {
            DownloadOperation^ download = downloader->CreateDownload(downloadToRetry->RequestedUri, downloadToRetry->ResultFile);

            // We do not wait on background transfer asynchronous tasks, since wait will only 
            // finish after the entire transfer is complete, which may take a very long time. 
            // The completion of these downloads will be handled by a future CompletionGroupTask instance.
            auto ignore = download->StartAsync();
        });

        postRenameTasks.push_back(postRenameTask);
    }

    return when_all(postRenameTasks.begin(), postRenameTasks.end()).then([downloader]()
    {
        downloader->CompletionGroup->Enable();
    });
}

void CompletionGroupTask::InvokeSimpleToast(int succeeded, int failed)
{
    XmlDocument^ toastXml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastText04);

    XmlNodeList^ stringElements = toastXml->GetElementsByTagName("text");
    stringElements->Item(0)->AppendChild(toastXml->CreateTextNode(succeeded.ToString() + " downloads succeeded."));
    stringElements->Item(1)->AppendChild(toastXml->CreateTextNode(failed.ToString() + " downloads failed."));

    ToastNotification^ toast = ref new ToastNotification(toastXml);
    ToastNotificationManager::CreateToastNotifier()->Show(toast);
}
