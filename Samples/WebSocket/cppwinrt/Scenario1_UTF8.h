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

#include "Scenario1_UTF8.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_UTF8 : Scenario1_UTF8T<Scenario1_UTF8>
    {
        Scenario1_UTF8();

        winrt::fire_and_forget OnConnect(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        winrt::fire_and_forget OnSend(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnDisconnect(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

    private:
        Windows::Foundation::IAsyncAction ConnectAsync();
        Windows::Foundation::IAsyncAction SendAsync();
        winrt::fire_and_forget OnServerCustomValidationRequested(Windows::Networking::Sockets::MessageWebSocket const&, Windows::Networking::Sockets::WebSocketServerCustomValidationRequestedEventArgs e);
        winrt::fire_and_forget MessageReceived(Windows::Networking::Sockets::MessageWebSocket const& sender, Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs e);
        winrt::fire_and_forget OnClosed(Windows::Networking::Sockets::IWebSocket sender, Windows::Networking::Sockets::WebSocketClosedEventArgs e);
        void CloseSocket();
        void AppendOutputLine(hstring const& value);
        void UpdateVisualState();
        void SetBusy(bool value);

        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };
        Windows::Networking::Sockets::MessageWebSocket m_messageWebSocket{ nullptr };
        Windows::Storage::Streams::DataWriter m_messageWriter{ nullptr };
        bool m_busy{ false };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_UTF8 : Scenario1_UTF8T<Scenario1_UTF8, implementation::Scenario1_UTF8>
    {
    };
}
