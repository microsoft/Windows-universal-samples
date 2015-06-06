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
// Scenario2_Upload.xaml.h
// Declaration of the Scenario2_Upload class
//

#pragma once

#include "pch.h"
#include "Scenario2_Upload.g.h"
#include "MainPage.xaml.h"
#include "Collection.h"

namespace BackgroundTransfer
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Upload sealed
    {
    public:
        Scenario2_Upload();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        Concurrency::cancellation_token_source* cancellationToken;

        static const UINT32 maxUploadFileSize = 100 * 1024 * 1024; // 100 MB (arbitrary limit chosen for this sample)

        ref class SizeCounter sealed
        {
        public:
            SizeCounter();

            void Add(INT64 sizeValue);

            INT64 GetSize();

        private:
            INT64 size;
        };

        void StartUpload_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StartMultipartUpload_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void CancelAll_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DiscoverActiveUploads();
        void UploadProgress(
            Windows::Foundation::IAsyncOperationWithProgress<Windows::Networking::BackgroundTransfer::UploadOperation^, Windows::Networking::BackgroundTransfer::UploadOperation^>^ operation,
            Windows::Networking::BackgroundTransfer::UploadOperation^);
        void HandleUploadAsync(Windows::Networking::BackgroundTransfer::UploadOperation^ upload, bool start);
        void LogException(Platform::String^ title, Platform::Exception^ ex);
        void MarshalLog(Platform::String^ value);
        void Log(Platform::String^ message);
        void LogStatus(Platform::String^ message, SDKTemplate::NotifyType type);

        Concurrency::task<void> UploadSingleFileAsync(Windows::Foundation::Uri^ uri, Windows::Storage::IStorageFile^ file);
        Concurrency::task<void> UploadMultipleFilesAsync(
            Windows::Foundation::Uri^ uri,
            Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile^>^ files);
        Concurrency::task<Platform::Collections::Vector<Windows::Networking::BackgroundTransfer::BackgroundTransferContentPart^>^> CreatePartsAsync(
            Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile^>^ files);

        ~Scenario2_Upload();
    };
}
