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

#include "Scenario4_PartialReadWrite.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_PartialReadWrite : Scenario4_PartialReadWriteT<Scenario4_PartialReadWrite>
    {
        Scenario4_PartialReadWrite();

        Windows::Foundation::IAsyncAction OnConnect(Windows::Foundation::IInspectable sender, Windows::UI::Xaml::RoutedEventArgs e);
        Windows::Foundation::IAsyncAction OnSend(Windows::Foundation::IInspectable sender, Windows::UI::Xaml::RoutedEventArgs e);
        void OnDisconnect(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        Windows::Foundation::IAsyncAction ConnectAsync();
        Windows::Foundation::IAsyncAction SendAsync();
        Windows::Foundation::IAsyncAction MessageReceived(Windows::Networking::Sockets::MessageWebSocket sender, Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs e);
        Windows::Foundation::IAsyncAction OnClosed(Windows::Networking::Sockets::IWebSocket sender, Windows::Networking::Sockets::WebSocketClosedEventArgs e);
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
    struct Scenario4_PartialReadWrite : Scenario4_PartialReadWriteT<Scenario4_PartialReadWrite, implementation::Scenario4_PartialReadWrite>
    {
    };
}
