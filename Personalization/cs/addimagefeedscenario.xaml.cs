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
using Windows.Foundation;
using Windows.UI.Input;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Input;
using Windows.System.UserProfile;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class AddImageFeedScenario : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public AddImageFeedScenario()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        private async void SetDefaultButton_Click(object sender, RoutedEventArgs e)
        {
            // Pass a URI to your enclosure-based feed of lock screen images.  These must be JPG or PNG files.
            // Note that the sample URL given here is deliberately non-functional.
            SetImageFeedResult result = await LockScreen.RequestSetImageFeedAsync(new Uri("http://contoso.com/myimagerss.xml"));
            if (result == SetImageFeedResult.Success)
            {
                AddImageFeedScenarioOutputText.Text = "Called RequestSetImageFeedAsync: the specified URL was set as the default.";
            }
            else if (result == SetImageFeedResult.ChangeDisabled)
            {
                AddImageFeedScenarioOutputText.Text = "Called RequestSetImageFeedAsync: call succeeded but group policy has the lock screen image slide show turned off.";
            }
            else // (result == SetImageFeedResult.UserCanceled)
            {
                AddImageFeedScenarioOutputText.Text = "Called RequestSetImageFeedAsync: the specified URL was not set as the default.";
            }
        }

        private void RemoveFeedButton_Click(object sender, RoutedEventArgs e)
        {
            bool fResult = LockScreen.TryRemoveImageFeed();
            if (fResult)
            {
                AddImageFeedScenarioOutputText.Text = "Called TryRemoveImageFeed: the associated URL registration was removed.";
            }
            else
            {
                AddImageFeedScenarioOutputText.Text = "Called TryRemoveImageFeed: the associated URL registration removal failed.";
            }
        }
    }
}
