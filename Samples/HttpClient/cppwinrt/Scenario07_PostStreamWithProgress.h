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

#include "Scenario07_PostStreamWithProgress.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario07_PostStreamWithProgress : Scenario07_PostStreamWithProgressT<Scenario07_PostStreamWithProgress>
    {
        Scenario07_PostStreamWithProgress();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void ChunkedResponseToggle_Toggled(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget Start_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void Cancel_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Web::Http::HttpClient httpClient{ nullptr };
        Windows::Foundation::IAsyncAction pendingAction{ nullptr };

        void UpdateAddressField();
        void ResetFields();

        Windows::Foundation::IAsyncAction CancelableRequestAsync(
            Windows::Foundation::Uri resourceUri,
            Windows::Web::Http::IHttpContent content);
        fire_and_forget ProgressHandler(
            Windows::Foundation::IAsyncOperationWithProgress<Windows::Web::Http::HttpRequestResult, Windows::Web::Http::HttpProgress> const& sender,
            Windows::Web::Http::HttpProgress progress);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario07_PostStreamWithProgress : Scenario07_PostStreamWithProgressT<Scenario07_PostStreamWithProgress, implementation::Scenario07_PostStreamWithProgress>
    {
    };
}
