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
using Windows.ApplicationModel.UserActivities;
using Windows.UI;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_AdvancedUserActivity : Page
    {
        private UserActivitySession _currentSession;

        public Scenario2_AdvancedUserActivity()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            // If we were activated from a URI, say so.
            // A real app would use the information in the URI to restore state.
            var message = e.Parameter as string;
            if (!String.IsNullOrEmpty(message))
            {
                MainPage.Current.NotifyUser(message, NotifyType.StatusMessage);
            }

            await CreateUserActivityAsync();
        }

        private async Task CreateUserActivityAsync()
        {
            // Get channel and create activity.
            UserActivityChannel channel = UserActivityChannel.GetDefault();
            UserActivity activity = await channel.GetOrCreateUserActivityAsync("Scenario2_AdvancedUserActivity");

            // Set deep-link and properties.
            activity.ActivationUri = new Uri("sdkUserActivitySample:page?Scenario2_AdvancedUserActivity");
            activity.VisualElements.DisplayText = "README.md";

            // Add advanced properties.
            activity.ContentUri = new Uri("https://raw.githubusercontent.com/Microsoft/Windows-universal-samples/master/README.md");
            activity.FallbackUri = new Uri("http://go.microsoft.com/fwlink/p/?LinkId=873257");

            activity.VisualElements.Attribution = new UserActivityAttribution()
            {

                IconUri = new Uri("https://raw.githubusercontent.com/Microsoft/Windows-universal-samples/master/SharedContent/media/storelogo-sdk.png"),
                AlternateText = "Windows SDK Samples",
                AddImageQuery = false
            };

            activity.VisualElements.BackgroundColor = Color.FromArgb(0xFF, 0x00, 0xb2, 0xf0);
            activity.VisualElements.Description = "https://raw.githubusercontent.com/Microsoft/Windows-universal-samples/master/README.md";

            // Save to activity feed.
            await activity.SaveAsync();

            // Create a session, which indicates that the user is engaged
            // in the activity.
            _currentSession = activity.CreateSession();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            // Dispose the session, which indicates that the user is no longer
            // engaged in the activity.
            _currentSession?.Dispose();
        }
    }
}
