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
#include "Scenario1_MoveCertificate.h"
#include "Scenario1_MoveCertificate.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Security::Cryptography::Certificates;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_MoveCertificate::Scenario1_MoveCertificate() :
        appStore{ CertificateStores::GetStoreByName(StandardCertificateStoreNames::Personal()) },
        userStore{ CertificateStores::GetUserStoreByName(StandardCertificateStoreNames::Personal()) }
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1_MoveCertificate::OnNavigatedTo(NavigationEventArgs const&)
    {
        auto lifetime = get_strong();

        IVectorView<Certificate> certificates = co_await CertificateStores::FindAllAsync();

        for (Certificate certificate : certificates)
        {
            ListViewItem item;
            hstring serialNumberAsString = CryptographicBuffer::EncodeToHexString(CryptographicBuffer::CreateFromByteArray(certificate.SerialNumber()));
            item.Content(box_value(L"Subject: " + certificate.Subject() + L", Serial number:" + serialNumberAsString));
            item.Tag(certificate);

            if (certificate.IsPerUser())
            {
                UserCertificateListView().Items().Append(item);
            }
            else
            {
                AppCertificateListView().Items().Append(item);
            }
        }
    }

    static void MoveItemBetweenListViews(ListViewItem const& item, ListView const& source, ListView const& destination)
    {
        uint32_t index;
        if (source.Items().IndexOf(item, index))
        {
            source.Items().RemoveAt(index);
        }

        destination.Items().Append(item);
        destination.SelectedItem(item);
        destination.ScrollIntoView(item);
    }

    fire_and_forget Scenario1_MoveCertificate::MoveUserCertificateToAppCertificateStore(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        auto item = UserCertificateListView().SelectedItem().as<ListViewItem>();
        if (item)
        {
            auto certificate = item.Tag().as<Certificate>();
            appStore.Add(certificate);

            if (co_await userStore.RequestDeleteAsync(certificate))
            {
                MoveItemBetweenListViews(item, UserCertificateListView(), AppCertificateListView());
            }
            else
            {
                appStore.Delete(certificate);
            }
        }
    }

    fire_and_forget Scenario1_MoveCertificate::MoveAppCertificateToUserCertificateStore(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        auto item = AppCertificateListView().SelectedItem().as<ListViewItem>();
        if (item)
        {
            auto certificate = item.Tag().as<Certificate>();
            if (co_await userStore.RequestAddAsync(certificate))
            {
                appStore.Delete(certificate);
                MoveItemBetweenListViews(item, AppCertificateListView(), UserCertificateListView());
            }
        }
    }
}
