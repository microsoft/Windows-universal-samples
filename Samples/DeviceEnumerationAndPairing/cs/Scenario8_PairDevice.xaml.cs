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
    public sealed partial class Scenario8_PairDevice : Page
    {
        private MainPage rootPage = MainPage.Current;

        private DeviceWatcherHelper deviceWatcherHelper;

        private ObservableCollection<DeviceInformationDisplay> resultCollection = new ObservableCollection<DeviceInformationDisplay>();

        public Scenario8_PairDevice()
        {
            this.InitializeComponent();

            deviceWatcherHelper = new DeviceWatcherHelper(resultCollection, Dispatcher);
            deviceWatcherHelper.DeviceChanged += OnDeviceListChanged;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            resultsListView.ItemsSource = resultCollection;

            selectorComboBox.ItemsSource = DeviceSelectorChoices.PairingSelectors;
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

            // Get the device selector chosen by the UI then add additional constraints for devices that
            // can be paired or are already paired.
            DeviceSelectorInfo deviceSelectorInfo = (DeviceSelectorInfo)selectorComboBox.SelectedItem;
            string selector = "(" + deviceSelectorInfo.Selector + ")" + " AND (System.Devices.Aep.CanPair:=System.StructuredQueryType.Boolean#True OR System.Devices.Aep.IsPaired:=System.StructuredQueryType.Boolean#True)";

            DeviceWatcher deviceWatcher;
            if (deviceSelectorInfo.Kind == DeviceInformationKind.Unknown)
            {
                // Kind will be determined by the selector
                deviceWatcher = DeviceInformation.CreateWatcher(
                    selector,
                    null // don't request additional properties for this sample
                    );
            }
            else
            {
                // Kind is specified in the selector info
                deviceWatcher = DeviceInformation.CreateWatcher(
                    selector,
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

        private void OnDeviceListChanged(DeviceWatcher sender, string id)
        {
            // If the item being updated is currently "selected", then update the pairing buttons
            DeviceInformationDisplay selectedDeviceInfoDisp = (DeviceInformationDisplay)resultsListView.SelectedItem;
            if ((selectedDeviceInfoDisp != null) && (selectedDeviceInfoDisp.Id == id))
            {
                UpdatePairingButtons();
            }
        }

        private async void PairButton_Click(object sender, RoutedEventArgs e)
        {
            // Gray out the pair button and results view while pairing is in progress.
            resultsListView.IsEnabled = false;
            pairButton.IsEnabled = false;
            rootPage.NotifyUser("Pairing started. Please wait...", NotifyType.StatusMessage);

            DeviceInformationDisplay deviceInfoDisp = resultsListView.SelectedItem as DeviceInformationDisplay;

            DevicePairingResult dpr = await deviceInfoDisp.DeviceInformation.Pairing.PairAsync();

            rootPage.NotifyUser(
                "Pairing result = " + dpr.Status.ToString(),
                dpr.Status == DevicePairingResultStatus.Paired ? NotifyType.StatusMessage : NotifyType.ErrorMessage);

            UpdatePairingButtons();
            resultsListView.IsEnabled = true;
        }

        private async void UnpairButton_Click(object sender, RoutedEventArgs e)
        {
            // Gray out the unpair button and results view while unpairing is in progress.
            resultsListView.IsEnabled = false;
            unpairButton.IsEnabled = false;
            rootPage.NotifyUser("Unpairing started. Please wait...", NotifyType.StatusMessage);

            DeviceInformationDisplay deviceInfoDisp = resultsListView.SelectedItem as DeviceInformationDisplay;

            DeviceUnpairingResult dupr = await deviceInfoDisp.DeviceInformation.Pairing.UnpairAsync();

            rootPage.NotifyUser(
                "Unpairing result = " + dupr.Status.ToString(),
                dupr.Status == DeviceUnpairingResultStatus.Unpaired ? NotifyType.StatusMessage : NotifyType.ErrorMessage);

            UpdatePairingButtons();
            resultsListView.IsEnabled = true;
        }

        private void ResultsListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UpdatePairingButtons();
        }

        private void UpdatePairingButtons()
        {
            DeviceInformationDisplay deviceInfoDisp = (DeviceInformationDisplay)resultsListView.SelectedItem;

            if (null != deviceInfoDisp &&
                deviceInfoDisp.DeviceInformation.Pairing.CanPair &&
                !deviceInfoDisp.DeviceInformation.Pairing.IsPaired)
            {
                pairButton.IsEnabled = true;
            }
            else
            {
                pairButton.IsEnabled = false;
            }

            if (null != deviceInfoDisp &&
                deviceInfoDisp.DeviceInformation.Pairing.IsPaired)
            {
                unpairButton.IsEnabled = true;
            }
            else
            {
                unpairButton.IsEnabled = false;
            }
        }
    }
}
