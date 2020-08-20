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
            IReadOnlyList<Certificate> certificates = await CertificateStores.FindAllAsync();

            foreach (Certificate certificate in certificates)
            {
                var item = new ListViewItem
                {
                    Content = $"Subject: {certificate.Subject}, Serial Number: {CryptographicBuffer.EncodeToHexString(CryptographicBuffer.CreateFromByteArray(certificate.SerialNumber))}",
                    Tag = certificate
                };

                if (certificate.IsPerUser)
                {
                    UserCertificateListView.Items.Add(item);
                }
                else
                {
                    AppCertificateListView.Items.Add(item);
                }
            }
        }

        static void MoveItemBetweenListViews(ListViewItem item, ListView source, ListView destination)
        {
            source.Items.Remove(item);

            destination.Items.Add(item);
            destination.SelectedItem = item;
            destination.ScrollIntoView(item);
        }

        private async void MoveUserCertificateToAppCertificateStore(object sender, RoutedEventArgs e)
        {
            var item = (ListViewItem)UserCertificateListView.SelectedItem;
            if (item != null)
            {
                var certificate = (Certificate)item.Tag;
                appStore.Add(certificate);

                if (await userStore.RequestDeleteAsync(certificate))
                {
                    MoveItemBetweenListViews(item, UserCertificateListView, AppCertificateListView);
                }
                else
                {
                    appStore.Delete(certificate);
                }
            }
        }

        private async void MoveAppCertificateToUserCertificateStore(object sender, RoutedEventArgs e)
        {
            var item = (ListViewItem)AppCertificateListView.SelectedItem;
            if (item != null)
            {
                var certificate = (Certificate)item.Tag;
                if (await userStore.RequestAddAsync(certificate))
                {
                    appStore.Delete(certificate);
                    MoveItemBetweenListViews(item, AppCertificateListView, UserCertificateListView);
                }
            }
        }
    }
}
