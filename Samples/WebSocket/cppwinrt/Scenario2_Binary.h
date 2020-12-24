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

#include "Scenario2_Binary.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_Binary : Scenario2_BinaryT<Scenario2_Binary>
    {
        Scenario2_Binary();

        winrt::fire_and_forget OnStart(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnStop(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

    private:
        Windows::Foundation::IAsyncAction StartAsync();
        Windows::Foundation::IAsyncAction SendDataAsync(Windows::Networking::Sockets::StreamWebSocket activeSocket);
        Windows::Foundation::IAsyncAction ReceiveDataAsync(Windows::Networking::Sockets::StreamWebSocket activeSocket);
        winrt::fire_and_forget OnServerCustomValidationRequested(Windows::Networking::Sockets::StreamWebSocket const&, Windows::Networking::Sockets::WebSocketServerCustomValidationRequestedEventArgs e);
        winrt::fire_and_forget OnClosed(Windows::Networking::Sockets::IWebSocket sender, Windows::Networking::Sockets::WebSocketClosedEventArgs e);
        void CloseSocket();
        void AppendOutputLine(hstring const& value);
        void UpdateVisualState();
        void SetBusy(bool value);

        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };
        Windows::Networking::Sockets::StreamWebSocket m_streamWebSocket{ nullptr };
        bool m_busy{ false };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_Binary : Scenario2_BinaryT<Scenario2_Binary, implementation::Scenario2_Binary>
    {
    };
}
