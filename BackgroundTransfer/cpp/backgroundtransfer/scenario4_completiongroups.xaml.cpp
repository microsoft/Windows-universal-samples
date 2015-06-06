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
#include "Scenario4_CompletionGroups.xaml.h"

using namespace BackgroundTransfer;

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace std;
using namespace Tasks;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::BackgroundTransfer;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario4_CompletionGroups::Scenario4_CompletionGroups()
{
    InitializeComponent();
}

void Scenario4_CompletionGroups::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    AttachDownloads();
}

void Scenario4_CompletionGroups::AttachDownloads()
{
    // You may (but are not required to) use transfer groups along with completion groups.
    // Note that we only enumerate transfers that belong to this sample scenario.
    BackgroundTransferGroup^ transferGroup = CompletionGroupTask::CreateTransferGroup();
    create_task(BackgroundDownloader::GetCurrentDownloadsForTransferGroupAsync(transferGroup))
        .then([this](IVectorView<DownloadOperation^>^ downloads)
    {
        if (downloads->Size > 0)
        {
            for (unsigned int i = 0; i < downloads->Size; i++)
            {
                create_task(downloads->GetAt(i)->AttachAsync()).then([this](task<DownloadOperation^> previousTask)
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

            MarshalNotifyUser(downloads->Size.ToString() + " downloads attached.", NotifyType::StatusMessage);
        }
    }).then([this](task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            MarshalNotifyUser("Error discovering downloads: " + ex->Message, NotifyType::ErrorMessage);
        }
    });
}

void Scenario4_CompletionGroups::StartDownloadsButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Clear the status block.
    rootPage->NotifyUser("", NotifyType::StatusMessage);

    BackgroundDownloader^ downloader = CompletionGroupTask::CreateBackgroundDownloader();

    vector<task<void>> postCreateFileTasks;

    for (int i = 0; i < 10; i++)
    {
        String^ sourceString = serverAddressField->Text + "?id=" + i.ToString();

        Uri^ source;
        if (!rootPage->TryGetUri(sourceString, &source))
        {
            rootPage->NotifyUser("Unable to create the source URI.", NotifyType::ErrorMessage);
            return;
        }

        task<StorageFile^> createFileTask = create_task(KnownFolders::PicturesLibrary->CreateFileAsync(
            "picture.png",
            CreationCollisionOption::GenerateUniqueName));

        task<void> postCreateFileTask = createFileTask.then([this, downloader, source](task<StorageFile^> createFileTask)
        {
            StorageFile^ destinationFile = nullptr;
            try
            {
                destinationFile = createFileTask.get();
            }
            catch (Exception^ ex)
            {
                MarshalNotifyUser("Error creating file: " + ex->Message, NotifyType::ErrorMessage);
            }

            if (destinationFile != nullptr)
            {
                StartDownload(destinationFile, downloader, source);
            }
        });

        postCreateFileTasks.push_back(postCreateFileTask);
    }


    when_all(postCreateFileTasks.begin(), postCreateFileTasks.end()).then([this, downloader]()
    {
        // The completion group must be enabled after all the downloads in the completion group are created.
        // Otherwise, if a download was allowed to be created within the completion group after the
        // completion group was enabled, it could happen that all downloads created before already completed
        // and completion group task was already triggered. It does not matter whether a completion group is
        // enabled before or after the calls to StartAsync().
        downloader->CompletionGroup->Enable();

        MarshalNotifyUser("Completion group enabled.", NotifyType::StatusMessage);
    });
}

void Scenario4_CompletionGroups::StartDownload(StorageFile^ destinationFile, BackgroundDownloader^ downloader, Uri^ source)
{
    DownloadOperation^ download = downloader->CreateDownload(source, destinationFile);

    create_task(download->StartAsync()).then([](task<DownloadOperation^> previousTask)
    {
        try
        {
            // Completion group tasks happen in addition to, not instead of, a
            // DownloadOperation's normal StartAsync and AttachAsync continuations. Even if you
            // are using a completion group, you should still use task.then() to attach a
            // completion handler to each transfer you create in your main application process.
            //
            // Even if a completion group is associated with multiple DownloadOperations, each
            // operation's StartAsync and AttachAsync continuations will be invoked individually.
            // The individual continuations execute independently of the completion group's
            // background task.They may occur before, after, or concurrently with each other.

            // Check if there is any error.
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            // It is required to handle exceptions. Otherwise exceptions bubble up
            // and the application may crash.
        }
    });
}

// When operations happen on a background thread we have to marshal UI updates back to the UI thread.
void Scenario4_CompletionGroups::MarshalNotifyUser(String^ message, NotifyType type)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type]()
    {
        rootPage->NotifyUser(message, type);
    }));
}
