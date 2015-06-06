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

using ScreenCasting.Controls;
using ScreenCasting.Data.Azure;
using ScreenCasting.Data.Common;
using System;
using Windows.Foundation;
using Windows.Media.Casting;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace ScreenCasting
{
    public sealed partial class Scenario02 : Page
    {
        private MainPage rootPage;
        private CastingDevicePicker picker = null;
        private VideoMetaData video = null;

        public Scenario02()
        {
            this.InitializeComponent();

            rootPage = MainPage.Current;

            //Subscribe to player events
            player.MediaOpened += Player_MediaOpened;
            player.MediaFailed += Player_MediaFailed;
            player.CurrentStateChanged += Player_CurrentStateChanged;

            // Get an Azure hosted video
            AzureDataProvider dataProvider = new AzureDataProvider();
            video = dataProvider.GetFromID("BigBuckBunnyTrailer");
            this.player.Source = video.VideoLink;

            //Set the source on the player
            rootPage.NotifyUser(string.Format("Opening '{0}'", video.Title), NotifyType.StatusMessage);
            player.Source = video.VideoLink;

            //Subscribe for the clicked event on the custom cast button
            ((MediaTransportControlsWithCustomCastButton)this.player.TransportControls).CastButtonClicked += TransportControls_CastButtonClicked;

            // Instantiate the Device Picker
            picker = new CastingDevicePicker();

            // Generate the filter based on the content in the MediaElement
            picker.Filter.SupportedCastingSources.Add(player.GetAsCastingSource());

            //Hook up device selected event
            picker.CastingDeviceSelected += Picker_CastingDeviceSelected;

            //Hook up device disconnected event
            picker.CastingDevicePickerDismissed += Picker_CastingDevicePickerDismissed;

            ////Set the Appearence of the picker
            /*picker.Appearance.BackgroundColor = Colors.Black;
            picker.Appearance.ForegroundColor = Colors.White;
            picker.Appearance.AccentColor = Colors.White;
            picker.Appearance.SelectedAccentColor = Colors.White;
            picker.Appearance.SelectedForegroundColor = Colors.White;
            picker.Appearance.SelectedBackgroundColor = Colors.Black;*/
                    }

        private void TransportControls_CastButtonClicked(object sender, EventArgs e)
        {
            rootPage.NotifyUser("Custom Cast Button Clicked", NotifyType.StatusMessage);

            //Pause Current Playback
            player.Pause();

            //Get the custom transport controls
            MediaTransportControlsWithCustomCastButton mtc = (MediaTransportControlsWithCustomCastButton)this.player.TransportControls;

            //Retrieve the location of the casting button
            GeneralTransform transform = mtc.CastButton.TransformToVisual(Window.Current.Content as UIElement);
            Point pt = transform.TransformPoint(new Point(0, 0));

            //Show the picker above our custom cast button
            picker.Show(new Rect(pt.X, pt.Y, mtc.CastButton.ActualWidth, mtc.CastButton.ActualHeight), Windows.UI.Popups.Placement.Above);
        }

        private async void Picker_CastingDevicePickerDismissed(CastingDevicePicker sender, object args)
        {
            // This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                player.Play();
            });
        }
        private async void Picker_CastingDeviceSelected(CastingDevicePicker sender, CastingDeviceSelectedEventArgs args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                // The args.SelectedCastingDevice is proxied from the picker process. The picker process is 
                // dismissmed as soon as you break into the debugger. Creating a non-proxied version 
                // allows debugging since the proxied version stops working once the picker is dismissed.
                CastingDevice selectedDevice = await CastingDevice.FromIdAsync(args.SelectedCastingDevice.Id);

                //Create a casting conneciton from our selected casting device
                rootPage.NotifyUser(string.Format("Creating connection for '{0}'", selectedDevice.FriendlyName), NotifyType.StatusMessage);
                CastingConnection connection = selectedDevice.CreateCastingConnection();

                //Hook up the casting events
                connection.ErrorOccurred += Connection_ErrorOccurred;
                connection.StateChanged += Connection_StateChanged;

                // Get the casting source from the MediaElement
                CastingSource source = null;

                try
                {
                    // Get the casting source from the Media Element
                    source = player.GetAsCastingSource();

                    // Start Casting
                    rootPage.NotifyUser(string.Format("Starting casting to '{0}'", selectedDevice.FriendlyName), NotifyType.StatusMessage);
                    CastingConnectionErrorStatus status = await connection.RequestStartCastingAsync(source);

                    if (status == CastingConnectionErrorStatus.Succeeded)
                    {
                        player.Play();
                        rootPage.NotifyUser(string.Format("Starting casting to '{0}'", selectedDevice.FriendlyName), NotifyType.StatusMessage);
                    }

                }
                catch
                {
                    rootPage.NotifyUser(string.Format("Failed to get casting source for video '{0}'", video.Title), NotifyType.ErrorMessage);
                }
            });
        }

        #region Casting Connection Status Methods
        private async void Connection_StateChanged(CastingConnection sender, object args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("Casting Connection State Changed: " + sender.State, NotifyType.StatusMessage);
            });
        }
        private async void Connection_ErrorOccurred(CastingConnection sender, CastingConnectionErrorOccurredEventArgs args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("Connection Error Occured: " + args.Message, NotifyType.ErrorMessage);
            });
        }

        #endregion


        #region Media Element Status Methods
        private void Player_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser(string.Format("{0} '{1}'", this.player.CurrentState, video.Title), NotifyType.StatusMessage);
        }
        private void Player_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            rootPage.NotifyUser(string.Format("Failed to load '{0}'", video.Title), NotifyType.ErrorMessage);
        }
        private void Player_MediaOpened(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser(string.Format("Openend '{0}'", video.Title), NotifyType.StatusMessage);
        }


        #endregion
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

    }
}
