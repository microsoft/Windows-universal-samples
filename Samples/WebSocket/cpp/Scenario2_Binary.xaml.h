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
// Scenario2.xaml.h
// Declaration of the Scenario2 class
//

#pragma once

#include "pch.h"
#include "Scenario2_Binary.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2 sealed
    {
    public:
        Scenario2();

    protected:
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        void OnStart();
        void OnStop();

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::Networking::Sockets::StreamWebSocket^ streamWebSocket;
        bool busy = false;

        void UpdateVisualState();
        void SetBusy(bool value);
        Concurrency::task<void> StartAsync();
        void OnServerCustomValidationRequested(
            Windows::Networking::Sockets::StreamWebSocket^ sender,
            Windows::Networking::Sockets::WebSocketServerCustomValidationRequestedEventArgs^ args);
        void StartReceiveData(Windows::Networking::Sockets::StreamWebSocket^ activeSocket);
        void ReceiveDataLoop(
            Windows::Networking::Sockets::StreamWebSocket^ activeSocket,
            Windows::Storage::Streams::IBuffer^ readBuffer,
            UINT32 bytesReceivedSoFar);
        void StartSendData(Windows::Networking::Sockets::StreamWebSocket^ activeSocket);
        void SendDataLoop(
            Windows::Networking::Sockets::StreamWebSocket^ activeSocket,
            Windows::Storage::Streams::IBuffer^ sendBuffer,
            UINT32 bytesSentSoFar);

        Concurrency::task<void> StopAsync();
        void OnClosed(Windows::Networking::Sockets::IWebSocket^ sender, Windows::Networking::Sockets::WebSocketClosedEventArgs^ args);
        void CloseSocket();
        void AppendOutputLine(Platform::String^ value);
    };
}
