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
using Windows.ApplicationModel.DataTransfer;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public abstract class SharePage : Page
    {
        protected MainPage rootPage = MainPage.Current;
        private DataTransferManager dataTransferManager;

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Register the current page as a share source.
            this.dataTransferManager = DataTransferManager.GetForCurrentView();
            this.dataTransferManager.DataRequested += new TypedEventHandler<DataTransferManager, DataRequestedEventArgs>(this.OnDataRequested);

            // Request to be notified when the user chooses a share target app.
            this.dataTransferManager.TargetApplicationChosen += OnTargetApplicationChosen;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            // Unregister our event handlers.
            this.dataTransferManager.DataRequested -= new TypedEventHandler<DataTransferManager, DataRequestedEventArgs>(this.OnDataRequested);
            this.dataTransferManager.TargetApplicationChosen -= OnTargetApplicationChosen;
        }

        // When share is invoked (by the user or programatically) the event handler we registered will be called to populate the datapackage with the
        // data to be shared.
        private void OnDataRequested(DataTransferManager sender, DataRequestedEventArgs e)
        {
            // Register to be notified if the share operation completes.
            e.Request.Data.ShareCompleted += OnShareCompleted;

            // Call the scenario specific function to populate the datapackage with the data to be shared.
            if (GetShareContent(e.Request))
            {
                // Out of the datapackage properties, the title is required. If the scenario completed successfully, we need
                // to make sure the title is valid since the sample scenario gets the title from the user.
                if (String.IsNullOrEmpty(e.Request.Data.Properties.Title))
                {
                    e.Request.FailWithDisplayText(MainPage.MissingTitleError);
                }
            }
        }

        private void OnTargetApplicationChosen(DataTransferManager sender, TargetApplicationChosenEventArgs e)
        {
            this.rootPage.NotifyUser($"User chose {e.ApplicationName}", NotifyType.StatusMessage);
        }

        private void OnShareCompleted(DataPackage sender, ShareCompletedEventArgs e)
        {
            string shareCompletedStatus = "Shared successfully. ";

            // Typically, this information is not displayed to the user because the
            // user already knows which share target was selected.
            if (!String.IsNullOrEmpty(e.ShareTarget.AppUserModelId))
            {
                // The user picked an app.
                shareCompletedStatus += $"Target: App \"{e.ShareTarget.AppUserModelId}\"";
            }
            else if (e.ShareTarget.ShareProvider != null)
            {
                // The user picked a ShareProvider.
                shareCompletedStatus += $"Target: Share Provider \"{e.ShareTarget.ShareProvider.Title}\"";
            }

            this.rootPage.NotifyUser(shareCompletedStatus, NotifyType.StatusMessage);
        }

        protected void ShowUIButton_Click(object sender, RoutedEventArgs e)
        {
            // If the user clicks the share button, invoke the share flow programatically.
            DataTransferManager.ShowShareUI();
        }

        // This function is implemented by each scenario to share the content specific to that scenario (text, link, image, etc.).
        protected abstract bool GetShareContent(DataRequest request);

        protected Uri ApplicationLink
        {
            get
            {
                return GetApplicationLink(GetType().Name);
            }
        }

        public static Uri GetApplicationLink(string sharePageName)
        {
            return new Uri("ms-sdk-sharesourcecs:navigate?page=" + sharePageName);
        }
    }
}
