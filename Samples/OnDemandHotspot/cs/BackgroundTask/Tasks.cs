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

using System;
using Windows.ApplicationModel.Background;
using Windows.Data.Xml.Dom;
using Windows.Devices.WiFi;
using Windows.Foundation.Collections;
using Windows.Security.Credentials;
using Windows.Storage;
using Windows.UI.Notifications;

namespace BackgroundTask
{
    // This task is triggered when the system wants us to refresh our hotspot metadata.
    // This typically happens when the system is about to show a list of available Wi-Fi networks.
    public sealed class UpdateMetadataTask : IBackgroundTask
    {
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            // A real on-demand hotspot task would contact each hotspot device to            
            // obtain this information. This sample doesn't have a real hotspot device,
            // so it just repeats the information set by the UI portion of the app.
            IPropertySet values = ApplicationData.Current.LocalSettings.Values;
            if (!values.TryGetValue("hotspotGuid", out object boxedHotspotGuid))
            {
                // Nothing configured. 
                return;
            }

            Guid hotspotGuid = (Guid)boxedHotspotGuid;
            WiFiOnDemandHotspotNetwork network = WiFiOnDemandHotspotNetwork.GetOrCreateById(hotspotGuid);
            WiFiOnDemandHotspotNetworkProperties properties = network.GetProperties();
            properties.Availability = (bool)values["availability"] ? WiFiOnDemandHotspotAvailability.Available : WiFiOnDemandHotspotAvailability.Unavailable;
            properties.DisplayName = (string)values["displayName"];
            properties.CellularBars = (WiFiOnDemandHotspotCellularBars?)(int?)values["bars"];
            properties.RemainingBatteryPercent = (uint?)values["battery"];
            properties.Ssid = (string)values["ssid"];
            properties.Password = new PasswordCredential { Password = (string)values["password"] };
            network.UpdateProperties(properties);
        }
    }

    // This task is triggered when the user selects our hotspot in the list of available Wi-Fi networks.
    // The task should turn on the Wi-Fi hotspot so that the system can connect to it.

    public sealed class ConnectTask : IBackgroundTask
    {
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            var triggerDetails = (WiFiOnDemandHotspotConnectTriggerDetails)taskInstance.TriggerDetails;

            // A real on-demand hotspot task would contact the hotspot device and tell it
            // to start advertising the SSID.
            //
            // This sample doesn't have a real hotspot device, so it will simulate the action
            // by displaying a toast to instruct the user to turn on the mobile hotspot manually.
            //
            // To demonstrate how to report errors turning on the hotspot, we will report an error
            // if the hotspot is configured without an SSID or without a password.

            WiFiOnDemandHotspotNetwork network = triggerDetails.RequestedNetwork;

            // Is this the network we know how to turn on?
            IPropertySet values = ApplicationData.Current.LocalSettings.Values;
            if (!values.TryGetValue("hotspotGuid", out object boxedHotspotGuid) || (Guid)boxedHotspotGuid != network.Id)
            {
                triggerDetails.ReportError(WiFiOnDemandHotspotConnectStatus.UnspecifiedFailure);
                return;
            }

            // To demonstrate how to report errors turning on the hotspot, we will report an error
            // if the hotspot is configured with an SSID named "error".
            WiFiOnDemandHotspotNetworkProperties properties = network.GetProperties();
            if (properties.Ssid == "error")
            {
                // Simulate an error turning on the hotspot. You can report any error code from the
                // WiFiOnDemandHotspotConnectStatus enumeration.
                triggerDetails.ReportError(WiFiOnDemandHotspotConnectStatus.NetworkNotAvailable);
            }
            else
            {
                DisplayToast(properties.Ssid);
                System.Threading.Thread.Sleep(5000);

                WiFiOnDemandHotspotConnectionResult result = triggerDetails.Connect();
                values["connectResult"] = (int)result.Status;
            }
        }

        private void DisplayToast(string ssid)
        {
            ToastNotifier ToastNotifier = ToastNotificationManager.CreateToastNotifier();

            // Create a two line toast and add audio reminder

            // Here the xml that will be passed to the
            // ToastNotification for the toast is retrieved
            XmlDocument toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText02);

            string message = $"SSID: '{ssid}'. Connecting in five seconds.";

            // Set both lines of text
            XmlNodeList toastNodeList = toastXml.GetElementsByTagName("text");
            toastNodeList.Item(0).AppendChild(toastXml.CreateTextNode("Please turn on your hotspot now!"));
            toastNodeList.Item(1).AppendChild(toastXml.CreateTextNode(message));

            // now create a xml node for the audio source
            IXmlNode toastNode = toastXml.SelectSingleNode("/toast");
            XmlElement audio = toastXml.CreateElement("audio");
            audio.SetAttribute("src", "ms-winsoundevent:Notification.SMS");

            ToastNotification toast = new ToastNotification(toastXml);
            ToastNotifier.Show(toast);
        }
    }
}
