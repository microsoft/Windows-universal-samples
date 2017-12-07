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
#include "Scenario5_RandomAccess.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario5_RandomAccess sealed
    {
    public:
        Scenario5_RandomAccess();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        void PauseDownload_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResumeDownload_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SeekDownload_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StartDownload_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    private:
        MainPage^ rootPage = MainPage::Current;

        static constexpr unsigned BytesPerMegaByte = 1000000;

        // Scenario instance state.
        Windows::Networking::BackgroundTransfer::DownloadOperation^ download;
        Windows::Storage::Streams::IRandomAccessStreamWithContentType^ randomAccessStream;
        Concurrency::task<Windows::Storage::Streams::IBuffer^> readOperation;
        Concurrency::cancellation_token_source readCancellationTokenSource;

        static Platform::String^ FormatDownloadedRanges(Windows::Foundation::Collections::IIterable<Windows::Networking::BackgroundTransfer::BackgroundTransferFileRange>^ ranges);

        Concurrency::task<void> CancelActiveDownloadsAsync();
        Concurrency::task<void> StartDownloadAndReadContentsAsync();
        Concurrency::task<void> ReadStreamAsync(
            Concurrency::task<Windows::Networking::BackgroundTransfer::DownloadOperation^> downloadTask,
            Windows::Storage::Streams::Buffer^ readBuffer);

        bool IsWebException(Platform::String^ title, Platform::Exception^ ex);

        void OnDownloadProgress(
            Windows::Foundation::IAsyncOperationWithProgress<Windows::Networking::BackgroundTransfer::DownloadOperation^, Windows::Networking::BackgroundTransfer::DownloadOperation^>^ sender,
            Windows::Networking::BackgroundTransfer::DownloadOperation^ progress);
        void OnRangesDownloaded(
            Windows::Networking::BackgroundTransfer::DownloadOperation^ sender,
            Windows::Networking::BackgroundTransfer::BackgroundTransferRangesDownloadedEventArgs^ args);
    };
}
