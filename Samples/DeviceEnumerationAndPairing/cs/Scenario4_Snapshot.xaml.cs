// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.ObjectModel;
using Windows.Devices.Enumeration;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace DeviceEnumeration
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4 : Page
    {
        private MainPage rootPage;

        public ObservableCollection<DeviceInformationDisplay> ResultCollection
        {
            get;
            private set;
        }

        public Scenario4()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            ResultCollection = new ObservableCollection<DeviceInformationDisplay>();

            selectorComboBox.ItemsSource = DeviceSelectorChoices.FindAllAsyncSelectors;
            selectorComboBox.SelectedIndex = 0;

            DataContext = this;
        }

        private async void FindButton_Click(object sender, RoutedEventArgs e)
        {
            DeviceSelectorInfo deviceSelectorInfo;
            DeviceInformationCollection deviceInfoCollection;

            findButton.IsEnabled = false;
            ResultCollection.Clear();

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
                ResultCollection.Add(new DeviceInformationDisplay(deviceInfo));
            }

            findButton.IsEnabled = true;
        }
    }
}
