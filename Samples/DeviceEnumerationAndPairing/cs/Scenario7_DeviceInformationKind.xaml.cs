// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario7_DeviceInformationKind : Page
    {
        private MainPage rootPage = MainPage.Current;

        private List<DeviceWatcherHelper> deviceWatcherHelpers = new List<DeviceWatcherHelper>();

        private ObservableCollection<DeviceInformationDisplay> resultCollection = new ObservableCollection<DeviceInformationDisplay>();

        public Scenario7_DeviceInformationKind()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            resultsListView.ItemsSource = resultCollection;

            kindComboBox.ItemsSource = DeviceInformationKindChoices.Choices;
            kindComboBox.SelectedIndex = 0;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            StopWatchers(/* reset */ true);
        }

        private void StartWatcherButton_Click(object sender, RoutedEventArgs e)
        {
            StartWatchers();
        }

        private void StopWatcherButton_Click(object sender, RoutedEventArgs e)
        {
            StopWatchers();
        }

        private void StartWatchers()
        {
            startWatcherButton.IsEnabled = false;
            resultCollection.Clear();

            // Create a watcher for each DeviceInformationKind selected by the user
            foreach (DeviceInformationKind deviceInfoKind in ((DeviceInformationKindChoice)kindComboBox.SelectedItem).DeviceInformationKinds)
            {
                DeviceWatcher deviceWatcher = DeviceInformation.CreateWatcher(
                        "", // AQS Filter string
                        null, // requested properties
                        deviceInfoKind);

                DeviceWatcherHelper deviceWatcherHelper = new DeviceWatcherHelper(resultCollection, Dispatcher);
                deviceWatcherHelper.UpdateStatus = false; // we will show our own status messages
                deviceWatcherHelper.DeviceChanged += OnDeviceListChanged;
                deviceWatcherHelpers.Add(deviceWatcherHelper);

                deviceWatcherHelper.StartWatcher(deviceWatcher);
            }

            stopWatcherButton.IsEnabled = true;
            stopWatcherButton.Focus(FocusState.Keyboard);
        }

        private void StopWatchers(bool reset = false)
        {
            stopWatcherButton.IsEnabled = false;

            foreach (DeviceWatcherHelper deviceWatcherHelper in deviceWatcherHelpers)
            {
                deviceWatcherHelper.StopWatcher();
                if (reset)
                {
                    deviceWatcherHelper.Reset();
                }
            }
            deviceWatcherHelpers.Clear();

            startWatcherButton.IsEnabled = true;
            startWatcherButton.Focus(FocusState.Keyboard);
        }

        private void OnDeviceListChanged(DeviceWatcher sender, string id)
        {
            int watchersRunning = 0;

            // Count running watchers
            foreach (DeviceWatcherHelper deviceWatcherHelper in deviceWatcherHelpers)
            {
                if (deviceWatcherHelper.IsWatcherRunning())
                {
                    watchersRunning++;
                }
            }

            string message = $"{watchersRunning}/{deviceWatcherHelpers.Count} watchers running. {resultCollection.Count} devices found.";
            rootPage.NotifyUser(message, NotifyType.StatusMessage);
        }
    }
}
