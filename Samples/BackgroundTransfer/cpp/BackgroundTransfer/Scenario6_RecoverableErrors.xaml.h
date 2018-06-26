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
#include "Scenario6_RecoverableErrors.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario6_RecoverableErrors sealed
    {
    public:
        Scenario6_RecoverableErrors();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        void StartDownload_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    private:
        MainPage^ rootPage = MainPage::Current;

        // Scenario instance state.
        Windows::Networking::BackgroundTransfer::DownloadOperation^ download;
        Concurrency::cancellation_token_source cancellationTokenSource;

        //
        Concurrency::task<void> CancelActiveDownloadsAsync();
        Concurrency::task<void> HandleDownloadAsync();

        bool IsWebException(Platform::String^ title, Platform::Exception^ ex);

        void OnDownloadProgress(
            Windows::Foundation::IAsyncOperationWithProgress<Windows::Networking::BackgroundTransfer::DownloadOperation^, Windows::Networking::BackgroundTransfer::DownloadOperation^>^ sender,
            Windows::Networking::BackgroundTransfer::DownloadOperation^ progress);
    };
}
