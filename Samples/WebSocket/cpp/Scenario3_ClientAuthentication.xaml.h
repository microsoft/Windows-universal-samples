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
// Scenario1.xaml.h
// Declaration of the Scenario1 class
//

#pragma once

#include "pch.h"
#include "Scenario3_ClientAuthentication.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3 sealed
    {
    public:
        Scenario3();

    protected:
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        void OnConnect();
        void OnDisconnect();

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::Networking::Sockets::StreamWebSocket^ streamWebSocket;
        bool busy = false;

        void UpdateVisualState();
        void SetBusy(bool value);
        Concurrency::task<Windows::Security::Cryptography::Certificates::Certificate^> FindCertificateFromStoreAsync();
        Concurrency::task<Windows::Security::Cryptography::Certificates::Certificate^> InstallClientCertificateAsync();
        Concurrency::task<Windows::Security::Cryptography::Certificates::Certificate^> GetClientCertificateAsync();
        Concurrency::task<void> ConnectWebSocketAsync();
        void OnClosed(Windows::Networking::Sockets::IWebSocket^ sender, Windows::Networking::Sockets::WebSocketClosedEventArgs^ args);
        void CloseSocket();
        void AppendOutputLine(Platform::String^ value);
    };
}
