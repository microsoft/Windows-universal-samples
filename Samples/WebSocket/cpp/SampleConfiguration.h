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

#include <collection.h>

namespace SDKTemplate
{
    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };

    partial ref class MainPage
    {
    public:
        static property Platform::String^ FEATURE_NAME
        {
            Platform::String^ get()
            {
                return "WebSocket C++ Sample";
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

    internal:
        Windows::Foundation::Uri^ TryGetUri(Platform::String^ uriString);
        static Platform::String^ BuildWebSocketError(Platform::Exception^ ex);
        static concurrency::task<bool> AreCertificateAndCertChainValidAsync(
            Windows::Security::Cryptography::Certificates::Certificate^ serverCert,
            Windows::Foundation::Collections::IVectorView<Windows::Security::Cryptography::Certificates::Certificate^>^ certChain);

    private:
        static Platform::Array<Scenario>^ scenariosInner;

        static concurrency::task<bool> IsCertificateValidAsync(Windows::Security::Cryptography::Certificates::Certificate^ serverCert);
    };
}
