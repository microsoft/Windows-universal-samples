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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Media.DialProtocol;
using Windows.Devices.Enumeration;
using ScreenCasting.Data.Common;
using Windows.Media.Casting;
using System.Threading.Tasks;
using ScreenCasting.Data.Azure;
using System.Collections.Generic;
using ScreenCasting.Util;
using Windows.UI.Xaml.Media;
using System.Reflection;
using Windows.UI.ViewManagement;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;

namespace ScreenCasting
{
    public sealed partial class Scenario3 : Page
    {
        private const int MAX_RESULTS = 10;

        private MainPage rootPage;
        private DeviceWatcher watcher;
        private DeviceInformation activeDevice = null;
        private object activeCastConnectionHandler = null;
        private VideoMetaData video = null;

        public Scenario3()
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

            //Set the source on the player
            rootPage.NotifyUser(string.Format("Opening '{0}'", video.Title), NotifyType.StatusMessage);
            player.Source = video.VideoLink;

            this.dial_launch_args_textbox.Text = string.Format("v={0}&t=0&pairingCode=E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F", video.Id);
        }

        private async void castingDevicesList_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            if (castingDevicesList.SelectedItem != null)
            {
                //Get the selected DeviceInformation obj before we stop the watcher and clear the list
                DeviceInformation selectedDeviceInfo = (DeviceInformation)castingDevicesList.SelectedItem;

                //When a device is selected, first thing we do is stop the watcher so it's search doesn't conflict with streaming
                if (watcher != null && watcher.Status != DeviceWatcherStatus.Stopped)
                {
                    progressText.Text = "Stopping watcher";
                    StopCurrentWatcher();
                }

                //Update the display status for the selected device to connecting.
                rootPage.NotifyUser("Connecting", NotifyType.StatusMessage);


                //Try casting using DIAL first
                bool castSucceeded = await TryLaunchDialAppAsync(selectedDeviceInfo);

                if (!castSucceeded) castSucceeded = await TryProjectionManagerCastAsync(selectedDeviceInfo);

                //If neither dial and projectionamanger worked for the selected device, try the CAST API
                if (!castSucceeded)
                    castSucceeded = await TryCastMediaElementAsync(selectedDeviceInfo);

                if (castSucceeded)
                {
                    //Update the display status for the selected device.
                    rootPage.NotifyUser(string.Format("Casting '{0}'", video.Title), NotifyType.StatusMessage);

                    disconnectButton.IsEnabled = true;
                }
                else
                {
                    //Show a retry button when connecting to the selected device failed.
                    rootPage.NotifyUser(string.Format("Casting {0} failed to device {1}", video.Title, selectedDeviceInfo.Name), NotifyType.ErrorMessage);
                }
            }
        }

        private async void disconnectButton_Click(object sender, RoutedEventArgs e)
        {
            //Update the display status for the selected device.
            rootPage.NotifyUser(string.Format("Disconnecting : '{0}'", activeDevice.Name), NotifyType.StatusMessage);

            bool disconnected = false;

            if (this.activeCastConnectionHandler is DialApp)
                disconnected = await TryStopDialAppAsync((DialApp)activeCastConnectionHandler, activeDevice);
            if (this.activeCastConnectionHandler is CastingConnection)
                disconnected = await TryDisconnectCastingSessionAsync((CastingConnection)activeCastConnectionHandler, activeDevice);

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

        private async Task<bool> TryDisconnectCastingSessionAsync(CastingConnection connection, DeviceInformation device)
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
        private async Task<bool> TryStopDialAppAsync(DialApp app, DeviceInformation device)
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
                                rootPage.NotifyUser(string.Format("Stop is not supported by device: '{0}'", device.Name), NotifyType.ErrorMessage);
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

        #region Watcher Events

        private async void Watcher_Added(DeviceWatcher sender, DeviceInformation args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                //Add each discovered device to our listbox
                castingDevicesList.Items.Add(args);
            });
        }

        private async void Watcher_Removed(DeviceWatcher sender, DeviceInformationUpdate args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                //Remove any removed devices from our listbox
                DeviceInformation removedDevice = null;

                foreach (DeviceInformation currentDevice in this.castingDevicesList.Items)
                {
                    if (currentDevice.Id == args.Id)
                    {
                        removedDevice = currentDevice;
                        break;
                    }
                }
                this.castingDevicesList.Items.Remove(removedDevice);
            });
        }

        private async void Watcher_EnumerationCompleted(DeviceWatcher sender, object args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                //If enumeration completes, update UI and transition watcher to the stopped state
                rootPage.NotifyUser("Watcher completed enumeration of devices", NotifyType.StatusMessage);
                progressText.Text = "Enumeration Completed";
                watcher.Stop();
            });
        }

        private async void Watcher_Stopped(DeviceWatcher sender, object args)
        {
            //unsubscribe from the stopped event
            sender.Stopped -= Watcher_Stopped;

            //Setting the watcher to null will allow StopCurrentWatcher to continue on the UI thread.
            watcher = null;

            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                //Update UX when the watcher stops
                rootPage.NotifyUser("Watcher has been stopped", NotifyType.StatusMessage);
                progressText.Text = "";
                watcherControlButton.Content = "Restart Device Watcher";
                progressRing.IsActive = false;
            });
        }

        private async void Watcher_Updated(DeviceWatcher sender, DeviceInformationUpdate args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                //If enumeration completes, update UI and transition watcher to the stopped state
                rootPage.NotifyUser("Watcher received an update for a listed device", NotifyType.StatusMessage);

                //Go through our list of device until you find the DeviceInformation object that is updated.
                DeviceInformation updateDevice = null;
                foreach (DeviceInformation currentDevice in this.castingDevicesList.Items)
                {
                    if (currentDevice.Id == args.Id)
                    {
                        updateDevice = currentDevice;
                        break;
                    }
                }
                //Update the DeviceInformation object.
                if (updateDevice != null)
                    updateDevice.Update(args);
            });
        }

        #endregion

        #region Watcher Start and Stop
        private void StopCurrentWatcher()
        {
            if (watcher != null)
            {
                //Unregister from the watcher events before stopping.
                watcher.Added -= Watcher_Added;
                watcher.Removed -= Watcher_Removed;
                watcher.EnumerationCompleted -= Watcher_EnumerationCompleted;

                //If the watcher isn't running, start it up
                if (watcher.Status != DeviceWatcherStatus.Stopping)
                {
                    //clear the list as we're starting the watcher over
                    castingDevicesList.Items.Clear();

                    //Stop the current watcher
                    watcher.Stop();

                    //Hold this thread until Watcher.Stopped has completed
                    System.Threading.SpinWait.SpinUntil(() =>
                                {
                                    return watcher == null;
                                }
                        );
                }
            }
        }

        private async void watcherControlButton_Click(object sender, RoutedEventArgs e)
        {
            //Pause the video in the local player
            this.player.Pause();

            StopCurrentWatcher();

            this.castingDevicesList.Items.Clear();

            CustomDevicePickerFilter filter = new CustomDevicePickerFilter();
            //Add the CAST API Filter, so that the application only shows Miracast, Bluetooth, DLNA devices that can render the video
            //filter.SupportedDeviceSelectors.Add(await CastingDevice.GetDeviceSelectorFromCastingSourceAsync(this.player.GetAsCastingSource()));
            //Add the DIAL Filter, so that the application only shows DIAL devices that have the application installed or advertise that they can install them.
            filter.SupportedDeviceSelectors.Add(DialDevice.GetDeviceSelector(this.dial_appname_textbox.Text));
            filter.SupportedDeviceSelectors.Add(ProjectionManager.GetDeviceSelector());
            filter.SupportedDeviceSelectors.Add("System.Devices.InterfaceClassGuid:=\"{D0875FB4-2196-4c7a-A63D-E416ADDD60A1}\"" + " AND System.Devices.InterfaceEnabled:=System.StructuredQueryType.Boolean#True");

            //Create our watcher and have it find casting devices capable of video casting
            watcher = DeviceInformation.CreateWatcher(filter.ToString());

            //Register for watcher events
            watcher.Added += Watcher_Added;
            watcher.Removed += Watcher_Removed;
            watcher.Stopped += Watcher_Stopped;
            watcher.Updated += Watcher_Updated;
            watcher.EnumerationCompleted += Watcher_EnumerationCompleted;

            //start the watcher
            watcher.Start();

            //update the UI to reflect the watcher's state
            rootPage.NotifyUser("Watcher has been started", NotifyType.StatusMessage);
            progressText.Text = "Searching";
            progressRing.IsActive = true;
        }

        #endregion

        ProjectionViewBroker pvb = new ProjectionViewBroker();
        int thisViewId;

        #region ProjectionManager APIs
        private async Task<bool> TryProjectionManagerCastAsync(DeviceInformation device)
        {
            bool projectionManagerCastAsyncSucceeded = false;

            if ((activeDevice == null && ProjectionManager.ProjectionDisplayAvailable && device == null) || device != null)
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
                        pvb.ProjectionStopping += Pvb_ProjectionStopping;

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
                        //if (ProjectionManager.ProjectionDisplayAvailable)
                        //{
                        //    // Show the view on a second display (if available) or on the primary display
                        //    await ProjectionManager.StartProjectingAsync(rootPage.ProjectionViewPageControl.Id, thisViewId);
                        //}
                        //else
                        //{
                        await ProjectionManager.StartProjectingAsync(rootPage.ProjectionViewPageControl.Id, thisViewId, device);

                        //}
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
        private async void Pvb_ProjectionStarted(object sender, EventArgs e)
        {
            //await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            //{
            //    //rootPage.NotifyUser("Moving Media Element to the second screen", NotifyType.StatusMessage);
            //    //ProjectionViewBroker broker = sender as ProjectionViewBroker;
            //});

        }

        private async void Pvb_ProjectionStopping(object sender, EventArgs e)
        {
            ProjectionViewBroker broker = sender as ProjectionViewBroker;

            TimeSpan position = broker.ProjectedPage.Player.Position;
            Uri source = broker.ProjectedPage.Player.Source;

            await rootPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {

                rootPage.NotifyUser("Moving Media Element back to the first screen", NotifyType.StatusMessage);


                this.player.Position = position;
                this.player.Source = source;
                this.player.Play();
            });

        }

        #endregion

        #region Media Element Casting Methods
        private async Task<bool> TryCastMediaElementAsync(DeviceInformation device)
        {
            bool castMediaElementSucceeded = false;

            rootPage.NotifyUser(string.Format("Checking to see if device {0} supports Miracast, Bluetooth, or DLNA", device.Name), NotifyType.StatusMessage);
            if (await CastingDevice.DeviceInfoSupportsCastingAsync(device))
            {
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
                        }
                        else
                        {
                            rootPage.NotifyUser(string.Format("Failed to cast to '{0}'", device.Name), NotifyType.ErrorMessage);
                        }
                    }
                }
            }
            else
            {
                rootPage.NotifyUser(string.Format("'{0}' does not support Miracast, Bluetooth, or DLNA", device.Name), NotifyType.StatusMessage);
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
                disconnectButton.IsEnabled = false;

                rootPage.NotifyUser("Casting Error Occured: " + args.Message, NotifyType.ErrorMessage);
            });
        }

        #endregion

        #region DIAL Casting Methods
        private async Task<bool> TryLaunchDialAppAsync(DeviceInformation device)
        {
            bool dialAppLaunchSucceeded = false;
            //Update the launch arguments to include the Position
            this.dial_launch_args_textbox.Text = string.Format("v={0}&t={1}&pairingCode=E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F", video.Id, player.Position.Ticks);

            //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
            rootPage.NotifyUser(string.Format("Checking to see if device {0} supports DIAL", device.Name), NotifyType.StatusMessage);
            if (await DialDevice.DeviceInfoSupportsDialAsync(device))
            {
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
            }
            else
            {
                rootPage.NotifyUser(string.Format("'{0}' does not support DIAL", device.Name), NotifyType.StatusMessage);
            }
            return dialAppLaunchSucceeded;
        }

        #endregion

        #region Media Element Status Changes
        private void Player_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            // As long as the video is ready to play and more importantly get a casting source
            // allow the watcher to be started
            if (
                this.player.CurrentState == MediaElementState.Playing
                 || this.player.CurrentState == MediaElementState.Paused
                 || this.player.CurrentState == MediaElementState.Buffering
                 )
            {
                this.watcherControlButton.IsEnabled = true;
            }
            else
            {
                this.watcherControlButton.IsEnabled = false;
                StopCurrentWatcher();
            }

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
