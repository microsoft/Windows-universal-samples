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

#include "Scenario3_ClientAuthentication.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_ClientAuthentication : Scenario3_ClientAuthenticationT<Scenario3_ClientAuthentication>
    {
        Scenario3_ClientAuthentication();

        Windows::Foundation::IAsyncAction OnConnect(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnDisconnect(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

    private:
        Windows::Foundation::IAsyncOperation<Windows::Security::Cryptography::Certificates::Certificate> FindCertificateFromStoreAsync();
        Windows::Foundation::IAsyncOperation<Windows::Security::Cryptography::Certificates::Certificate> InstallClientCertificateAsync();
        Windows::Foundation::IAsyncOperation<Windows::Security::Cryptography::Certificates::Certificate> GetClientCertificateAsync();
        Windows::Foundation::IAsyncAction ConnectWebSocketAsync();
        Windows::Foundation::IAsyncAction OnClosed(Windows::Networking::Sockets::IWebSocket sender, Windows::Networking::Sockets::WebSocketClosedEventArgs e);
        void CloseSocket();
        void AppendOutputLine(hstring const& value);
        void UpdateVisualState();
        void SetBusy(bool value);

        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };
        hstring m_ClientCertUriPath{ L"ms-appx:///Assets/clientCert.pfx" };
        hstring m_ClientCertPassword{ L"1234" };
        hstring m_ClientCertFriendlyName{ L"WebSocketSampleClientCert" };
        hstring m_ClientCertIssuerName{ L"www.contoso.com" };
        Windows::Networking::Sockets::StreamWebSocket m_streamWebSocket{ nullptr };
        bool m_busy{ false };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_ClientAuthentication : Scenario3_ClientAuthenticationT<Scenario3_ClientAuthentication, implementation::Scenario3_ClientAuthentication>
    {
    };
}
