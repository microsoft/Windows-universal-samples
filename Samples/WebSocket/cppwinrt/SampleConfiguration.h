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

namespace winrt
{
    hstring to_hstring(Windows::Networking::Sockets::SocketMessageType value);
    hstring to_hstring(Windows::Web::WebErrorStatus value);
}

namespace winrt::SDKTemplate
{
    Windows::Foundation::Uri TryGetUri(MainPage const& rootPage, hstring const& uriString);
    hstring BuildWebSocketError(hresult_error const& ex);
    Windows::Foundation::IAsyncOperation<bool> AreCertificateAndCertChainValidAsync(Windows::Security::Cryptography::Certificates::Certificate serverCert, Windows::Foundation::Collections::IVectorView<Windows::Security::Cryptography::Certificates::Certificate> certChain);
}
