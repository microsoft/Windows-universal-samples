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
// Scenario3_Notifications.xaml.h
// Declaration of the Scenario3_Notifications class
//

#pragma once

#include "pch.h"
#include "Scenario3_Notifications.g.h"
#include "MainPage.xaml.h"

namespace BackgroundTransfer
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_Notifications sealed
    {
    public:
        Scenario3_Notifications();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        Windows::Networking::BackgroundTransfer::BackgroundTransferGroup^ notificationsGroup;

        static UINT32 runId;

        static const UINT64 tenMinutesIn100NanoSecondUnits = 6000000000;

        enum ScenarioType
        {
            Toast,
            Tile
        };

        void ToastNotificationButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void TileNotificationButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void RunDownloads(
            Windows::Networking::BackgroundTransfer::BackgroundDownloader^ downloader,
            ScenarioType type);
        bool TryCreateDownloadAsync(
            Windows::Networking::BackgroundTransfer::BackgroundDownloader^ downloader, 
            UINT8 delaySeconds, 
            Platform::String^ fileNameSuffix, 
            ScenarioType type,
            Platform::Collections::Vector<Windows::Networking::BackgroundTransfer::DownloadOperation^>^ downloadOperations,
            Concurrency::task<void>& resultTask);
        void RunDownload(Windows::Networking::BackgroundTransfer::DownloadOperation^ download);
        void CancelActiveDownloads();
        void LogException(
            Platform::String^ title, 
            Platform::Exception^ ex, 
            Windows::Networking::BackgroundTransfer::DownloadOperation^ download = nullptr);
        void Log(Platform::String^ message);
        void LogStatus(Platform::String^ message, SDKTemplate::NotifyType type);

        ~Scenario3_Notifications();
    };
}
