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
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Media.Casting;
using Windows.Media.DialProtocol;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace ScreenCasting
{
    public sealed partial class Scenario1 : Page
    {
        private const int MAX_RESULTS = 10;

        private MainPage rootPage;
        private DevicePicker picker;
        private DeviceInformation activeDevice = null;
        private object activeCastConnectionHandler = null;
        private VideoMetaData video = null;


        public Scenario1()
        {
            this.InitializeComponent();

            rootPage = MainPage.Current;

            // Get the list of available Azure videos.
            AzureDataProvider dataProvider = new AzureDataProvider();
            List<VideoMetaData> videos = dataProvider.GetAll(MAX_RESULTS);
            Random indexRandomizer = new Random();

            // Pick a random video
            video = videos[indexRandomizer.Next(0, videos.Count - 1)];

            //Subscribe to player events
            player.MediaOpened += Player_MediaOpened;
            player.MediaFailed += Player_MediaFailed;
            player.CurrentStateChanged += Player_CurrentStateChanged;

            //Set the look and feel of the TransportControls
            player.TransportControls.IsCompact = true;

            // Instantiate the Device Picker
            picker = new DevicePicker();

            //Set the source on the player
            rootPage.NotifyUser(string.Format("Opening '{0}'", video.Title), NotifyType.StatusMessage);
            player.Source = video.VideoLink;

            this.dial_launch_args_textbox.Text = string.Format("v={0}&t=0&pairingCode=E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F", video.Id);

            //Hook up device selected event
            picker.DeviceSelected += Picker_DeviceSelected;

            //Hook up device disconnected event
            picker.DisconnectButtonClicked += Picker_DisconnectButtonClicked;

            //Hook up picker dismissed event
            picker.DevicePickerDismissed += Picker_DevicePickerDismissed;

            ////Set the Appearence of the picker
            //picker.Appearance.BackgroundColor = Colors.Black;
            //picker.Appearance.ForegroundColor = Colors.White;
            //picker.Appearance.SelectedAccentColor = Colors.LightGray;
            //picker.Appearance.SelectedForegroundColor = Colors.White;
            //picker.Appearance.AccentColor = Colors.White;
            //picker.Appearance.SelectedAccentColor = Colors.White;
            //picker.Appearance.SelectedBackgroundColor = Colors.Black;
        }


        private async void disconnectButton_Click(object sender, RoutedEventArgs e)
        {
            //Update the display status for the selected device.
            rootPage.NotifyUser("Disconnecting", NotifyType.StatusMessage);

            bool disconnected = false;

            if (this.activeCastConnectionHandler is DialApp)
                disconnected = await TryStopDialAppAsync((DialApp)activeCastConnectionHandler);
            if (this.activeCastConnectionHandler is CastingConnection)
                disconnected = await TryDisconnectCastingSessionAsync((CastingConnection)activeCastConnectionHandler);

            if (disconnected)
            {
                //Update the display status for the selected device.
                rootPage.NotifyUser(string.Format("Disconnected : '{0}'", activeDevice.Name), NotifyType.StatusMessage);
                // Set the active device variables to null
                activeDevice = null;
                activeCastConnectionHandler = null;
                disconnectButton.IsEnabled = true;

            }
            else
            {
                //Update the display status for the selected device.
                rootPage.NotifyUser(string.Format("Disconnecting from '{0}' failed", activeDevice.Name), NotifyType.ErrorMessage);
            }
        }
        private async void castButton_Click(object sender, RoutedEventArgs e)
        {
            player.Pause();

            //Retrieve the location of the casting button
            GeneralTransform transform = castButton.TransformToVisual(Window.Current.Content as UIElement);
            Point pt = transform.TransformPoint(new Point(0, 0));
            //Add the DIAL Filter, so that the application only shows DIAL devices that have the application installed or advertise that they can install them.
            picker.Filter.SupportedDeviceSelectors.Add(DialDevice.GetDeviceSelector(this.dial_appname_textbox.Text));
            //Add the CAST API Filter, so that the application only shows Miracast, Bluetooth, DLNA devices that can render the video
            picker.Filter.SupportedDeviceSelectors.Add(await CastingDevice.GetDeviceSelectorFromCastingSourceAsync(player.GetAsCastingSource()));
            //RequiredDeviceProperties.AddProps(picker.RequestedProperties);

            //if (activeDevice != null)
            //    //Update the display status for the previously selected device.
            //    picker.SetDisplayStatus(activeDevice, "Connected", DevicePickerDisplayStatusOptions.ShowDisconnectButton);

            //Show the picker above our Show Device Picker button
            picker.Show(new Rect(pt.X, pt.Y, castButton.ActualWidth, castButton.ActualHeight), Windows.UI.Popups.Placement.Above);

            rootPage.NotifyUser("Show Device Picker Button Clicked", NotifyType.StatusMessage);
        }

        private async void Picker_DeviceSelected(DevicePicker sender, DeviceSelectedEventArgs args)
        {
            string deviceId = args.SelectedDevice.Id;

            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                //Update the display status for the selected device to connecting.
                picker.SetDisplayStatus(args.SelectedDevice, "Connecting", DevicePickerDisplayStatusOptions.ShowProgress);

                // BUG: In order to support debugging it is best to retreive the device from the local app instead
                // of continuing to use the DeviceInformation instance that is proxied from the picker.
                DeviceInformation device = null;
                try { if (deviceId.IndexOf("dial", StringComparison.OrdinalIgnoreCase) == -1) device = await DeviceInformation.CreateFromIdAsync(deviceId, RequiredDeviceProperties.Props, DeviceInformationKind.DeviceContainer); } catch { }
                try { if (device == null) device = await DeviceInformation.CreateFromIdAsync(deviceId, RequiredDeviceProperties.Props, DeviceInformationKind.AssociationEndpoint); } catch { }
                // In case the workaround did not work
                if (device == null) device = args.SelectedDevice;

                //Try casting using DIAL first
                bool castSucceeded = await TryLaunchDialAppAsync(device);
                //If DIAL did not work for the selected device, try the CAST API
                if (!castSucceeded)
                {
                    castSucceeded = await TryCastMediaElementAsync(device);
                }

                if (castSucceeded)
                {
                    //Update the display status for the selected device.
                    try { picker.SetDisplayStatus(device, "Connected", DevicePickerDisplayStatusOptions.ShowDisconnectButton); } catch { }
                    disconnectButton.IsEnabled = true;

                    // Hide the picker now that all the work is completed
                    //picker.Hide();
                }
                else
                {
                    //Show a retry button when connecting to the selected device failed.
                    picker.SetDisplayStatus(device, "Connecting failed", DevicePickerDisplayStatusOptions.ShowRetryButton);
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
                    this.disconnectButton.IsEnabled = false;
                    player.Play();
                }
            });
        }
        private async void Picker_DisconnectButtonClicked(DevicePicker sender, DeviceDisconnectButtonClickedEventArgs args)
        {

            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                rootPage.NotifyUser("Disconnect Button clicked", NotifyType.StatusMessage);

                //Update the display status for the selected device.
                sender.SetDisplayStatus(args.Device, "Disconnecting", DevicePickerDisplayStatusOptions.ShowProgress);

                bool disconnected = false;

                if (this.activeCastConnectionHandler is DialApp)
                    disconnected = await TryStopDialAppAsync((DialApp)activeCastConnectionHandler);
                if (this.activeCastConnectionHandler is CastingConnection)
                    disconnected = await TryDisconnectCastingSessionAsync((CastingConnection)activeCastConnectionHandler);

                if (disconnected)
                {
                    //Update the display status for the selected device.
                    sender.SetDisplayStatus(args.Device, "Disconnected", DevicePickerDisplayStatusOptions.None);
                    // Set the active device variables to null
                    activeDevice = null;
                    activeCastConnectionHandler = null;

                    //Hide the picker
                    sender.Hide();
                }
                else
                {
                    //Update the display status for the selected device.
                    sender.SetDisplayStatus(args.Device, "Disconnect failed", DevicePickerDisplayStatusOptions.ShowDisconnectButton);
                }
            });
        }

        private void Player_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            // As long as the video is ready to play and more importantly get a casting source
            // allow the watcher to be started
            this.castButton.IsEnabled =
                this.player.CurrentState == MediaElementState.Playing
                 || this.player.CurrentState == MediaElementState.Paused
                 || this.player.CurrentState == MediaElementState.Buffering;

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

        private async Task<bool> TryCastMediaElementAsync(DeviceInformation device)
        {
            bool castMediaElementSucceeded = false;

            //Verify whether the selected device supports DLNA, Bluetooth, or Miracast.
            rootPage.NotifyUser(string.Format("Checking to see if device {0} supports Miracast, Bluetooth, or DLNA", device.Name), NotifyType.StatusMessage);
            
            //BUG: Takes too long. Workaround, just try to create the CastingDevice
            //if (await CastingDevice.DeviceInfoSupportsCastingAsync(device))
            //{
                CastingConnection connection = null;

                //Check to see whether we are casting to the same device
                if (activeDevice != null && device.Id == activeDevice.Id)
                    connection = activeCastConnectionHandler as CastingConnection;
                else // if not casting to the same device reset the active device related variables.
                {
                    activeDevice = null;
                    activeCastConnectionHandler = null;
                }

                // If we can re-use the existing connection
                if (connection == null || connection.State == CastingConnectionState.Disconnected || connection.State == CastingConnectionState.Disconnecting)
                {
                    CastingDevice castDevice = null;
                    activeDevice = null;

                    //Try to create a CastingDevice instannce. If it doesn't succeed, the selected device does not support playback of the video source.
                    rootPage.NotifyUser(string.Format("Attempting to resolve casting device for '{0}'", device.Name), NotifyType.StatusMessage);
                    try { castDevice = await CastingDevice.FromIdAsync(device.Id); } catch { }

                    if (castDevice == null)
                    {
                        //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
                        rootPage.NotifyUser(string.Format("'{0}' does not support playback of this media", device.Name), NotifyType.StatusMessage);
                    }
                    else
                    {
                        //Create a casting conneciton from our selected casting device
                        rootPage.NotifyUser(string.Format("Creating connection for '{0}'", device.Name), NotifyType.StatusMessage);
                        connection = castDevice.CreateCastingConnection();

                        //Hook up the casting events
                        connection.ErrorOccurred += Connection_ErrorOccurred;
                        connection.StateChanged += Connection_StateChanged;
                    }

                    //Cast the content loaded in the media element to the selected casting device
                    rootPage.NotifyUser(string.Format("Casting to '{0}'", device.Name), NotifyType.StatusMessage);

                    CastingSource source = null;
                    // Get the casting source
                    try { source = player.GetAsCastingSource(); } catch { }

                    if (source == null)
                    {
                        rootPage.NotifyUser(string.Format("Failed to get casting source for video '{0}'", video.Title), NotifyType.ErrorMessage);
                    }
                    else
                    {
                        CastingConnectionErrorStatus status = await connection.RequestStartCastingAsync(source);

                        if (status == CastingConnectionErrorStatus.Succeeded)
                        {
                            //Remember the device to which casting succeeded
                            activeDevice = device;
                            //Remember the current active connection.
                            activeCastConnectionHandler = connection;
                            castMediaElementSucceeded = true;
                            player.Play();
                        }
                        else
                        {
                            rootPage.NotifyUser(string.Format("Failed to cast to '{0}'", device.Name), NotifyType.ErrorMessage);
                        }
                    }
                //}
            }
            return castMediaElementSucceeded;
        }
        private async void Connection_StateChanged(CastingConnection sender, object args)
        {            
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                disconnectButton.IsEnabled = !(sender.State == CastingConnectionState.Disconnected || sender.State == CastingConnectionState.Disconnecting);
                rootPage.NotifyUser("Casting Connection State Changed: " + sender.State, NotifyType.StatusMessage);
            });
        }
        private async void Connection_ErrorOccurred(CastingConnection sender, CastingConnectionErrorOccurredEventArgs args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("Casting Error Occured: " + args.Message, NotifyType.ErrorMessage);
                disconnectButton.IsEnabled = false;
                activeDevice = null;
                activeCastConnectionHandler = null;
            });
        }
        private async Task<bool> TryDisconnectCastingSessionAsync(CastingConnection connection)
        {
            bool disconnected = false;

            //Disconnect the casting session
            CastingConnectionErrorStatus status = await connection.DisconnectAsync();

            if (status == CastingConnectionErrorStatus.Succeeded)
            {
                rootPage.NotifyUser("Connection disconnected successfully.", NotifyType.StatusMessage);
                disconnected = true;
            }
            else
            {
                rootPage.NotifyUser(string.Format("Failed to disconnect connection with reason {0}.", status.ToString()), NotifyType.ErrorMessage);
            }

            return disconnected;
        }

        private async Task<bool> TryLaunchDialAppAsync(DeviceInformation device)
        {
            bool dialAppLaunchSucceeded = false;

            if (device.Id.IndexOf("dial", StringComparison.OrdinalIgnoreCase) > -1)
            {
                //Update the launch arguments to include the Position
                this.dial_launch_args_textbox.Text = string.Format("v={0}&t={1}&pairingCode=E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F", video.Id, player.Position.Ticks);

                //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
                rootPage.NotifyUser(string.Format("Checking to see if device {0} supports DIAL", device.Name), NotifyType.StatusMessage);
                //BUG: Takes too long. Workaround, just try to create the DialDevice
                //if (await DialDevice.DeviceInfoSupportsDialAsync(device))
                //{
                DialDevice dialDevice = null;

                //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
                rootPage.NotifyUser(string.Format("Attempting to resolve DIAL device for '{0}'", device.Name), NotifyType.StatusMessage);
                try { dialDevice = await DialDevice.FromIdAsync(device.Id); } catch { }

                if (dialDevice == null)
                {
                    //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
                    rootPage.NotifyUser(string.Format("'{0}' does not support DIAL", device.Name), NotifyType.StatusMessage);
                }
                else
                {
                    //Get the DialApp object for the specific application on the selected device
                    DialApp app = dialDevice.GetDialApp(this.dial_appname_textbox.Text);

                    if (app == null)
                    {
                        //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
                        rootPage.NotifyUser(string.Format("'{0}' cannot find app with ID '{1}'", device.Name, this.dial_appname_textbox.Text), NotifyType.StatusMessage);
                    }
                    else
                    {
                        //Get the current application state
                        DialAppStateDetails stateDetails = await app.GetAppStateAsync();
                        if (stateDetails.State == DialAppState.NetworkFailure)
                        {
                            // In case getting the application state failed because of a network failure
                            rootPage.NotifyUser("Network Failure while getting application state", NotifyType.ErrorMessage);
                        }
                        else
                        {
                            rootPage.NotifyUser(string.Format("Attempting to launch '{0}'", app.AppName), NotifyType.StatusMessage);
                            //Launch the application on the 1st screen device
                            DialAppLaunchResult result = await app.RequestLaunchAsync(this.dial_launch_args_textbox.Text);

                            //Verify to see whether the application was launched
                            if (result == DialAppLaunchResult.Launched)
                            {
                                //Remember the device to which casting succeeded
                                activeDevice = device;
                                //DIAL is sessionsless but the DIAL app allows us to get the state and "disconnect".
                                //Disconnect in the case of DIAL is equivalenet to stopping the app.
                                activeCastConnectionHandler = app;
                                rootPage.NotifyUser(string.Format("Launched '{0}'", app.AppName), NotifyType.StatusMessage);
                                //This is where you will need to add you application specific communication between your 1st and 2nd screen applications
                                //...
                                dialAppLaunchSucceeded = true;
                            }
                        }
                    }
                }
                //}
                //else
                //{
                //    rootPage.NotifyUser(string.Format("'{0}' does not support DIAL", device.Name), NotifyType.StatusMessage);
                //}
            }
            return dialAppLaunchSucceeded;
        }
        private async Task<bool> TryStopDialAppAsync(DialApp app)
        {         
            bool stopped = false;

            //Get the current application state
            DialAppStateDetails stateDetails = await app.GetAppStateAsync();

            switch (stateDetails.State)
            {
                case DialAppState.NetworkFailure:
                    {
                        // In case getting the application state failed because of a network failure, you could add retry logic
                        rootPage.NotifyUser("Network Failure while getting application state", NotifyType.ErrorMessage);
                        break;
                    }
                case DialAppState.Stopped:
                    {
                        stopped = true;
                        // In case getting the application state failed because of a network failure, you could add retry logic
                        rootPage.NotifyUser("Application was already stopped.", NotifyType.StatusMessage);
                        break;
                    }
                default:
                    {
                        DialAppStopResult result = await app.StopAsync();

                        if (result == DialAppStopResult.Stopped)
                        {
                            stopped = true;
                            // In case getting the application state failed because of a network failure, you could add retry logic
                            rootPage.NotifyUser("Application stopped successfully.", NotifyType.StatusMessage);
                        }
                        else
                        {
                            if (result == DialAppStopResult.StopFailed || result == DialAppStopResult.NetworkFailure)
                            {
                                // In case getting the application state failed because of a network failure, you could add retry logic
                                rootPage.NotifyUser(string.Format("Error occured trying to stop application. Status: '{0}'", result.ToString()), NotifyType.StatusMessage);
                            }
                            else //in case of DialAppStopResult.OperationNotSupported, there is not much more you can do. You could implement your own
                            // mechanism to stop the application on that device.
                            {
                                stopped = true;
                                // In case getting the application state failed because of a network failure, you could add retry logic
                                rootPage.NotifyUser(string.Format("Stop is not supported by device: '{0}'", activeDevice.Name), NotifyType.ErrorMessage);
                            }
                        }
                        break;
                    }
            }
            return stopped;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

    }
}
