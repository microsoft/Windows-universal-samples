using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.WiFiDirect;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace SDKTemplate
{
    public sealed partial class ConnectionSettingsPanel : UserControl
    {
        private MainPage rootPage = MainPage.Current;

        List<WiFiDirectConfigurationMethod> _supportedConfigMethods = new List<WiFiDirectConfigurationMethod>();

        public ConnectionSettingsPanel()
        {
            this.InitializeComponent();

            // Initialize the GroupOwnerIntent combo box.
            // The options are "Default", then values 0 through 15.
            cmbGOIntent.Items.Add(new ComboBoxItem() { Content = "Default" });
            for (int i = 0; i <= 15; i++)
            {
                cmbGOIntent.Items.Add(new ComboBoxItem() { Content = i.ToString(), Tag = (short)i });
            }
            cmbGOIntent.SelectedIndex = 0;
        }

        public void Reset()
        {
            _supportedConfigMethods.Clear();
        }

        private void btnAddConfigMethods_Click(object sender, RoutedEventArgs e)
        {
            // The configuration methods are added in the order of preference, most preferred first.
            // Save the values here so we can apply them in OnConnectionRequested.
            // In practice, most apps will have a hard-coded preference list.
            WiFiDirectConfigurationMethod method = Utils.GetSelectedItemTag<WiFiDirectConfigurationMethod>(cmbSupportedConfigMethods);
            _supportedConfigMethods.Add(method);
            rootPage.NotifyUser($"Added configuration method {method}", NotifyType.StatusMessage);
        }

        public async Task<bool> RequestPairDeviceAsync(DeviceInformationPairing pairing)
        {
            WiFiDirectConnectionParameters connectionParams = new WiFiDirectConnectionParameters();

            short? groupOwnerIntent = Utils.GetSelectedItemTag<short?>(cmbGOIntent);
            if (groupOwnerIntent.HasValue)
            {
                connectionParams.GroupOwnerIntent = groupOwnerIntent.Value;
            }

            DevicePairingKinds devicePairingKinds = DevicePairingKinds.None;
            if (_supportedConfigMethods.Count > 0)
            {
                // If specific configuration methods were added, then use them.
                foreach (var configMethod in _supportedConfigMethods)
                {
                    connectionParams.PreferenceOrderedConfigurationMethods.Add(configMethod);
                    devicePairingKinds |= WiFiDirectConnectionParameters.GetDevicePairingKinds(configMethod);
                }
            }
            else
            {
                // If specific configuration methods were not added, then we'll use these pairing kinds.
                devicePairingKinds = DevicePairingKinds.ConfirmOnly | DevicePairingKinds.DisplayPin | DevicePairingKinds.ProvidePin;
            }

            connectionParams.PreferredPairingProcedure = Utils.GetSelectedItemTag<WiFiDirectPairingProcedure>(cmbPreferredPairingProcedure);
            DeviceInformationCustomPairing customPairing = pairing.Custom;
            customPairing.PairingRequested += OnPairingRequested;

            DevicePairingResult result = await customPairing.PairAsync(devicePairingKinds, DevicePairingProtectionLevel.Default, connectionParams);
            if (result.Status != DevicePairingResultStatus.Paired)
            {
                rootPage.NotifyUser($"PairAsync failed, Status: {result.Status}", NotifyType.ErrorMessage);
                return false;
            }
            return true;
        }

        private void OnPairingRequested(DeviceInformationCustomPairing sender, DevicePairingRequestedEventArgs args)
        {
            Utils.HandlePairing(Dispatcher, args);
        }
    }
}
