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
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Media.DialProtocol;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace ScreenCasting
{
    public sealed partial class Scenario03 : Page
    {
        private MainPage rootPage;
        private DialDevicePicker picker = null;
        private VideoMetaData video = null;
        private DialDevice activeDialDevice = null;
        private DeviceInformation activeDeviceInformation = null;


        public Scenario03()
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

            //Subscribe for the clicked event on the custom cast button
            ((MediaTransportControlsWithCustomCastButton)this.player.TransportControls).CastButtonClicked += TransportControls_CastButtonClicked;

            //Configure the DIAL launch arguments for the current video
            this.dial_launch_args_textbox.Text = string.Format("v={0}&t=0&pairingCode=E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F", video.Id);

            //Subscribe for the clicked event on the custom cast button
            ((MediaTransportControlsWithCustomCastButton)this.player.TransportControls).CastButtonClicked += TransportControls_CastButtonClicked;

            // Instantiate the Device Picker
            picker = new DialDevicePicker();

            //Add the DIAL Filter, so that the application only shows DIAL devices that have 
            // the application installed or advertise that they can install them.
            picker.Filter.SupportedAppNames.Add(this.dial_appname_textbox.Text);

            //Hook up device selected event
            picker.DialDeviceSelected += Picker_DeviceSelected;

            //Hook up the picker disconnected event
            picker.DisconnectButtonClicked += Picker_DisconnectButtonClicked;

            //Hook up the picker dismissed event
            picker.DialDevicePickerDismissed += Picker_DevicePickerDismissed;
        }
        private void TransportControls_CastButtonClicked(object sender, EventArgs e)
        {
            rootPage.NotifyUser("Show Device Picker Button Clicked", NotifyType.StatusMessage);

            //Pause Current Playback
            player.Pause();

            //Get the custom transport controls
            MediaTransportControlsWithCustomCastButton mtc = (MediaTransportControlsWithCustomCastButton)this.player.TransportControls;
            //Retrieve the location of the casting button
            GeneralTransform transform = mtc.CastButton.TransformToVisual(Window.Current.Content as UIElement);
            Point pt = transform.TransformPoint(new Point(0, 0));

            //Show the picker above our Show Device Picker button
            picker.Show(new Rect(pt.X, pt.Y, mtc.CastButton.ActualWidth, mtc.CastButton.ActualHeight), Windows.UI.Popups.Placement.Above);

            try
            {
                if (activeDialDevice != null)
                    picker.SetDisplayStatus(activeDialDevice, DialDeviceDisplayStatus.Connected);
            }
            catch (Exception ex)
            {
                UnhandledExceptionPage.ShowUnhandledException(ex);
            }
        }

        #region Device Picker Methods

        private async void Picker_DevicePickerDismissed(DialDevicePicker sender, object args)
        {
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                try
                {
                    rootPage.NotifyUser(string.Format("Picker DevicePickerDismissed event fired for device"), NotifyType.StatusMessage);

                    if (activeDialDevice != null)
                    {
                        // Resume video playback
                        this.player.Play();
                    }
                }
                catch (Exception ex)
                {
                    UnhandledExceptionPage.ShowUnhandledException(ex);
                }
            });
        }

        private async void Picker_DisconnectButtonClicked(DialDevicePicker sender, DialDisconnectButtonClickedEventArgs args)
        {
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                try
                {
                    rootPage.NotifyUser(string.Format("Picker DisconnectButtonClicked event fired for device '{0}'", activeDeviceInformation.Name), NotifyType.StatusMessage);

                    // Get the DialDevice instance for the selected device
                    DialDevice selectedDialDevice = await DialDevice.FromIdAsync(args.Device.Id);
                    // Update the picker status
                    picker.SetDisplayStatus(selectedDialDevice, DialDeviceDisplayStatus.Connecting);

                    DialApp app = selectedDialDevice.GetDialApp(this.dial_appname_textbox.Text);

                    //Get the current application state
                    //DialAppStateDetails stateDetails = await app.GetAppStateAsync();

                    DialAppStopResult result = await app.StopAsync();

                    if (result == DialAppStopResult.Stopped)
                    {
                        // In case getting the application state failed because of a network failure, you could add retry logic
                        rootPage.NotifyUser("Application stopped successfully.", NotifyType.StatusMessage);
                        picker.SetDisplayStatus(args.Device, DialDeviceDisplayStatus.Disconnected);
                        activeDialDevice = null;
                        activeDeviceInformation = null;
                        picker.Hide(); 
                    }
                    else
                    {
                        if (result == DialAppStopResult.StopFailed || result == DialAppStopResult.NetworkFailure)
                        {
                            // In case getting the application state failed because of a network failure, you could add retry logic
                            rootPage.NotifyUser(string.Format("Error occured trying to stop application. Status: '{0}'", result.ToString()), NotifyType.StatusMessage);
                            picker.SetDisplayStatus(args.Device, DialDeviceDisplayStatus.Error);
                        }
                        else //in case of DialAppStopResult.OperationNotSupported, there is not much more you can do. You could implement your own
                             // mechanism to stop the application on that device.
                        {
                            // In case getting the application state failed because of a network failure, you could add retry logic
                            rootPage.NotifyUser(string.Format("Stop is not supported by device: '{0}'", activeDeviceInformation.Name), NotifyType.ErrorMessage);
                            activeDialDevice = null;    
                            activeDeviceInformation = null;
                            
                        }
                    }
                }
                catch (Exception ex)
                {
                    UnhandledExceptionPage.ShowUnhandledException(ex);
                }

            });
        }

        private async void Picker_DeviceSelected(DialDevicePicker sender, DialDeviceSelectedEventArgs args)
        {
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>            
            {
                try {
                    rootPage.NotifyUser(string.Format("Picker DeviceSelected event fired"), NotifyType.StatusMessage);

                    // Set the status to connecting
                    picker.SetDisplayStatus(args.SelectedDialDevice, DialDeviceDisplayStatus.Connecting);

                    rootPage.NotifyUser(string.Format("Resolving DialDevice'"), NotifyType.StatusMessage);
             
                    //Get the DialApp object for the specific application on the selected device
                    DialApp app = args.SelectedDialDevice.GetDialApp(this.dial_appname_textbox.Text);

                    if (app == null)
                    {
                        //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
                        //rootPage.NotifyUser(string.Format("'{0}' cannot find app with ID '{1}'", selectedDeviceInformation.Name, this.dial_appname_textbox.Text), NotifyType.StatusMessage);
                        picker.SetDisplayStatus(args.SelectedDialDevice, DialDeviceDisplayStatus.Error);
                    }
                    else
                    {
                        rootPage.NotifyUser(string.Format("Attempting to launch '{0}'", app.AppName), NotifyType.StatusMessage);
                        //Launch the application on the 1st screen device
                        DialAppLaunchResult result = await app.RequestLaunchAsync(this.dial_launch_args_textbox.Text);

                        //Verify to see whether the application was launched
                        if (result == DialAppLaunchResult.Launched)
                        {
                            rootPage.NotifyUser(string.Format("Launched '{0}'", app.AppName), NotifyType.StatusMessage);
                            activeDialDevice = args.SelectedDialDevice;
                            DeviceInformation selectedDeviceInformation = await DeviceInformation.CreateFromIdAsync(args.SelectedDialDevice.Id);

                            activeDeviceInformation = selectedDeviceInformation;

                            //This is where you will need to add you application specific communication between your 1st and 2nd screen applications
                            //...
                            //...
                            //...

                            picker.SetDisplayStatus(activeDialDevice, DialDeviceDisplayStatus.Connected);
                            picker.Hide();
                        }
                        else
                        {
                            rootPage.NotifyUser(string.Format("Attempting to launch '{0}'", app.AppName), NotifyType.StatusMessage);
                            picker.SetDisplayStatus(args.SelectedDialDevice, DialDeviceDisplayStatus.Error);
                        }
                    }
                }
                catch (Exception ex)
                {
                    UnhandledExceptionPage.ShowUnhandledException(ex);
                }
            });
        }

        #endregion

        #region Media Element Status Methods
        private void Player_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            // Update status
            rootPage.NotifyUser(string.Format("{0} '{1}'", this.player.CurrentState, video.Title), NotifyType.StatusMessage);
        }
        private void Player_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            // Update status
            rootPage.NotifyUser(string.Format("Failed to load '{0}'", video.Title), NotifyType.ErrorMessage);
        }
        private void Player_MediaOpened(object sender, RoutedEventArgs e)
        {
            // Update status
            rootPage.NotifyUser(string.Format("Openend '{0}'", video.Title), NotifyType.StatusMessage);
            // Start playback
            player.Play();
        }

        #endregion

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

    }
}
