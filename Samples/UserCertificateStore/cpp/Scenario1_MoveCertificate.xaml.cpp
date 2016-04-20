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
#include "pch.h"
#include "Scenario1_MoveCertificate.xaml.h"

using namespace concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Cryptography::Certificates;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_MoveCertificate::Scenario1_MoveCertificate() :
    appStore(CertificateStores::GetStoreByName(StandardCertificateStoreNames::Personal)),
    userStore(CertificateStores::GetUserStoreByName(StandardCertificateStoreNames::Personal))
{
    InitializeComponent();
}

void Scenario1_MoveCertificate::OnNavigatedTo(NavigationEventArgs^ e)
{
    create_task(CertificateStores::FindAllAsync()).then([=](IVectorView<Certificate^>^ certs)
    {
        for (Certificate^ cert : certs)
        {
            String^ serialNumber = CryptographicBuffer::EncodeToHexString(CryptographicBuffer::CreateFromByteArray(cert->SerialNumber));
            auto item = ref new CertificateItem("Subject: " + cert->Subject + ", Serial Number: " + serialNumber, cert);

            if (cert->IsPerUser)
            {
                UserCertificateListView->Items->Append(item);
            }
            else
            {
                AppCertificateListView->Items->Append(item);
            }
        }
    });
}

void Scenario1_MoveCertificate::MoveUserCertificateToAppCertificateStore()
{
    auto item = dynamic_cast<CertificateItem^>(UserCertificateListView->SelectedItem);
    if (item != nullptr)
    {
        appStore->Add(item->Certificate);
        create_task(userStore->RequestDeleteAsync(item->Certificate)).then([=](boolean succeeded)
        {
            if (succeeded)
            {
                appStore->Add(item->Certificate);

                unsigned int index;
                if (UserCertificateListView->Items->IndexOf(item, &index))
                {
                    UserCertificateListView->Items->RemoveAt(index);
                }

                AppCertificateListView->Items->Append(item);
                AppCertificateListView->SelectedItem = item;
                AppCertificateListView->ScrollIntoView(item);
            }
            else
            {
                appStore->Delete(item->Certificate);
            }
        });
    }
}

void Scenario1_MoveCertificate::MoveAppCertificateToUserCertificateStore()
{
    auto item = dynamic_cast<CertificateItem^>(AppCertificateListView->SelectedItem);
    if (item != nullptr)
    {
        create_task(userStore->RequestAddAsync(item->Certificate)).then([=](bool succeeded)
        {
            if (succeeded)
            {
                appStore->Delete(item->Certificate);
                unsigned int index;
                if (AppCertificateListView->Items->IndexOf(item, &index))
                {
                    AppCertificateListView->Items->RemoveAt(index);
                }

                UserCertificateListView->Items->Append(item);
                UserCertificateListView->SelectedItem = item;
                UserCertificateListView->ScrollIntoView(item);

            }
        });
    }
}

