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

#include "pch.h"
#include "Scenario4_CompletionGroups.g.h"
#include "MainPage.xaml.h"

namespace BackgroundTransfer
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4_CompletionGroups sealed
    {
    public:
        Scenario4_CompletionGroups();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;

        void AttachDownloads();
        void StartDownloadsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StartDownload(
            Windows::Storage::StorageFile^ destinationFile,
            Windows::Networking::BackgroundTransfer::BackgroundDownloader^ downloader,
            Windows::Foundation::Uri^ source);
        void MarshalNotifyUser(Platform::String^ message, SDKTemplate::NotifyType type);
    };
}
