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
using Windows.Storage;
using Windows.UI.Shell;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3_AdaptiveCard : Page
    {
        private UserActivitySession _currentSession;

        public Scenario3_AdaptiveCard()
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
            UserActivity activity = await channel.GetOrCreateUserActivityAsync("Scenario3_AdaptiveCard");

            // Set deep-link and properties.
            activity.ActivationUri = new Uri("sdkUserActivitySample:page?Scenario3_AdaptiveCard");
            activity.VisualElements.DisplayText = "User Activity with Adaptive Card";

            // Sreate and set Adaptive Card.
            StorageFile cardFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Assets/UserActivityCard.json"));
            string cardText = await FileIO.ReadTextAsync(cardFile);
            activity.VisualElements.Content = AdaptiveCardBuilder.CreateAdaptiveCardFromJson(cardText);

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
