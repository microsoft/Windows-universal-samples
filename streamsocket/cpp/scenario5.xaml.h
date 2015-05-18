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
// Scenario5.xaml.h
// Declaration of the Scenario5 class
//

#pragma once

#include "pch.h"
#include "Scenario5.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    namespace StreamSocketSample
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario5 sealed
        {
        public:
            Scenario5();

        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        private:
            static const int continueButtonId = 1;
            static const int abortButtonId = 0;
            MainPage^ rootPage;
            void ConnectSocket_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void ConnectSocketWithRetry(Windows::Networking::HostName^ hostName, Platform::String^ serviceName);
            concurrency::task<void> ConnectSocketWithRetryHandleSslErrorAsync(
                Windows::Networking::Sockets::StreamSocket^ socket,
                Windows::Networking::HostName^ hostName,
                Platform::String^ serviceName);
            concurrency::task<bool> ShouldIgnoreCertificateErrorsAsync(
                Windows::Foundation::Collections::IVectorView<
                Windows::Security::Cryptography::Certificates::ChainValidationResult>^ serverCertificateErrors);
            Platform::String^ GetCertificateInformation(
                Windows::Security::Cryptography::Certificates::Certificate^ serverCert,
                Windows::Foundation::Collections::IVectorView<
                Windows::Security::Cryptography::Certificates::Certificate^>^ intermediateCertificates);
        };
    }
}
