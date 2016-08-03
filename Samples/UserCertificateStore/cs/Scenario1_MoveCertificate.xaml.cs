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

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Security.Cryptography.Certificates;
using Windows.Security.Cryptography;


namespace SDKTemplate
{
    // These items are shown in the ListView controls.
    class CertificateItem
    {
        public string Name { get; set; }
        public Certificate Certificate { get; set; }
    }

    public sealed partial class Scenario1_MoveCertificate : Page
    {
        CertificateStore appStore = CertificateStores.GetStoreByName(StandardCertificateStoreNames.Personal);
        UserCertificateStore userStore = CertificateStores.GetUserStoreByName(StandardCertificateStoreNames.Personal);

        public Scenario1_MoveCertificate()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            IReadOnlyList<Certificate> certs = await CertificateStores.FindAllAsync();

            foreach (Certificate cert in certs)
            {
                var item = new CertificateItem
                {
                    Name = $"Subject: {cert.Subject}, Serial Number: {CryptographicBuffer.EncodeToHexString(CryptographicBuffer.CreateFromByteArray(cert.SerialNumber))}",
                    Certificate = cert
                };

                if (cert.IsPerUser)
                {
                    UserCertificateListView.Items.Add(item);
                }
                else
                {
                    AppCertificateListView.Items.Add(item);
                }
            }
        }

        private async void MoveUserCertificateToAppCertificateStore()
        {
            var item = (CertificateItem)UserCertificateListView.SelectedItem;
            if (item != null)
            {
                appStore.Add(item.Certificate);

                if (await userStore.RequestDeleteAsync(item.Certificate))
                {
                    UserCertificateListView.Items.Remove(item);

                    AppCertificateListView.Items.Add(item);
                    AppCertificateListView.SelectedItem = item;
                    AppCertificateListView.ScrollIntoView(item);
                }
                else
                {
                    appStore.Delete(item.Certificate);
                }
            }
        }

        private async void MoveAppCertificateToUserCertificateStore()
        {
            var item = (CertificateItem)AppCertificateListView.SelectedItem;
            if (item != null)
            {
                if (await userStore.RequestAddAsync(item.Certificate))
                {
                    appStore.Delete(item.Certificate);
                    AppCertificateListView.Items.Remove(item);

                    UserCertificateListView.Items.Add(item);
                    UserCertificateListView.SelectedItem = item;
                    UserCertificateListView.ScrollIntoView(item);
                }
            }
        }
    }
}
