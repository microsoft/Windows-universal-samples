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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class S4_CheckIndexRevision
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public S4_CheckIndexRevision()
        {
            this.InitializeComponent();

            // This initializes the value used throughout the sample which tracks the expected index revision number. This
            // value is used to check if the app's expected revision number matches the actual index revision number and is
            // stored in the app's local settings to allow it to persist across termination.
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            if (!localSettings.Values.ContainsKey("expectedIndexRevision"))
            {
                localSettings.Values.Add("expectedIndexRevision", (ulong)0);
            }
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        /// <summary>
        /// This function displays the app's expected index revision number and the actual index revision number reported by
        /// the index. For purposes of this sample, this is done in reaction to a button being pressed. In practice, this should
        /// be done when an app is activated.
        /// If the reported index revision number is 0, it means the index has been reset and an app should re-push all of its
        /// data.
        /// If the reported index revision number is not 0, but doesn't match the app's expected index revision number, it
        /// indicates that not all of the app's adds/deletes/updates were successfully processed. This should be an edge case,
        /// but when hit, the app should re-do the missed operations. Tracking the expected index revision number can be used to
        /// create checkpoints in the app so it can only re-do the operations required to match the expected index revision
        /// number.
        /// </summary>
        private void CheckIndexRevision_Click(object sender, RoutedEventArgs e)
        {
            var indexer = Windows.Storage.Search.ContentIndexer.GetIndexer();
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            var expectedRevisionNumber = localSettings.Values["expectedIndexRevision"];
            string outputString = "Expected Index Revision Number: " + expectedRevisionNumber;
            outputString += "\nReported Index Revision Number: " + indexer.Revision;
            if ((ulong)expectedRevisionNumber != indexer.Revision)
            {
                // There is a mismatch between the expected and reported index revision numbers.
                if (indexer.Revision == 0)
                {
                    // The index has been reset, so code would be added here to re-push all data.
                }
                else
                {
                    // The index hasn't been reset, but it doesn't contain all expected updates, so add code to get the index
                    // back into the expected state.
                }

                // After doing the necessary work to get back to a synchronized state, set the expected index revision number
                // to match the reported revision number.
                localSettings.Values["expectedIndexRevision"] = indexer.Revision;
            }
            rootPage.NotifyUser(outputString, NotifyType.StatusMessage);
        }
    }
}
