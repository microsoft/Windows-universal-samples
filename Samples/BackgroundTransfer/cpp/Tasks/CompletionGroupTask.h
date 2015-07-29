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

#pragma once

namespace Tasks
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class CompletionGroupTask sealed : public Windows::ApplicationModel::Background::IBackgroundTask
    {
    public:
        CompletionGroupTask();
        virtual void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);

        static Windows::Networking::BackgroundTransfer::BackgroundTransferGroup^ CreateTransferGroup();
        static Windows::Networking::BackgroundTransfer::BackgroundDownloader^ CreateBackgroundDownloader();

    private:
        bool Succeeded(Windows::Networking::BackgroundTransfer::DownloadOperation^ download);
        bool IsBadRequest(Windows::Networking::BackgroundTransfer::DownloadOperation^ download);
        void RetryDownloads(Windows::Foundation::Collections::IIterable<Windows::Networking::BackgroundTransfer::DownloadOperation^>^ downloads);
        void InvokeSimpleToast(int succeeded, int badRequests, int failed);
    };
}
