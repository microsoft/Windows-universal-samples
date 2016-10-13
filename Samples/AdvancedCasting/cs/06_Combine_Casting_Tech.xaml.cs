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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Media;
using Windows.Media.DialProtocol;
using ScreenCasting.Data.Azure;
using ScreenCasting.Data.Common;
using Windows.Devices.Enumeration;
using System.Threading.Tasks;
using Windows.Media.Casting;
using ScreenCasting.Controls;
using ScreenCasting.Util;
using Windows.UI.ViewManagement;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.Storage;

namespace ScreenCasting
{
    public sealed partial class Scenario06 : Page
    {
        private const int MAX_RESULTS = 10;

        private MainPage rootPage;
        private DevicePicker picker = null;
        private DeviceInformation activeDevice = null;
        private object activeCastConnectionHandler = null;
        private VideoMetaData video = null;
        int thisViewId;

        public Scenario06()
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

            //Configure the DIAL launch arguments for the current video
            this.dial_launch_args_textbox.Text = string.Format("v={0}&t=0&pairingCode=E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F", video.Id);

            //Subscribe for the clicked event on the custom cast button
            ((MediaTransportControlsWithCustomCastButton)this.player.TransportControls).CastButtonClicked += TransportControls_CastButtonClicked;

            // Instantiate the Device Picker
            picker = new DevicePicker();

            //Hook up device selected event
            picker.DeviceSelected += Picker_DeviceSelected;

            //Hook up device disconnected event
            picker.DisconnectButtonClicked += Picker_DisconnectButtonClicked;

            //Hook up device disconnected event
            picker.DevicePickerDismissed += Picker_DevicePickerDismissed;

            //Add the DIAL Filter, so that the application only shows DIAL devices that have the application installed or advertise that they can install them.            
            //BUG: picker.Filter.SupportedDeviceSelectors.Add(DialDevice.GetDeviceSelector(this.dial_appname_textbox.Text));            
            picker.Filter.SupportedDeviceSelectors.Add("System.Devices.DevObjectType:=6 AND System.Devices.AepContainer.ProtocolIds:~~{0E261DE4-12F0-46E6-91BA-428607CCEF64} AND System.Devices.AepContainer.Categories:~~Multimedia.ApplicationLauncher.DIAL");

            //Add the CAST API Filter, so that the application only shows Miracast, Bluetooth, DLNA devices that can render the video
            // BUG: picker.Filter.SupportedDeviceSelectors.Add(await CastingDevice.GetDeviceSelectorFromCastingSourceAsync(player.GetAsCastingSource()));
            // BUG: picker.Filter.SupportedDeviceSelectors.Add(CastingDevice.GetDeviceSelector(CastingPlaybackTypes.Video));
            picker.Filter.SupportedDeviceSelectors.Add("System.Devices.InterfaceClassGuid:=\"{D0875FB4-2196-4c7a-A63D-E416ADDD60A1}\"" + " AND System.Devices.InterfaceEnabled:=System.StructuredQueryType.Boolean#True");
            
            //Add projection manager filter
            picker.Filter.SupportedDeviceSelectors.Add(ProjectionManager.GetDeviceSelector());

            pvb.ProjectionStopping += Pvb_ProjectionStopping;
        }

        ProjectionViewBroker pvb = new ProjectionViewBroker();

        private void TransportControls_CastButtonClicked(object sender, EventArgs e)
        {
            //Pause Current Playback
            player.Pause();

            rootPage.NotifyUser("Show Device Picker Button Clicked", NotifyType.StatusMessage);

            //Get the custom transport controls
            MediaTransportControlsWithCustomCastButton mtc = (MediaTransportControlsWithCustomCastButton)this.player.TransportControls;
            //Retrieve the location of the casting button
            GeneralTransform transform = mtc.CastButton.TransformToVisual(Window.Current.Content as UIElement);
            Point pt = transform.TransformPoint(new Point(0, 0));

            //Show the picker above our Show Device Picker button
            picker.Show(new Rect(pt.X, pt.Y, mtc.CastButton.ActualWidth, mtc.CastButton.ActualHeight), Windows.UI.Popups.Placement.Above);
        }

