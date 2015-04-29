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

using ScreenCasting.Data.Azure;
using ScreenCasting.Data.Common;
using ScreenCasting.Util;
using System;
using Windows.ApplicationModel.Activation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace ScreenCasting
{
    public sealed partial class Scenario04 : Page
    {
        private MainPage rootPage;
        private VideoMetaData video = null;
        private TimeSpan originalPosition = new TimeSpan(0);

        public Scenario04()
        {
            this.InitializeComponent();

            rootPage = MainPage.Current;
            rootPage.SizeChanged += RootPage_SizeChanged;
        }



        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            if (e.Parameter is DialReceiverActivatedEventArgs)
            {
                DialReceiverActivatedEventArgs activationArgs = (DialReceiverActivatedEventArgs)e.Parameter;

                //Parse the DIAL arguments from the activation arguments
                DialLaunchArguments dialArgs = DialLaunchArguments.Parse(activationArgs.Arguments);

                // Get the list of available Azure videos.
                AzureDataProvider dataProvider = new AzureDataProvider();
                // Get the vide that is playing
                video = dataProvider.GetFromID(dialArgs.VideoId);

                //Subscribe to player events
                player.MediaOpened += Player_MediaOpened;
                player.MediaFailed += Player_MediaFailed;
                player.CurrentStateChanged += Player_CurrentStateChanged;
                
                //Set the source on the player
                rootPage.NotifyUser(string.Format("Opening '{0}'", video.Title), NotifyType.StatusMessage);
                this.player.Source = video.VideoLink;

                player.Position = dialArgs.Position;
            }
            else
            {
                rootPage.NotifyUser("Scenario was not activated using DIAL", NotifyType.ErrorMessage);
            }
        }

        private void RootPage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            this.player.Height = this.Height;
            this.player.Width = this.Width;
        }

        #region MediaElement Status Changes
        private void Player_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            // Update status
            rootPage.NotifyUser(string.Format("{0} '{1}'", this.player.CurrentState, video.Title), NotifyType.StatusMessage);
        }

        private void Player_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            rootPage.NotifyUser(string.Format("Failed to load '{0}'", video.Title), NotifyType.ErrorMessage);
        }

        private void Player_MediaOpened(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser(string.Format("Openend '{0}'", video.Title), NotifyType.StatusMessage);

            player.Play();
        }

        #endregion
    }
}
