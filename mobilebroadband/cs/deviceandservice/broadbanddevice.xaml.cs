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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using System.Collections.Generic;
using Windows.Networking.NetworkOperators;
using Windows.Devices.Sms;

namespace MobileBroadband
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class BroadbandDevice : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private IReadOnlyList<string> deviceAccountId = null;
        private int deviceSelected = 0;

        public BroadbandDevice()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            PrepScenario1Sample();
        }

        /// <summary>
        /// This is the click handler for the 'Get Device Information' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void UpdateData_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                GetCurrentDeviceInfo(deviceAccountId[deviceSelected]);
                 // increment device count until reach number max number of devices and then start over
                deviceSelected = (deviceSelected + 1) % deviceAccountId.Count;

                // update Button with next device
                UpdateData.Content = "Get Information for Device #" + (deviceSelected + 1);
            }
        }

        void PrepScenario1Sample()
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            try
            {
                deviceSelected = 0;
                deviceAccountId = MobileBroadbandAccount.AvailableNetworkAccountIds;

                if (deviceAccountId.Count != 0)
                {
                    rootPage.NotifyUser("Mobile Broadband Device(s) have been installed that grant access to this application", NotifyType.StatusMessage);
					
                    NumDevices.Text = "There are " + deviceAccountId.Count + " account(s) installed.";
                    UpdateData.Content = "Get Information for Device #1";
                    UpdateData.IsEnabled = true;
                }
                else
                {
                    UpdateData.Content = "No available accounts detected";
                    UpdateData.IsEnabled = false;
                }
            }
            catch (Exception ex)
            {
               rootPage.NotifyUser("Error:" + ex.Message, NotifyType.ErrorMessage);
            }
        }

        void GetCurrentDeviceInfo(string accountId)
        {
            try
            {
				// Test touching new TH objectq
				var modem = MobileBroadbandModem.GetDefault();
				var deviceId = modem.DeviceInformation.DeviceId;

                var mobileBroadbandAccount = MobileBroadbandAccount.CreateFromNetworkAccountId(accountId);

                ProviderName.Text = mobileBroadbandAccount.ServiceProviderName;
                ProviderGuid.Text = mobileBroadbandAccount.ServiceProviderGuid.ToString();
                NetworkAccountId.Text = mobileBroadbandAccount.NetworkAccountId;

                var currentNetwork = mobileBroadbandAccount.CurrentNetwork;

                if (currentNetwork != null)
                {
                    string accessPointName = currentNetwork.AccessPointName;
                    if (String.IsNullOrEmpty(accessPointName))
                    {
                        accessPointName = "(not connected)";
                    }

                    NetRegister.Text = currentNetwork.NetworkRegistrationState.ToString();
                    NetRegError.Text = NetErrorToString(currentNetwork.RegistrationNetworkError);
                    PacketAttachError.Text = NetErrorToString(currentNetwork.PacketAttachNetworkError);
                    ActivateError.Text = NetErrorToString(currentNetwork.ActivationNetworkError);
                    AccessPointName.Text = accessPointName;
                    NetworkAdapterId.Text = currentNetwork.NetworkAdapter.NetworkAdapterId.ToString();
                    NetworkType.Text = currentNetwork.NetworkAdapter.NetworkItem.GetNetworkTypes().ToString();
                    RegisteredProviderId.Text = currentNetwork.RegisteredProviderId;
                    RegisteredProviderName.Text = currentNetwork.RegisteredProviderName;
                    RegisteredDataClass.Text = currentNetwork.RegisteredDataClass.ToString();

                }
                else
                {
                    NetRegister.Text = "";
                    NetRegError.Text = "";
                    PacketAttachError.Text = "";
                    ActivateError.Text = "";
                    AccessPointName.Text = "";
                    NetworkAdapterId.Text = "";
                    NetworkType.Text = "";
                    RegisteredProviderId.Text = "";
                    RegisteredProviderName.Text = "";
                    RegisteredDataClass.Text = "";
                }

                var deviceInformation = mobileBroadbandAccount.CurrentDeviceInformation;

                if (deviceInformation != null)
                {
                    string mobileNumber = "";
                    if (deviceInformation.TelephoneNumbers.Count > 0)
                    {
                        mobileNumber = deviceInformation.TelephoneNumbers[0];
                    }

                    DeviceManufacturer.Text = deviceInformation.Manufacturer;
                    DeviceModel.Text = deviceInformation.Model;
                    Firmware.Text = deviceInformation.FirmwareInformation;
                    CellularClasses.Text = deviceInformation.CellularClass.ToString();
                    DataClasses.Text = deviceInformation.DataClasses.ToString();
                    if (deviceInformation.DataClasses.HasFlag(Windows.Networking.NetworkOperators.DataClasses.Custom))
                    {
                        DataClasses.Text += " (custom is " + deviceInformation.CustomDataClass + ")";
                    }
                    MobileNumber.Text = mobileNumber;
                    SimId.Text = deviceInformation.SimIccId;
                    DeviceType.Text = deviceInformation.DeviceType.ToString();
                    DeviceId.Text = deviceInformation.DeviceId.ToString();
                    NetworkDeviceStatus.Text = deviceInformation.NetworkDeviceStatus.ToString();

                    if (deviceInformation.CellularClass == CellularClass.Gsm)
                    {
                        MobEquipIdLabel.Text = "IMEI:";
                        MobEquipIdValue.Text = deviceInformation.MobileEquipmentId;

                        SubIdLabel.Text = "IMSI:";
                        SubIdValue.Text = deviceInformation.SubscriberId;
                    }
                    else if (deviceInformation.CellularClass == CellularClass.Cdma)
                    {
                        MobEquipIdLabel.Text = "ESN/MEID:";
                        MobEquipIdValue.Text = deviceInformation.MobileEquipmentId;

                        SubIdLabel.Text = "MIN/IRM:";
                        SubIdValue.Text = deviceInformation.SubscriberId;
                    }
                    else
                    {
                        MobEquipIdLabel.Text = "";
                        MobEquipIdValue.Text = "";
                        SubIdLabel.Text = "";
                        SubIdValue.Text = "";
                    }
                }
                else
                {
                    DeviceManufacturer.Text = "";
                    DeviceModel.Text = "";
                    Firmware.Text = "";
                    CellularClasses.Text = "";
                    DataClasses.Text = "";
                    MobileNumber.Text = "";
                    SimId.Text = "";
                    DeviceType.Text = "";
                    DeviceId.Text = "";
                    NetworkDeviceStatus.Text = "";
                    MobEquipIdLabel.Text = "";
                    MobEquipIdValue.Text = "";
                    SubIdLabel.Text = "";
                    SubIdValue.Text = "";
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error:" + ex.Message, NotifyType.ErrorMessage);

                ProviderName.Text = "";
                ProviderGuid.Text = "";
                NetworkAccountId.Text = "";
                NetRegister.Text = "";
                NetRegError.Text = "";
                PacketAttachError.Text = "";
                ActivateError.Text = "";
                AccessPointName.Text = "";
                NetworkAdapterId.Text = "";
                NetworkType.Text = "";
                DeviceManufacturer.Text = "";
                DeviceModel.Text = "";
                Firmware.Text = "";
                CellularClasses.Text = "";
                DataClasses.Text = "";
                MobileNumber.Text = "";
                SimId.Text = "";
                DeviceType.Text = "";
                DeviceId.Text = "";
                NetworkDeviceStatus.Text = "";
                MobEquipIdLabel.Text = "";
                MobEquipIdValue.Text = "";
                SubIdLabel.Text = "";
                SubIdValue.Text = "";
                RegisteredProviderId.Text = "";
                RegisteredProviderName.Text = "";
                RegisteredDataClass.Text = "";
            }
        }

        string NetErrorToString(uint netError)
        {
            return netError == 0 ? "none" : netError.ToString();
        }
    }
}
