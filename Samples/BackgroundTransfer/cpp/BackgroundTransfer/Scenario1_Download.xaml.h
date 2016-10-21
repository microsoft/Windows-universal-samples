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
// Scenario1_Download.xaml.h
// Declaration of the Scenario1_Download class
//

#pragma once

#include "pch.h"
#include "Scenario1_Download.g.h"
#include "MainPage.xaml.h"

namespace BackgroundTransfer
{
    struct GuidComparer
    {
        bool operator() (const Platform::Guid&, const Platform::Guid&) const
        {
            return false;
        }
    };

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Download sealed
    {
    public:
        Scenario1_Download();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    
    private:
        SDKTemplate::MainPage^ rootPage;
        Concurrency::cancellation_token_source* cancellationToken;
        std::map<int /*Platform::Guid*/, Windows::Networking::BackgroundTransfer::DownloadOperation^> activeDownloads;
    
        void StartDownload_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StartHighPriorityDownload_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void CancelAll_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResumeAll_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PauseAll_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            
        void HandleDownloadAsync(Windows::Networking::BackgroundTransfer::DownloadOperation^ download, boolean start);
        void DownloadProgress(
            Windows::Foundation::IAsyncOperationWithProgress<Windows::Networking::BackgroundTransfer::DownloadOperation^, Windows::Networking::BackgroundTransfer::DownloadOperation^>^ operation,
            Windows::Networking::BackgroundTransfer::DownloadOperation^);
        void DiscoverActiveDownloads();
        void LogException(Platform::String^ title, Platform::Exception^ ex);
        void MarshalLog(Platform::String^ value);
        void Log(Platform::String^ message);
        void LogStatus(Platform::String^ message, SDKTemplate::NotifyType type);
        void StartDownload(Windows::Networking::BackgroundTransfer::BackgroundTransferPriority priority);

        ~Scenario1_Download();
    };
}
