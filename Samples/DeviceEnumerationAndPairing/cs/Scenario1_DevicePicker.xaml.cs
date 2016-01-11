// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.ObjectModel;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace DeviceEnumeration
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;

        private DevicePicker devicePicker = null;

        public ObservableCollection<DeviceInformationDisplay> ResultCollection
        {
            get;
            private set;
        }

        public Scenario1()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            ResultCollection = new ObservableCollection<DeviceInformationDisplay>();

            selectorComboBox.ItemsSource = DeviceSelectorChoices.DevicePickerSelectors;
            selectorComboBox.SelectedIndex = 0;

            DataContext = this;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
        }

        private void PickSingleDeviceButton_Click(object sender, RoutedEventArgs e)
        {
            ShowDevicePicker(
                true // pickSingle
                );
        }

        private void ShowDevicePickerButton_Click(object sender, RoutedEventArgs e)
        {
            ShowDevicePicker(
                false // pickSingle
                );
        }

        private async void ShowDevicePicker(bool pickSingle)
        {
            showDevicePickerButton.IsEnabled = false;
            ResultCollection.Clear();

            devicePicker = new DevicePicker();

            // First get the device selector chosen by the UI.
            DeviceSelectorInfo deviceSelectorInfo = (DeviceSelectorInfo)selectorComboBox.SelectedItem;

            if (null == deviceSelectorInfo.Selector)
            {
                // If the a pre-canned device class selector was chosen, call the DeviceClass overload
                devicePicker.Filter.SupportedDeviceClasses.Add(deviceSelectorInfo.DeviceClassSelector);
            }
            else
            {
                // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
                devicePicker.Filter.SupportedDeviceSelectors.Add(deviceSelectorInfo.Selector);
            }

            rootPage.NotifyUser("Showing Device Picker", NotifyType.StatusMessage);

            // Calculate the position to show the picker (right below the buttons)
            GeneralTransform ge = pickSingleDeviceButton.TransformToVisual(null);
            Point point = ge.TransformPoint(new Point());
            Rect rect = new Rect(point, new Point(point.X + pickSingleDeviceButton.ActualWidth, point.Y + pickSingleDeviceButton.ActualHeight));

            if (pickSingle)
            {
                DeviceInformation di = await devicePicker.PickSingleDeviceAsync(rect);
                if (null != di)
                {
                    ResultCollection.Add(new DeviceInformationDisplay(di));
                }
                showDevicePickerButton.IsEnabled = true;
            }
            else
            {
                devicePicker.DevicePickerDismissed += new TypedEventHandler<DevicePicker, object>(
                async (picker, obj) =>
                {
                    // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
                    await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                    {
                        showDevicePickerButton.IsEnabled = true;
                        rootPage.NotifyUser("Hiding Device Picker", NotifyType.StatusMessage);
                    });
                });

                devicePicker.DeviceSelected += new TypedEventHandler<DevicePicker, DeviceSelectedEventArgs>(
                async (picker, args) =>
                {
                    // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
                    await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                    {
                        ResultCollection.Clear();
                        ResultCollection.Add(new DeviceInformationDisplay(args.SelectedDevice));
                    });
                });

                // Show the picker
                devicePicker.Show(rect);
            }
        }
    }
}
