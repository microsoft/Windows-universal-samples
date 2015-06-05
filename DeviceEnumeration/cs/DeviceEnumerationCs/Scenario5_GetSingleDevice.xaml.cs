// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.ObjectModel;
using System.IO;
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
    public sealed partial class Scenario5 : Page
    {
        private MainPage rootPage;

        public ObservableCollection<DeviceInformationDisplay> ResultCollection
        {
            get;
            private set;
        }

        public Scenario5()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            ResultCollection = new ObservableCollection<DeviceInformationDisplay>();

            DataContext = this;
        }

        private async void InterfaceIdTextBox_Loaded(object sender, RoutedEventArgs e)
        {
            DeviceInformationCollection deviceInfoCollection;

            // Pre-populate the text box with an interface id.
            deviceInfoCollection = await DeviceInformation.FindAllAsync(DeviceClass.AudioRender);
            if (deviceInfoCollection.Count > 0)
            {
                interfaceIdTextBox.Text = deviceInfoCollection[0].Id;
            }
        }

        private async void GetButton_Click(object sender, RoutedEventArgs e)
        {
            DeviceInformation deviceInfo;
            string interfaceId = interfaceIdTextBox.Text;

            interfaceIdTextBox.IsEnabled = false;
            getButton.IsEnabled = false;
            ResultCollection.Clear();
            
            rootPage.NotifyUser("CreateFromIdAsync operation started...", NotifyType.StatusMessage);

            try
            {
                deviceInfo = await DeviceInformation.CreateFromIdAsync(interfaceId);

                rootPage.NotifyUser("CreateFromIdAsync operation completed.", NotifyType.StatusMessage);

                ResultCollection.Add(new DeviceInformationDisplay(deviceInfo));
            }
            catch (FileNotFoundException)
            {
                rootPage.NotifyUser("Invalid Interface Id", NotifyType.ErrorMessage);
            }

            getButton.IsEnabled = true;
            getButton.Focus(Windows.UI.Xaml.FocusState.Keyboard);
            interfaceIdTextBox.IsEnabled = true;
        }
    }
}
