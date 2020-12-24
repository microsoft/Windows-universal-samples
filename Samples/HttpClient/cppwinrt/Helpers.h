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

namespace winrt::SDKTemplate::Helpers
{
    hstring ToSimpleString(Windows::Foundation::DateTime time);

    Windows::Foundation::IAsyncAction DisplayTextResultAsync(
        Windows::Web::Http::HttpResponseMessage response,
        Windows::UI::Xaml::Controls::TextBox output);

    void DisplayWebError(MainPage const& rootPage, hresult const& exception);

    hstring SerializeHeaders(Windows::Web::Http::HttpResponseMessage const& response);

    Windows::Web::Http::HttpClient CreateHttpClientWithCustomHeaders();

    void ScenarioStarted(
        Windows::UI::Xaml::Controls::Button const& startButton,
        Windows::UI::Xaml::Controls::Button const& cancelButton,
        Windows::UI::Xaml::Controls::TextBox const& outputField);

    void ScenarioCompleted(
        Windows::UI::Xaml::Controls::Button const& startButton,
        Windows::UI::Xaml::Controls::Button const& cancelButton);

    void ReplaceQueryString(Windows::UI::Xaml::Controls::TextBox const& addressField, hstring const& newQueryString);
    Windows::Foundation::Uri TryParseHttpUri(hstring const& uriString);
}
