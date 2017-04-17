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
using ScreenCasting.Util;
using System;
using Windows.ApplicationModel.Core;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace ScreenCasting
{
    public sealed partial class Scenario05 : Page
    {
        private MainPage rootPage;
        private DevicePicker picker;
        private VideoMetaData video = null;
        ProjectionViewBroker pvb = new ProjectionViewBroker();
        DeviceInformation activeDevice = null;

        int thisViewId;

        public Scenario05()
        {
            this.InitializeComponent();

            rootPage = MainPage.Current;

            //Subscribe to player events
            player.MediaOpened += Player_MediaOpened;
            player.MediaFailed += Player_MediaFailed;
            player.CurrentStateChanged += Player_CurrentStateChanged;

            // Get an Azure hosted video
            AzureDataProvider dataProvider = new AzureDataProvider();
            video = dataProvider.GetRandomVideo();

            //Set the source on the player
            rootPage.NotifyUser(string.Format("Opening '{0}'", video.Title), NotifyType.StatusMessage);
            this.player.Source = video.VideoLink;
            this.LicenseText.Text = "License: " + video.License;

            //Subscribe for the clicked event on the custom cast button
            ((MediaTransportControlsWithCustomCastButton)this.player.TransportControls).CastButtonClicked += TransportControls_CastButtonClicked;

            // Instantiate the Device Picker
            picker = new DevicePicker ();

            // Get the device selecter for Miracast devices
            picker.Filter.SupportedDeviceSelectors.Add(ProjectionManager.GetDeviceSelector());

            //Hook up device selected event
            picker.DeviceSelected += Picker_DeviceSelected;

            //Hook up device disconnected event
            picker.DisconnectButtonClicked += Picker_DisconnectButtonClicked;

            //Hook up picker dismissed event
            picker.DevicePickerDismissed += Picker_DevicePickerDismissed;

            // Hook up the events that are received when projection is stoppped
            pvb.ProjectionStopping += Pvb_ProjectionStopping;
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
        private async void Picker_DeviceSelected(DevicePicker sender, DeviceSelectedEventArgs args)
        {       
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                try
                { 
                // Set status to Connecting
                picker.SetDisplayStatus(args.SelectedDevice, "Connecting", DevicePickerDisplayStatusOptions.ShowProgress);
                
                // Getting the selected device improves debugging
                DeviceInformation selectedDevice = args.SelectedDevice;

                thisViewId = Windows.UI.ViewManagement.ApplicationView.GetForCurrentView().Id;

                // If projection is already in progress, then it could be shown on the monitor again
                // Otherwise, we need to create a new view to show the presentation
                if (rootPage.ProjectionViewPageControl == null)
                {
                    // First, create a new, blank view
                    var thisDispatcher = Window.Current.Dispatcher;
                    await CoreApplication.CreateNewView().Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        // ViewLifetimeControl is a wrapper to make sure the view is closed only
                        // when the app is done with it
                        rootPage.ProjectionViewPageControl = ViewLifetimeControl.CreateForCurrentView();

                        // Assemble some data necessary for the new page
                        pvb.MainPageDispatcher = thisDispatcher;
                        pvb.ProjectionViewPageControl = rootPage.ProjectionViewPageControl;
                        pvb.MainViewId = thisViewId;

                        // Display the page in the view. Note that the view will not become visible
                        // until "StartProjectingAsync" is called
                        var rootFrame = new Frame();
                        rootFrame.Navigate(typeof(ProjectionViewPage), pvb);
                        Window.Current.Content = rootFrame;

                        Window.Current.Activate();
                    });
                }

                try
                {
                    // Start/StopViewInUse are used to signal that the app is interacting with the
                    // view, so it shouldn't be closed yet, even if the user loses access to it
                    rootPage.ProjectionViewPageControl.StartViewInUse();

                    try
                    {
                        await ProjectionManager.StartProjectingAsync(rootPage.ProjectionViewPageControl.Id, thisViewId, selectedDevice);

                    }
                    catch (Exception ex)
                    {
                        if (!ProjectionManager.ProjectionDisplayAvailable || pvb.ProjectedPage == null)
                            throw ex;
                    }

                    // ProjectionManager currently can throw an exception even when projection has started.\
                    // Re-throw the exception when projection has not been started after calling StartProjectingAsync 
                    if (ProjectionManager.ProjectionDisplayAvailable && pvb.ProjectedPage != null)
                    {
                        this.player.Pause();
                        await pvb.ProjectedPage.SetMediaSource(this.player.Source, this.player.Position);
                        activeDevice = selectedDevice;
                        // Set status to Connected
                        picker.SetDisplayStatus(args.SelectedDevice, "Connected", DevicePickerDisplayStatusOptions.ShowDisconnectButton);
                        picker.Hide(); 
                    }
                    else
                    {
                        rootPage.NotifyUser(string.Format("Projection has failed to '{0}'", selectedDevice.Name), NotifyType.ErrorMessage);
                        // Set status to Failed
                        picker.SetDisplayStatus(args.SelectedDevice, "Connection Failed", DevicePickerDisplayStatusOptions.ShowRetryButton);
                    }
                }
                catch (Exception)
                {
                    rootPage.NotifyUser(string.Format("Projection has failed to '{0}'", selectedDevice.Name), NotifyType.ErrorMessage);
                    // Set status to Failed
                    try { picker.SetDisplayStatus(args.SelectedDevice, "Connection Failed", DevicePickerDisplayStatusOptions.ShowRetryButton); } catch { }
                }
                }
                catch (Exception ex)
                {
                    UnhandledExceptionPage.ShowUnhandledException(ex);
                }
            });
        }
        private async void Picker_DevicePickerDismissed(DevicePicker sender, object args)
        {
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                if (activeDevice == null)
                {
                    player.Play();
                }
            });
        }
        private async void Picker_DisconnectButtonClicked(DevicePicker sender, DeviceDisconnectButtonClickedEventArgs args)
        {
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("Disconnect Button clicked", NotifyType.StatusMessage);
                //Update the display status for the selected device.
                sender.SetDisplayStatus(args.Device, "Disconnecting", DevicePickerDisplayStatusOptions.ShowProgress);

                if (this.pvb.ProjectedPage != null)
                    this.pvb.ProjectedPage.StopProjecting();

                //Update the display status for the selected device.
                sender.SetDisplayStatus(args.Device, "Disconnected", DevicePickerDisplayStatusOptions.None);
                rootPage.NotifyUser("Disconnected", NotifyType.StatusMessage);

                // Set the active device variables to null
                activeDevice = null;
            });
        }

        private async void Pvb_ProjectionStopping(object sender, EventArgs e)
        {
            ProjectionViewBroker broker = sender as ProjectionViewBroker;

            TimeSpan position;
            Uri source = null;

            await broker.ProjectedPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                position = broker.ProjectedPage.Player.Position;
                source = broker.ProjectedPage.Player.Source;
            });

            await rootPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("Resuming playback on the first screen", NotifyType.StatusMessage);
                this.player.Source = source;

                this.player.Position = position;
                this.player.Play();
                rootPage.ProjectionViewPageControl = null;
            });
        }

        #region MediaElement Status Methods
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
     
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }
    }
}
