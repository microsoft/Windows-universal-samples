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

#include "Scenario1_MoveCertificate.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    // These items are shown in the ListView controls.
    [Windows::UI::Xaml::Data::Bindable]
    public ref class CertificateItem sealed
    {
    public:
        CertificateItem(Platform::String^ name, Windows::Security::Cryptography::Certificates::Certificate^ certificate)
        {
            this->name = name;
            this->certificate = certificate;
        }

        property Platform::String^ Name
        {
            Platform::String^ get()
            {
                return name;
            }
        }

        property Windows::Security::Cryptography::Certificates::Certificate^ Certificate
        {
            Windows::Security::Cryptography::Certificates::Certificate^ get()
            {
                return certificate;
            }
        }

    private:
        Platform::String^ name;
        Windows::Security::Cryptography::Certificates::Certificate^ certificate;
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_MoveCertificate sealed
    {
    public:
        Scenario1_MoveCertificate();
    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void MoveUserCertificateToAppCertificateStore();
        void MoveAppCertificateToUserCertificateStore();

    private:
        Windows::Security::Cryptography::Certificates::CertificateStore^ appStore;
        Windows::Security::Cryptography::Certificates::UserCertificateStore^ userStore;
    };
}
