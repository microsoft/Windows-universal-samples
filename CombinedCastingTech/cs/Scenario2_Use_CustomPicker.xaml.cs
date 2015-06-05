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
using CombineCastingTech;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Media.DialProtocol;
using Windows.Devices.Enumeration;

namespace CombineCastingTech
{
    public sealed partial class Scenario2 : Page
    {
        private MainPage rootPage;
        private DeviceWatcher watcher;

        public Scenario2()
        {
            this.InitializeComponent();

            this.dial_appname_textbox.Text = "CombineCastingTech";
            this.dial_launch_args_textbox.Text = "v=AzureMediaServicesPromo.ism&t=0&pairingCode={E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F}";
        }

        private async void Picker_DialDeviceSelected(DialDevicePicker sender, DialDeviceSelectedEventArgs args)
        {
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                //Get the DialApp object for the specific application on the selected device
                DialApp app = args.SelectedDialDevice.GetDialApp(this.dial_appname_textbox.Text);
                DialAppStateDetails stateDetails = await app.GetAppStateAsync();
                if (stateDetails.State == DialAppState.NetworkFailure)
                {
                    // In case getting the application state failed because of a network failure, you could add retry logic
                    rootPage.NotifyUser("Network Failure while getting application state", NotifyType.ErrorMessage);
                }
                else
                {
                    DialAppLaunchResult result = await app.RequestLaunchAsync(this.dial_launch_args_textbox.Text);
                    //Verify to see whether the application was launched
                    if (result == DialAppLaunchResult.Launched)
                    {
                        rootPage.NotifyUser(string.Format("Launched {0}", app.AppName), NotifyType.StatusMessage);
                        //This is where you will need to add you application specific communication between your 1st and 2nd screen applications
                        //...
                    }
                    else
                    {
                        // In case launch failed, you could add retry logic                       
                        rootPage.NotifyUser(string.Format("Failed to launch application: {0}. Reason: ", app.AppName, result.ToString()), NotifyType.ErrorMessage);
                    }
                }
            });
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

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

        private void Watcher_Stopped(DeviceWatcher sender, object args)
        {
            //unsubscribe from the stopped event
            sender.Stopped -= Watcher_Stopped;

            //Setting the watcher to null will allow StopCurrentWatcher to continue on the UI thread.
            watcher = null;
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

                    //Update UX when the watcher stops
                    rootPage.NotifyUser("Watcher has been stopped", NotifyType.StatusMessage);
                    progressRing.IsActive = false;
                }
            }
        }
        private void watcherControlButton_Click(object sender, RoutedEventArgs e)
        {
            StopCurrentWatcher();

            //Create our watcher and have it find casting devices capable of video casting
            watcher = DeviceInformation.CreateWatcher(DialDevice.GetDeviceSelector(this.dial_appname_textbox.Text));

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

        private async void castingDevicesList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

            if (castingDevicesList.SelectedItem != null)
            {
                //Get the selected DeviceInformation obj before we stop the watcher and clear the list
                DeviceInformation selectedDeviceInfo = (DeviceInformation)castingDevicesList.SelectedItem;

                //When a device is selected, first thing we do is stop the watcher so it's search doesn't conflict with streaming
                if (watcher.Status != DeviceWatcherStatus.Stopped)
                {
                    progressText.Text = "";
                    StopCurrentWatcher();
                }

                //Get the DIAL Device for the selected DeviceInformation. This call will fail if the selected device is not a DIAL device.
                DialDevice selectedDevice = await DialDevice.FromIdAsync(selectedDeviceInfo.Id);
                //Update the status
                rootPage.NotifyUser(string.Format("Attempting to launch {0}", this.dial_appname_textbox.Text), NotifyType.StatusMessage);

                //Get the DialApp object for the specific application on the selected device
                DialApp app = selectedDevice.GetDialApp(this.dial_appname_textbox.Text);
                //Get the application state for your DIAL application
                DialAppStateDetails stateDetails = await app.GetAppStateAsync();
                if (stateDetails.State == DialAppState.NetworkFailure)
                {
                    // In case getting the application state failed because of a network failure, you could add retry logic
                    rootPage.NotifyUser("Network Failure while getting application state", NotifyType.ErrorMessage);
                }
                else
                {
                    //Launch the application on the 1st screen device
                    DialAppLaunchResult result = await app.RequestLaunchAsync(this.dial_launch_args_textbox.Text);
                    //Verify to see whether the application was launched
                    if (result == DialAppLaunchResult.Launched)
                    {
                        rootPage.NotifyUser(string.Format("Launched {0}", app.AppName), NotifyType.StatusMessage);
                        //This is where you will need to add you application specific communication between your 1st and 2nd screen applications
                        //...
                    }
                    else
                    {
                        // In case launch failed, you could add retry logic                       
                        rootPage.NotifyUser(string.Format("Failed to launch application: {0}. Reason: ", app.AppName, result.ToString()), NotifyType.ErrorMessage);
                    }
                }
            }
        }
    }
}
