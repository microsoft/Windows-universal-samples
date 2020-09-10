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
using System.Collections.Generic;
using Windows.Foundation;
using Windows.Security.Authorization.AppCapabilityAccess;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario2_RequestMany : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario2_RequestMany()
        {
            this.InitializeComponent();
        }

        async void RequestAccessButton_Click(object sender, RoutedEventArgs e)
        {
            // This will prompt for any capabilities whose current status is UserPromptRequired.
            IReadOnlyDictionary<string, AppCapabilityAccessStatus> result =
                await AppCapability.RequestAccessForCapabilitiesAsync(new[] { "location", "webcam" });
            string locationMessage;
            if (result["location"] == AppCapabilityAccessStatus.Allowed)
            {
                locationMessage = "Location access is granted. ";
            }
            else
            {
                // We could inspect the specific status to provide better feedback,
                // but for the purpose of this sample, we merely report whether access was granted
                // and do not study the reason why.
                locationMessage = "Location access is not granted. ";
            }
            string webcamMessage;
            if (result["webcam"] == AppCapabilityAccessStatus.Allowed)
            {
                webcamMessage = "Webcam access is granted. ";
            }
            else
            {
                // We could inspect the specific status to provide better feedback,
                // but for the purpose of this sample, we merely report whether access was granted
                // and do not study the reason why.
                webcamMessage = "Webcam access is not granted. ";
            }
            rootPage.NotifyUser(locationMessage + webcamMessage, NotifyType.StatusMessage);
        }
    }
}
