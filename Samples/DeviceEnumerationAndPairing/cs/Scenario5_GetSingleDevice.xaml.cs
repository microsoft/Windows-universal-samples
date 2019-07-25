// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.ObjectModel;
using System.IO;
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
    public sealed partial class Scenario5_GetSingleDevice : Page
    {
        private MainPage rootPage = MainPage.Current;
        private DeviceInformationKind deviceInformationKind;

        private ObservableCollection<DeviceInformationDisplay> resultCollection = new ObservableCollection<DeviceInformationDisplay>();

        public Scenario5_GetSingleDevice()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            resultsListView.ItemsSource = resultCollection;
        }

        private async void Page_Loaded(object sender, RoutedEventArgs e)
        {
            DeviceInformationCollection deviceInfoCollection;

            // Pre-populate the text box with an interface id.
            deviceInfoCollection = await DeviceInformation.FindAllAsync(DeviceClass.AudioRender);
            if (deviceInfoCollection.Count > 0)
            {
                // When you want to "save" a DeviceInformation to get it back again later,
                // use both the DeviceInformation.Kind and the DeviceInformation.Id.
                interfaceIdTextBox.Text = deviceInfoCollection[0].Id;
                deviceInformationKind = deviceInfoCollection[0].Kind;
                InformationKindTextBox.Text = deviceInformationKind.ToString();
                getButton.IsEnabled = true;
            }
        }

        private async void GetButton_Click(object sender, RoutedEventArgs e)
        {
            resultCollection.Clear();

            string interfaceId = interfaceIdTextBox.Text;
            getButton.IsEnabled = false;

            rootPage.NotifyUser("CreateFromIdAsync operation started...", NotifyType.StatusMessage);

            try
            {
                // When you want to "save" a DeviceInformation to get it back again later,
                // use both the DeviceInformation.Kind and the DeviceInformation.Id.
                DeviceInformation deviceInfo = await DeviceInformation.CreateFromIdAsync(interfaceId, null, deviceInformationKind);

                rootPage.NotifyUser("CreateFromIdAsync operation completed.", NotifyType.StatusMessage);

                resultCollection.Add(new DeviceInformationDisplay(deviceInfo));
            }
            catch (FileNotFoundException)
            {
                rootPage.NotifyUser("Invalid Interface Id", NotifyType.ErrorMessage);
            }

            getButton.IsEnabled = true;
            getButton.Focus(FocusState.Keyboard);
        }
    }
}
