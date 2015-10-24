//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate;
using System;
using System.Threading.Tasks;
using Windows.Devices.SmartCards;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using static NfcSample.NfcUtils;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace NfcSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ManageCardScenario : Page
    {
        private MainPage rootPage;
        private bool isHceSupported;

        public ManageCardScenario()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            // Clear the messages
            rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage, true);

            if (!ApplicationData.Current.RoamingSettings.Values.ContainsKey("DenyIfPhoneLocked"))
            {
                ApplicationData.Current.RoamingSettings.Values["DenyIfPhoneLocked"] = false;
            }

            if (!ApplicationData.Current.RoamingSettings.Values.ContainsKey("LaunchAboveLock"))
            {
                ApplicationData.Current.RoamingSettings.Values["LaunchAboveLock"] = false;
            }

            chkDenyIfPhoneLocked.IsChecked = (bool)ApplicationData.Current.RoamingSettings.Values["DenyIfPhoneLocked"];
            chkLaunchAboveLock.IsChecked = (bool)ApplicationData.Current.RoamingSettings.Values["LaunchAboveLock"];

            isHceSupported = await CheckHceSupport();
            if (!isHceSupported)
            {
                btnRegisterBgTask.IsEnabled = false;
                btnAddCard.IsEnabled = false;
            }
            else
            {
                lstCards.ItemsSource = await SmartCardEmulator.GetAppletIdGroupRegistrationsAsync();
            }

            Window.Current.VisibilityChanged += Window_VisibilityChanged;
        }

        private async void Window_VisibilityChanged(object sender, VisibilityChangedEventArgs e)
        {
            // Since a registration can change from ForegroundOverride to Disabled when the device
            // is locked, update the registrations when the app window becomes visible
            if (e.Visible && isHceSupported)
            {
                // Clear the messages
                rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage, true);

                lstCards.ItemsSource = await SmartCardEmulator.GetAppletIdGroupRegistrationsAsync();
            }
        }

        private async Task ShowDialog(string msg)
        {
            var msgbox = new Windows.UI.Popups.MessageDialog(msg);
            msgbox.Commands.Add(new Windows.UI.Popups.UICommand("OK"));
            await msgbox.ShowAsync();
        }

        private async void btnRegisterBgTask_Click(object sender, RoutedEventArgs e)
        {
            // Clear the messages
            rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage, true);

            // First check if the device supports NFC/HCE at all
            if (!(await NfcUtils.CheckHceSupport()))
            {
                // HCE not supported
                return;
            }

            // Register the background task which gets launched to handle and respond to incoming APDUs
            await NfcUtils.GetOrRegisterHceBackgroundTask("NFC HCE Sample - Activated", "NfcHceBackgroundTask.BgTask", SmartCardTriggerType.EmulatorHostApplicationActivated);

            // Register the background task which gets launched when our registration state changes (for example the user changes which card is the payment default in the control panel, or another app causes us to be disabled due to an AID conflict)
            await NfcUtils.GetOrRegisterHceBackgroundTask("NFC HCE Sample - Registration Changed", "NfcHceBackgroundTask.BgTask", SmartCardTriggerType.EmulatorAppletIdGroupRegistrationChanged);

            // Register for UICC transaction event
            await NfcUtils.GetOrRegisterHceBackgroundTask("NFC HCE Sample - Transaction", "NfcHceBackgroundTask.BgTask", SmartCardTriggerType.EmulatorTransaction);
        }

        private async void btnRegisterSamplePaymentCard_Click(object sender, RoutedEventArgs e)
        {
            // Clear the messages
            rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage, true);

            // First check if the device supports NFC/HCE at all
            if (!(await NfcUtils.CheckHceSupport()))
            {
                // HCE not supported
                return;
            }

            // Next check if NFC card emualtion is turned on in the settings control panel
            if ((await SmartCardEmulator.GetDefaultAsync()).EnablementPolicy == SmartCardEmulatorEnablementPolicy.Never)
            {
                await ShowDialog("Your NFC tap+pay setting is turned off, you will be taken to the NFC control panel to turn it on");

                // This URI will navigate the user to the NFC tap+pay control panel
                NfcUtils.LaunchNfcPaymentsSettingsPage();
                return;
            }

            this.Frame.Navigate(typeof(SetCardDataScenario));
        }

        private async void btnEnableCard_Click(object sender, RoutedEventArgs e)
        {
            // Clear the messages
            rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage, true);

            var reg = (SmartCardAppletIdGroupRegistration)lstCards.SelectedItem;
            if (reg == null)
            {
                LogMessage("No card selected, must select from listbox", NotifyType.ErrorMessage);
                return;
            }

            await SetCardActivationPolicy(reg, SmartCardAppletIdGroupActivationPolicy.Enabled);

            // Refresh the listbox
            lstCards.ItemsSource = await SmartCardEmulator.GetAppletIdGroupRegistrationsAsync();
        }

        private async void btnDisableCard_Click(object sender, RoutedEventArgs e)
        {
            // Clear the messages
            rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage, true);

            var reg = (SmartCardAppletIdGroupRegistration)lstCards.SelectedItem;
            if (reg == null)
            {
                LogMessage("No card selected, must select from listbox", NotifyType.ErrorMessage);
                return;
            }

            await SetCardActivationPolicy(reg, SmartCardAppletIdGroupActivationPolicy.Disabled);

            // Refresh the listbox
            lstCards.ItemsSource = await SmartCardEmulator.GetAppletIdGroupRegistrationsAsync();
        }

        private async void btnForegroundOverrideCard_Click(object sender, RoutedEventArgs e)
        {
            // Clear the messages
            rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage, true);

            var reg = (SmartCardAppletIdGroupRegistration)lstCards.SelectedItem;
            if (reg == null)
            {
                LogMessage("No card selected, must select from listbox", NotifyType.ErrorMessage);
                return;
            }

            await SetCardActivationPolicy(reg, SmartCardAppletIdGroupActivationPolicy.ForegroundOverride);

            // Refresh the listbox
            lstCards.ItemsSource = await SmartCardEmulator.GetAppletIdGroupRegistrationsAsync();
        }

        private async void btnUnregisterCard_Click(object sender, RoutedEventArgs e)
        {
            // Clear the messages
            rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage, true);

            var reg = (SmartCardAppletIdGroupRegistration)lstCards.SelectedItem;
            if (reg == null)
            {
                LogMessage("No card selected, must select from listbox", NotifyType.ErrorMessage);
                return;
            }

            // Unregister the card
            await SmartCardEmulator.UnregisterAppletIdGroupAsync(reg);

            if (reg.AppletIdGroup.SmartCardEmulationCategory == SmartCardEmulationCategory.Payment
                && reg.AppletIdGroup.SmartCardEmulationType == SmartCardEmulationType.Host)
            {
                // Delete the data file for the card
                await (await Windows.Storage.ApplicationData.Current.LocalFolder.GetFileAsync("ReadRecordResponse-" + reg.Id.ToString("B") + ".dat")).DeleteAsync();
            }

            // Refresh the listbox
            lstCards.ItemsSource = await SmartCardEmulator.GetAppletIdGroupRegistrationsAsync();

            LogMessage("AID group successfully unregistered");
        }

        private void chkDenyIfPhoneLocked_Checked(object sender, RoutedEventArgs e)
        {
            ApplicationData.Current.RoamingSettings.Values["DenyIfPhoneLocked"] = true;
        }

        private void chkDenyIfPhoneLocked_Unchecked(object sender, RoutedEventArgs e)
        {
            ApplicationData.Current.RoamingSettings.Values["DenyIfPhoneLocked"] = false;
        }

        private void chkLaunchAboveLock_Checked(object sender, RoutedEventArgs e)
        {
            ApplicationData.Current.RoamingSettings.Values["LaunchAboveLock"] = true;
        }

        private void chkLaunchAboveLock_Unchecked(object sender, RoutedEventArgs e)
        {
            ApplicationData.Current.RoamingSettings.Values["LaunchAboveLock"] = false;
        }

        private void lstCards_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var reg = (SmartCardAppletIdGroupRegistration)lstCards.SelectedItem;
            if (reg != null)
            {
                // Clear the messages
                rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage, true);
                LogMessage("Selected card is: " + reg.ActivationPolicy.ToString());

                // Setting registration's activation policy from Enabled to ForegroundOverride is
                // not allowed, so disable the button to prevent the user from attempting to do this
                btnForegroundOverrideCard.IsEnabled = (reg.ActivationPolicy != SmartCardAppletIdGroupActivationPolicy.Enabled);
            }
        }

        private async void btnDebugLog_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            var debugLogFile = (StorageFile)(await Windows.Storage.ApplicationData.Current.LocalFolder.TryGetItemAsync("DebugLog.txt"));
            if (debugLogFile != null)
            {
                if (txtDebugLog.Visibility == Windows.UI.Xaml.Visibility.Visible)
                {
                    // Log was already displayed, clear it
                    await debugLogFile.DeleteAsync();
                    txtDebugLog.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                    btnDebugLog.Content = "Debug Log";
                }
                else
                {
                    // Display the log
                    txtDebugLog.Text = await FileIO.ReadTextAsync(debugLogFile);
                    txtDebugLog.Visibility = Windows.UI.Xaml.Visibility.Visible;
                    btnDebugLog.Content = "Clear Debug Log";
                }
            }
        }
    }
}