        #region Windows.Devices.Enumeration.DevicePicker Methods
        private async void Picker_DeviceSelected(DevicePicker sender, DeviceSelectedEventArgs args)
        {
            string deviceId = args.SelectedDevice.Id;

            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                //Update the display status for the selected device to connecting.
                try { picker.SetDisplayStatus(args.SelectedDevice, "Connecting", DevicePickerDisplayStatusOptions.ShowProgress); } catch { }

                //The selectedDeviceInfo instance is needed to be able to update the picker.
                DeviceInformation selectedDeviceInfo = args.SelectedDevice;
#if DEBUG
                // The args.SelectedCastingDevice is proxied from the picker process. The picker process is 
                // dismissmed as soon as you break into the debugger. Creating a non-proxied version 
                // allows debugging since the proxied version stops working once the picker is dismissed.
                selectedDeviceInfo = await DeviceInformation.CreateFromIdAsync(args.SelectedDevice.Id);
#endif

                bool castSucceeded = false;

                // If the ProjectionManager API did not work and the device id will have 'dial' in it.
                castSucceeded = await TryLaunchDialAppAsync(selectedDeviceInfo);

                // If it doesn't try the ProjectionManager API.
                if (!castSucceeded)
                    castSucceeded = await TryProjectionManagerCastAsync(selectedDeviceInfo);

                //If DIAL and ProjectionManager did not work for the selected device, try the CAST API
                if (!castSucceeded)
                    castSucceeded = await TryCastMediaElementAsync(selectedDeviceInfo);

                if (castSucceeded)
                {
                    //Update the display status for the selected device.  Try Catch in case the picker is not visible anymore.
                    try { picker.SetDisplayStatus(selectedDeviceInfo, "Connected", DevicePickerDisplayStatusOptions.ShowDisconnectButton); } catch { }

                    // Hide the picker now that all the work is completed. Try Catch in case the picker is not visible anymore.
                    try { picker.Hide(); } catch { }
                }
                else
                {
                    //Show a retry button when connecting to the selected device failed.
                    try { picker.SetDisplayStatus(selectedDeviceInfo, "Connecting failed", DevicePickerDisplayStatusOptions.ShowRetryButton); } catch { }
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
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                rootPage.NotifyUser("Disconnect Button clicked", NotifyType.StatusMessage);

                //Update the display status for the selected device.
                sender.SetDisplayStatus(args.Device, "Disconnecting", DevicePickerDisplayStatusOptions.ShowProgress);

                bool disconnected = false;
                if (this.activeCastConnectionHandler is ProjectionViewBroker)
                    disconnected = TryStopProjectionManagerAsync((ProjectionViewBroker)activeCastConnectionHandler);
                if (this.activeCastConnectionHandler is DialApp)
                    disconnected = await TryStopDialAppAsync((DialApp)activeCastConnectionHandler);
                if (this.activeCastConnectionHandler is CastingConnection)
                    disconnected = await TryDisconnectCastingSessionAsync((CastingConnection)activeCastConnectionHandler);

                if (disconnected)
                {
                    //Update the display status for the selected device.
                    try { sender.SetDisplayStatus(args.Device, "Disconnected", DevicePickerDisplayStatusOptions.None); } catch { }
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

        #endregion

        #region ProjectionManager APIs
        private async Task<bool> TryProjectionManagerCastAsync(DeviceInformation device)
        {
            bool projectionManagerCastAsyncSucceeded = false;

            if ((activeDevice ==null && ProjectionManager.ProjectionDisplayAvailable && device == null) || device != null)
            {
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
                        rootPage.NotifyUser(string.Format("Starting projection of '{0}' on a second view '{1}' using ProjectionManager", video.Title, device.Name), NotifyType.StatusMessage);

                        await ProjectionManager.StartProjectingAsync(rootPage.ProjectionViewPageControl.Id, thisViewId, device);
                    }
                    catch (Exception ex)
                    {
                        if (!ProjectionManager.ProjectionDisplayAvailable)
                            throw ex;
                    }

                    if (pvb.ProjectedPage != null)
                    {
                        this.player.Pause();
                        await pvb.ProjectedPage.SetMediaSource(this.player.Source, this.player.Position);
                    }
                    if (device != null)
                    {
                        activeDevice = device;
                        activeCastConnectionHandler = pvb;
                    }
                    projectionManagerCastAsyncSucceeded = true;

                    rootPage.NotifyUser(string.Format("Displaying '{0}' on a second view '{1}' using ProjectionManager", video.Title, device.Name), NotifyType.StatusMessage);
                }
                catch (Exception)
                {
                    rootPage.NotifyUser("The projection view is being disposed", NotifyType.ErrorMessage);
                }
                ApplicationView.GetForCurrentView().ExitFullScreenMode();
            }
                return projectionManagerCastAsyncSucceeded;
        }
        private bool TryStopProjectionManagerAsync(ProjectionViewBroker broker)
        {
            broker.ProjectedPage.StopProjecting();
            return true;
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

        #endregion      

        #region Windows.Media.Casting APIs

        private async Task<bool> TryCastMediaElementAsync(DeviceInformation device)
        {
            bool castMediaElementSucceeded = false;

            //Verify whether the selected device supports DLNA, Bluetooth, or Miracast.
            rootPage.NotifyUser(string.Format("Checking to see if device {0} supports Miracast, Bluetooth, or DLNA", device.Name), NotifyType.StatusMessage);

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
            }
            return castMediaElementSucceeded;
        }
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
                rootPage.NotifyUser("Casting Error Occured: " + args.Message, NotifyType.ErrorMessage);
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

        #endregion

        #region Windows.Media.DialProtocol APIs
        private async Task<bool> TryLaunchDialAppAsync(DeviceInformation device)
        {            
            bool dialAppLaunchSucceeded = false;

            //Update the launch arguments to include the Position
            this.dial_launch_args_textbox.Text = string.Format("v={0}&t={1}&pairingCode=E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F", video.Id, player.Position.Ticks);

            //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
            rootPage.NotifyUser(string.Format("Checking to see if device {0} supports DIAL", device.Name), NotifyType.StatusMessage);

            DialDevice dialDevice = null;

            //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
            rootPage.NotifyUser(string.Format("Attempting to resolve DIAL device for '{0}'", device.Name), NotifyType.StatusMessage);

            string[] newIds = (string[])device.Properties["{0BBA1EDE-7566-4F47-90EC-25FC567CED2A} 2"];

            if (newIds.Length > 0)
            {
                string deviceId = newIds[0];
                try { dialDevice = await DialDevice.FromIdAsync(deviceId); } catch { }
            }

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

        #endregion

        #region Media Element Status Methods
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
