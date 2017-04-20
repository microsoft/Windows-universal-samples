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
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class ShareCustomProvider
    {
        private DataTransferManager dataTransferManager = DataTransferManager.GetForCurrentView();

        public ShareCustomProvider()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            // Register the event to add a Share Provider
            dataTransferManager.ShareProvidersRequested += OnShareProvidersRequested;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            this.dataTransferManager.ShareProvidersRequested -= OnShareProvidersRequested;
            base.OnNavigatedFrom(e);
        }

        protected override bool GetShareContent(DataRequest request)
        {
            DataPackage requestData = request.Data;
            requestData.Properties.Title = "I got a new high score!";
            requestData.SetText("I got a high score of 15063!");
            return true;
        }

        private void OnShareProvidersRequested(DataTransferManager s, ShareProvidersRequestedEventArgs e)
        {
            // Take a deferral so that we can perform async operations. (This sample
            // doesn't perform any async operations in the ShareProvidersRequested
            // event handler, but we take the deferral anyway to demonstrate the pattern.)
            using (var deferral = e.GetDeferral())
            {
                // Create the custom share provider and add it.
                RandomAccessStreamReference icon = RandomAccessStreamReference.CreateFromUri(new Uri(@"ms-appx:///Assets/windows-sdk.png"));
                ShareProvider provider = new ShareProvider("Contoso Chat", icon, Windows.UI.Colors.LightGray, OnShareToContosoChat);
                e.Providers.Add(provider);
            }
        }

        private async void OnShareToContosoChat(ShareProviderOperation operation)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                ShareCompletedText.Text = "";

                // Obtain the text from the data package. This should match
                // the information placed in it by the GetShareContent method.
                String text = await operation.Data.GetTextAsync();

                // The app can display custom UI to complete the Share operation.
                // Here, we simply display a progress control and delay for a while,
                // to simulate posting to the Contoso Chat service.
                ProgressControl.Visibility = Visibility.Visible;
                await Task.Delay(TimeSpan.FromSeconds(2));

                // All done. Show the success message.
                ProgressControl.Visibility = Visibility.Collapsed;
                ShareCompletedText.Text = $"Your message '{text}' has been shared with Contoso Chat.";

                // Completing the operation causes ShareCompleted to be raised.
                operation.ReportCompleted();
            });
        }
    }
}
