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
using SDKTemplate;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Media;
using Windows.Media.DialProtocol;

namespace DIALMediaCasting
{
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;
        private DialDevicePicker picker;
        private DialDevice activeDialDevice = null;

        public Scenario1()
        {
            this.InitializeComponent();

            //Initialize our picker object
            picker = new DialDevicePicker();

            this.dial_appname_textbox.Text = "dialmediacasting";
            this.dial_launch_args_textbox.Text = "v=AzureMediaServicesPromo.ism&t=0&pairingCode={E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F}";
            
            //Hook up device selected event
            picker.DialDeviceSelected += Picker_DialDeviceSelected; ;

            //Hook up device disconnected event
            picker.DisconnectButtonClicked += Picker_DisconnectButtonClicked;
        }

        private async void Picker_DialDeviceSelected(DialDevicePicker sender, DialDeviceSelectedEventArgs args)
        {
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                //Get the DialApp object for the specific application on the selected device
                DialApp app = args.SelectedDialDevice.GetDialApp(this.dial_appname_textbox.Text);

                //Update the display status for the selected device to connecting.
                picker.SetDisplayStatus(args.SelectedDialDevice, DialDeviceDisplayStatus.Connecting);

                //Get the current application state
                DialAppStateDetails stateDetails = await app.GetAppStateAsync();
                if (stateDetails.State == DialAppState.NetworkFailure)
                {
                    //Update the display status for the selected device.
                    picker.SetDisplayStatus(args.SelectedDialDevice, DialDeviceDisplayStatus.Error);

                    // In case getting the application state failed because of a network failure, you could add retry logic
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
                        //Update the display status for the selected device.
                        picker.SetDisplayStatus(args.SelectedDialDevice, DialDeviceDisplayStatus.Connected);

                        //Remember the selected device
                        activeDialDevice = args.SelectedDialDevice;

                        rootPage.NotifyUser(string.Format("Launched {0}", app.AppName), NotifyType.StatusMessage);
                        //This is where you will need to add you application specific communication between your 1st and 2nd screen applications
                        //...

                    }
                    else
                    {
                        //Update the display status for the selected device.
                        picker.SetDisplayStatus(args.SelectedDialDevice, DialDeviceDisplayStatus.Error);

                        // In case launch failed, you could add retry logic                       
                        rootPage.NotifyUser(string.Format("Failed to launch application: {0}. Reason: ", app.AppName, result.ToString()), NotifyType.ErrorMessage);
                    }
                }
                // Hide the picker now that all the work is completed
                picker.Hide();
            });
        }

        private async void Picker_DisconnectButtonClicked(DialDevicePicker sender, DialDisconnectButtonClickedEventArgs args)
        {
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {

                //Update the display status for the selected device.
                sender.SetDisplayStatus(args.Device, DialDeviceDisplayStatus.Disconnecting);

                //Get the DialApp object for the specific application on the device we are disconnecting from
                DialApp app = args.Device.GetDialApp(this.dial_appname_textbox.Text);

                //Get the current application state
                DialAppStateDetails stateDetails = await app.GetAppStateAsync();

                switch (stateDetails.State)
                {
                    case DialAppState.NetworkFailure:
                        {
                            //Update the display status for the selected device.
                            picker.SetDisplayStatus(args.Device, DialDeviceDisplayStatus.Error);

                            // In case getting the application state failed because of a network failure, you could add retry logic
                            rootPage.NotifyUser("Network Failure while getting application state", NotifyType.ErrorMessage);
                            break;
                        }
                    case DialAppState.Stopped:
                        {
                            //Update the display status for the selected device.
                            picker.SetDisplayStatus(args.Device, DialDeviceDisplayStatus.Disconnected);

                            // In case getting the application state failed because of a network failure, you could add retry logic
                            rootPage.NotifyUser("Application was already stopped.", NotifyType.StatusMessage);
                            break;
                        }
                    default:
                        {
                            DialAppStopResult result = await app.StopAsync();

                            if (result == DialAppStopResult.Stopped)
                            {
                                //Update the display status for the selected device.
                                picker.SetDisplayStatus(args.Device, DialDeviceDisplayStatus.Disconnected);
                                // In case getting the application state failed because of a network failure, you could add retry logic
                                rootPage.NotifyUser("Application stopped successfully.", NotifyType.StatusMessage);
                            }
                            else
                            { 
                                // In case getting the application state failed because of a network failure, you could add retry logic
                                rootPage.NotifyUser(string.Format("Issue occured trying to stop application. Status: {0}", result.ToString()), NotifyType.StatusMessage);
                                //Update the display status for the selected device.
                                picker.SetDisplayStatus(args.Device, DialDeviceDisplayStatus.Error);
                            }

                            break;
                        }
                }
            });
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private void castButton_Click(object sender, RoutedEventArgs e)
        {
            //Retrieve the location of the casting button
            GeneralTransform transform = castButton.TransformToVisual(Window.Current.Content as UIElement);
            Point pt = transform.TransformPoint(new Point(0, 0));
            //Specify the filter, so that the application only shows DIAL devices that have the application installed or advertise that they can install them.
            picker.Filter.SupportedAppNames.Add(this.dial_appname_textbox.Text);

            //if (activeDialDevice != null)
                //Update the display status for the previously selected device.
                //picker.SetDisplayStatus(activeDialDevice, DialDeviceDisplayStatus.Connected);

            //Show the picker above our Show Device Picker button
            picker.Show(new Rect(pt.X, pt.Y, castButton.ActualWidth, castButton.ActualHeight), Windows.UI.Popups.Placement.Above);

            rootPage.NotifyUser("Show Device Picker Button Clicked", NotifyType.StatusMessage);
        }       
    }
}
