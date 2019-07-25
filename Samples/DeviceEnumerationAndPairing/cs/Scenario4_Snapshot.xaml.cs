// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.ObjectModel;
using Windows.Devices.Enumeration;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4_Snapshot : Page
    {
        private MainPage rootPage = MainPage.Current;

        private ObservableCollection<DeviceInformationDisplay> resultCollection = new ObservableCollection<DeviceInformationDisplay>();

        public Scenario4_Snapshot()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            resultsListView.ItemsSource = resultCollection;

            selectorComboBox.ItemsSource = DeviceSelectorChoices.FindAllAsyncSelectors;
            selectorComboBox.SelectedIndex = 0;
        }

        private async void FindButton_Click(object sender, RoutedEventArgs e)
        {
            DeviceSelectorInfo deviceSelectorInfo;
            DeviceInformationCollection deviceInfoCollection;

            findButton.IsEnabled = false;
            resultCollection.Clear();

            // First get the device selector chosen by the UI.
            deviceSelectorInfo = (DeviceSelectorInfo)selectorComboBox.SelectedItem;

            rootPage.NotifyUser("FindAllAsync operation started...", NotifyType.StatusMessage);

            if (null == deviceSelectorInfo.Selector)
            {
                // If the a pre-canned device class selector was chosen, call the DeviceClass overload
                deviceInfoCollection = await DeviceInformation.FindAllAsync(deviceSelectorInfo.DeviceClassSelector);
            }
            else
            {
                // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
                deviceInfoCollection = await DeviceInformation.FindAllAsync(
                    deviceSelectorInfo.Selector,
                    null // don't request additional properties for this sample
                    );
            }

            rootPage.NotifyUser(
                String.Format("FindAllAsync operation completed. {0} devices found.", deviceInfoCollection.Count),
                NotifyType.StatusMessage);

            foreach (DeviceInformation deviceInfo in deviceInfoCollection)
            {
                resultCollection.Add(new DeviceInformationDisplay(deviceInfo));
            }

            findButton.IsEnabled = true;
        }
    }
}
