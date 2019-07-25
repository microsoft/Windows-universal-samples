// Copyright (c) Microsoft. All rights reserved.

using System;
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
    public sealed partial class Scenario2_DeviceWatcher : Page
    {
        private MainPage rootPage = MainPage.Current;

        private DeviceWatcherHelper deviceWatcherHelper;

        private ObservableCollection<DeviceInformationDisplay> resultCollection = new ObservableCollection<DeviceInformationDisplay>();

        public Scenario2_DeviceWatcher()
        {
            this.InitializeComponent();

            deviceWatcherHelper = new DeviceWatcherHelper(resultCollection, Dispatcher);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            resultsListView.ItemsSource = resultCollection;
            selectorComboBox.ItemsSource = DeviceSelectorChoices.DeviceWatcherSelectors;
            selectorComboBox.SelectedIndex = 0;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            deviceWatcherHelper.Reset();
        }

        private void StartWatcherButton_Click(object sender, RoutedEventArgs e)
        {
            StartWatcher();
        }

        private void StopWatcherButton_Click(object sender, RoutedEventArgs e)
        {
            StopWatcher();
        }

        private void StartWatcher()
        {
            startWatcherButton.IsEnabled = false;
            resultCollection.Clear();

            // First get the device selector chosen by the UI.
            DeviceSelectorInfo deviceSelectorInfo = (DeviceSelectorInfo)selectorComboBox.SelectedItem;

            DeviceWatcher deviceWatcher;
            if (null == deviceSelectorInfo.Selector)
            {
                // If the a pre-canned device class selector was chosen, call the DeviceClass overload
                deviceWatcher = DeviceInformation.CreateWatcher(deviceSelectorInfo.DeviceClassSelector);
            }
            else if (deviceSelectorInfo.Kind == DeviceInformationKind.Unknown)
            {
                // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
                // Kind will be determined by the selector
                deviceWatcher = DeviceInformation.CreateWatcher(
                    deviceSelectorInfo.Selector,
                    null // don't request additional properties for this sample
                    );
            }
            else
            {
                // Kind is specified in the selector info
                deviceWatcher = DeviceInformation.CreateWatcher(
                    deviceSelectorInfo.Selector,
                    null, // don't request additional properties for this sample
                    deviceSelectorInfo.Kind);
            }

            rootPage.NotifyUser("Starting Watcher...", NotifyType.StatusMessage);
            deviceWatcherHelper.StartWatcher(deviceWatcher);
            stopWatcherButton.IsEnabled = true;
        }

        private void StopWatcher()
        {
            stopWatcherButton.IsEnabled = false;
            deviceWatcherHelper.StopWatcher();
            startWatcherButton.IsEnabled = true;
        }
    }
}
