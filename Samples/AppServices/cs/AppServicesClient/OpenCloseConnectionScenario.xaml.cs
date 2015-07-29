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
using Windows.ApplicationModel.AppService;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppServicesClient
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class OpenCloseConnectionScenario : Page
    {
        private MainPage rootPage;

        public OpenCloseConnectionScenario()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void GenerateRandomNumber_Click(object sender, RoutedEventArgs e)
        {
            //Parse user input
            int minValueInput = 0;
            bool valueParsed = int.TryParse(MinValue.Text, out minValueInput);
            if (!valueParsed)
            {
                rootPage.NotifyUser("The Minimum Value should be a valid integer", NotifyType.ErrorMessage);
                return;
            }

            int maxValueInput = 0;
            valueParsed = int.TryParse(MaxValue.Text, out maxValueInput);
            if (!valueParsed)
            {
                rootPage.NotifyUser("The Maximum Value should be a valid integer", NotifyType.ErrorMessage);
                return;
            }

            if (maxValueInput <= minValueInput)
            {
                rootPage.NotifyUser("Maximum Value must be larger than Minimum Value", NotifyType.ErrorMessage);
                return;
            }

            using (var connection = new AppServiceConnection())
            {
                //Set up a new app service connection
                connection.AppServiceName = "com.microsoft.randomnumbergenerator";
                connection.PackageFamilyName = "Microsoft.SDKSamples.AppServicesProvider.CS_8wekyb3d8bbwe";
                AppServiceConnectionStatus status = await connection.OpenAsync();

                //The new connection opened successfully
                if (status == AppServiceConnectionStatus.Success)
                {
                    rootPage.NotifyUser("Connection established", NotifyType.StatusMessage);
                }

                //If something went wrong. Lets figure out what it was and show the 
                //user a meaningful message and walk away
                switch (status)
                {
                    case AppServiceConnectionStatus.AppNotInstalled:
                        rootPage.NotifyUser("The app AppServicesProvider is not installed. Deploy AppServicesProvider to this device and try again.", NotifyType.ErrorMessage);
                        return;

                    case AppServiceConnectionStatus.AppUnavailable:
                        rootPage.NotifyUser("The app AppServicesProvider is not available. This could be because it is currently being updated or was installed to a removable device that is no longer available.", NotifyType.ErrorMessage);
                        return;

                    case AppServiceConnectionStatus.AppServiceUnavailable:
                        rootPage.NotifyUser(string.Format("The app AppServicesProvider is installed but it does not provide the app service {0}.", connection.AppServiceName), NotifyType.ErrorMessage);
                        return;

                    case AppServiceConnectionStatus.Unknown:
                        rootPage.NotifyUser("An unkown error occurred while we were trying to open an AppServiceConnection.", NotifyType.ErrorMessage);
                        return;
                }

                //Set up the inputs and send a message to the service
                var inputs = new ValueSet();
                inputs.Add("minvalue", minValueInput);
                inputs.Add("maxvalue", maxValueInput);
                AppServiceResponse response = await connection.SendMessageAsync(inputs);

                //If the service responded with success display the result and walk away
                if (response.Status == AppServiceResponseStatus.Success &&
                    response.Message.ContainsKey("result"))
                {
                    var resultText = response.Message["result"].ToString();
                    if (!string.IsNullOrEmpty(resultText))
                    {
                        Result.Text = resultText;
                        rootPage.NotifyUser("App service responded with a result", NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("App service did not respond with a result", NotifyType.ErrorMessage);
                    }

                    return;
                }

                //Something went wrong while sending a message. Let display
                //a meaningful error message
                switch (response.Status)
                {
                    case AppServiceResponseStatus.Failure:
                        rootPage.NotifyUser("The service failed to acknowledge the message we sent it. It may have been terminated or it's RequestReceived handler might not be handling incoming messages correctly.", NotifyType.ErrorMessage);
                        return;

                    case AppServiceResponseStatus.ResourceLimitsExceeded:
                        rootPage.NotifyUser("The service exceeded the resources allocated to it and had to be terminated.", NotifyType.ErrorMessage);
                        return;

                    case AppServiceResponseStatus.Unknown:
                        rootPage.NotifyUser("An unkown error occurred while we were trying to send a message to the service.", NotifyType.ErrorMessage);
                        return;
                }
            }
        }
    }
}
