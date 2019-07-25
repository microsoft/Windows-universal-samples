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
    public sealed partial class Scenario6_CustomFilterAddedProps : Page
    {
        private MainPage rootPage = MainPage.Current;

        private DeviceWatcherHelper deviceWatcherHelper;

        private ObservableCollection<DeviceInformationDisplay> resultCollection = new ObservableCollection<DeviceInformationDisplay>();

        public Scenario6_CustomFilterAddedProps()
        {
            this.InitializeComponent();

            deviceWatcherHelper = new DeviceWatcherHelper(resultCollection, Dispatcher);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            resultsListView.ItemsSource = resultCollection;
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
            aqsFilterTextBox.IsEnabled = false;
            startWatcherButton.IsEnabled = false;
            resultCollection.Clear();

            // Request some additional properties.  In this saample, these extra properties are just used in the ResultsListViewTemplate.
            // Take a look there in the XAML. Also look at the converter used by the XAML GeneralPropertyValueConverter.  In general you just use
            // DeviceInformation.Properties["<property name>"] to get a property. e.g. DeviceInformation.Properties["System.Devices.InterfaceClassGuid"].
            string[] requestedProperties = new string[] {
                "System.Devices.InterfaceClassGuid",
                "System.ItemNameDisplay"
            };

            // Use AQS string filter from the text box
            DeviceWatcher deviceWatcher = DeviceInformation.CreateWatcher(
                aqsFilterTextBox.Text,
                requestedProperties);

            rootPage.NotifyUser("Starting Watcher...", NotifyType.StatusMessage);
            deviceWatcherHelper.StartWatcher(deviceWatcher);
            stopWatcherButton.IsEnabled = true;
            stopWatcherButton.Focus(FocusState.Keyboard);
            aqsFilterTextBox.IsEnabled = true;
        }

        private void StopWatcher()
        {
            aqsFilterTextBox.IsEnabled = false;
            stopWatcherButton.IsEnabled = false;

            deviceWatcherHelper.StopWatcher();

            startWatcherButton.IsEnabled = true;
            startWatcherButton.Focus(FocusState.Keyboard);
            aqsFilterTextBox.IsEnabled = true;
        }
    }
}
