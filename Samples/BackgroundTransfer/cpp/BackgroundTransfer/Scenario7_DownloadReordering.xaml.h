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
#include "Scenario7_DownloadReordering.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    // Class for each download and its row in the UI.
    public ref class DownloadOperationItem sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        DownloadOperationItem(Windows::Networking::BackgroundTransfer::DownloadOperation^ download);

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

        property Windows::Networking::BackgroundTransfer::DownloadOperation^ download
        {
            Windows::Networking::BackgroundTransfer::DownloadOperation^ get();
        }

        property int percentComplete
        {
            int get();
            void set(int value);
        }

        property Platform::String^ stateText
        {
            Platform::String^ get();
            void set(Platform::String^ value);
        }

    private:
        Windows::Networking::BackgroundTransfer::DownloadOperation^ _download;
        int _percentComplete;
        Platform::String^ _stateText;
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario7_DownloadReordering sealed
    {
    public:
        Scenario7_DownloadReordering();

        property Windows::Foundation::Collections::IObservableVector<DownloadOperationItem^>^ downloadCollection
        {
            Windows::Foundation::Collections::IObservableVector<DownloadOperationItem^>^ get();
        }
    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        Concurrency::task<std::vector<Windows::Networking::BackgroundTransfer::DownloadOperation^>>
            CancelActiveDownloadsAsync();
        void StartDownload_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void MakeCurrent_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs e);
        Concurrency::task<void> RunDownloadsAsync();
        Concurrency::task<Windows::Networking::BackgroundTransfer::DownloadOperation^> CreateDownloadAsync(
            Windows::Networking::BackgroundTransfer::BackgroundDownloader^ downloader,
            Platform::String^ remoteAddress,
            Platform::String^ fileName);
        Concurrency::task<void> DownloadAsync(DownloadOperationItem^ item);
        void DownloadProgress(DownloadOperationItem^ item);
        bool Scenario7_DownloadReordering::IsWebException(
            Platform::String^ title,
            Platform::Exception^ ex,
            Windows::Networking::BackgroundTransfer::DownloadOperation^ download = nullptr);

        const int NumDownloads = 5;

        MainPage^ rootPage;
        Windows::Networking::BackgroundTransfer::BackgroundTransferGroup^ reorderGroup;
        Windows::Foundation::Collections::IObservableVector<DownloadOperationItem^>^ _downloadCollection;
    };
}
