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
using CombineCastingTech.Data.Azure;
using System.Collections.Generic;
using CombineCastingTech.Data.Common;
using Windows.Devices.Enumeration;
using System.Threading.Tasks;
using Windows.Media.Casting;

namespace CombineCastingTech
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

            //Initialize our picker object
            picker = new DevicePicker();

            // Get the list of available Azure videos.
            AzureDataProvider dataProvider = new AzureDataProvider();
            List<VideoMetaData> videos = dataProvider.GetAll(MAX_RESULTS);
            Random indexRandomizer = new Random();
            // Pick a random video
            video = videos[indexRandomizer.Next(0, videos.Count- 1)];
            player.MediaOpened += Player_MediaOpened;
            player.MediaFailed += Player_MediaFailed;
            //Set the source on the player
            player.AutoPlay = true;

            player.Source = video.VideoLink;

            this.dial_appname_textbox.Text = "CombineCastingTech";
            this.dial_launch_args_textbox.Text = string.Format("v={0}&t=0&pairingCode=E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F", video.Id);

            //Hook up device selected event
            picker.DeviceSelected += Picker_DeviceSelected;

            //Hook up device disconnected event
            picker.DisconnectButtonClicked += Picker_DisconnectButtonClicked1;
            //Start playing the video
        }

        private void Player_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            rootPage.NotifyUser("Media Failed", NotifyType.ErrorMessage);
        }

        private void Player_MediaOpened(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Media Openend", NotifyType.StatusMessage);

            player.Play();
        }

        private async void Picker_DisconnectButtonClicked1(DevicePicker sender, DeviceDisconnectButtonClickedEventArgs args)
        {
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                //Update the display status for the selected device.
                sender.SetDisplayStatus(args.Device, "Disconnecting", DevicePickerDisplayStatusOptions.ShowProgress);

                bool disconnected = false;

                if (this.activeCastConnectionHandler is DialApp)
                    disconnected = await TryStopDialAppAsync((DialApp)activeCastConnectionHandler, args.Device);

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
            });
        }

        private async Task<bool> TryCastMediaElementAsync(DeviceInformation device)
        {
            bool castMediaElementSucceeded = false;

            return castMediaElementSucceeded;
        }
        private async Task<bool> TryLaunchDialAppAsync(DeviceInformation device)
        {
            bool dialAppLaunchSucceeded = false;
            DialDevice dialDevice = null;

            //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
            rootPage.NotifyUser(string.Format("Attempting to resolve DIAL device for {0}", device.Name), NotifyType.StatusMessage);
            try { dialDevice = await DialDevice.FromIdAsync(device.Id); } catch { }

            if (dialDevice == null)
            {
                //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
                rootPage.NotifyUser(string.Format("{0} does not support DIAL", device.Name), NotifyType.StatusMessage);
            }
            else
            { 
                //Get the DialApp object for the specific application on the selected device
                DialApp app = dialDevice.GetDialApp(this.dial_appname_textbox.Text);

                if (app == null)
                {
                    //Try to create a DIAL device. If it doesn't succeed, the selected device does not support DIAL.
                    rootPage.NotifyUser(string.Format("{0} cannot find app with ID '{1}'", device.Name, this.dial_appname_textbox.Text), NotifyType.StatusMessage);
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
                        rootPage.NotifyUser(string.Format("Attempting to launch {0}", app.AppName), NotifyType.StatusMessage);
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
                            rootPage.NotifyUser(string.Format("Launched {0}", app.AppName), NotifyType.StatusMessage);
                            //This is where you will need to add you application specific communication between your 1st and 2nd screen applications
                            //...
                            dialAppLaunchSucceeded = true;
                        }
                    }
                }
            }
            return dialAppLaunchSucceeded;
        }
        private async void Picker_DeviceSelected(DevicePicker sender, DeviceSelectedEventArgs args)
        {
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                //Update the display status for the selected device to connecting.
                picker.SetDisplayStatus(args.SelectedDevice, "Connecting", DevicePickerDisplayStatusOptions.ShowProgress);

                //Try casting using DIAL first
                bool castSucceeded = await TryLaunchDialAppAsync(args.SelectedDevice);
                //If DIAL did not work for the selected device, try the CAST API
                if (!castSucceeded)
                {
                    castSucceeded = await TryCastMediaElementAsync(args.SelectedDevice);
                }

                if (castSucceeded)
                {
                    //Update the display status for the selected device.
                    picker.SetDisplayStatus(args.SelectedDevice, "Connected", DevicePickerDisplayStatusOptions.ShowDisconnectButton);
                    // Hide the picker now that all the work is completed
                    picker.Hide();
                }
                else
                {
                    //Show a retry button when connecting to the selected device failed.
                    picker.SetDisplayStatus(args.SelectedDevice, "Connecting failed", DevicePickerDisplayStatusOptions.ShowRetryButton);
                }
            });
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
                                rootPage.NotifyUser(string.Format("Error occured trying to stop application. Status: {0}", result.ToString()), NotifyType.StatusMessage);
                                //Update the display status for the selected device.
                                picker.SetDisplayStatus(device, "Disconnect Failed", DevicePickerDisplayStatusOptions.ShowDisconnectButton);
                            }
                            else //in case of DialAppStopResult.OperationNotSupported, there is not much more you can do. You could implement your own
                            // mechanism to stop the application on that device.
                            {
                                stopped = true;
                                // In case getting the application state failed because of a network failure, you could add retry logic
                                rootPage.NotifyUser(string.Format("Stop is not supported by device: {0}", device.Name), NotifyType.ErrorMessage);
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

        private async void castButton_Click(object sender, RoutedEventArgs e)
        {
            //Retrieve the location of the casting button
            GeneralTransform transform = castButton.TransformToVisual(Window.Current.Content as UIElement);
            Point pt = transform.TransformPoint(new Point(0, 0));
            //Add the DIAL Filter, so that the application only shows DIAL devices that have the application installed or advertise that they can install them.
            picker.Filter.SupportedDeviceSelectors.Add(DialDevice.GetDeviceSelector(this.dial_appname_textbox.Text));
            //Add the CAST API Filter, so that the application only shows Miracast, Bluetooth, DLNA devices that can render the video
            picker.Filter.SupportedDeviceSelectors.Add(await CastingDevice.GetDeviceSelectorFromCastingSourceAsync(player.GetAsCastingSource()));

            //if (activeDevice != null)
            //    //Update the display status for the previously selected device.
            //    picker.SetDisplayStatus(activeDevice, "Connected", DevicePickerDisplayStatusOptions.ShowDisconnectButton);

            //Show the picker above our Show Device Picker button
            picker.Show(new Rect(pt.X, pt.Y, castButton.ActualWidth, castButton.ActualHeight), Windows.UI.Popups.Placement.Above);

            rootPage.NotifyUser("Show Device Picker Button Clicked", NotifyType.StatusMessage);
        }       
    }
}
