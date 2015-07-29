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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.ApplicationModel.AppService;
using Windows.Foundation.Collections;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppServicesClient
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class KeepConnectionOpenScenario : Page
    {
        private MainPage rootPage;
        private AppServiceConnection connection;

        public KeepConnectionOpenScenario()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void OpenConnection_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            //Is a connection already open?
            if (connection != null)
            {
                rootPage.NotifyUser("A connection already exists", NotifyType.ErrorMessage);
                return;
            }

            //Set up a new app service connection
            connection = new AppServiceConnection();
            connection.AppServiceName = "com.microsoft.randomnumbergenerator";
            connection.PackageFamilyName = "Microsoft.SDKSamples.AppServicesProvider.CS_8wekyb3d8bbwe";
            connection.ServiceClosed += Connection_ServiceClosed;
            AppServiceConnectionStatus status = await connection.OpenAsync();

            //If the new connection opened successfully we're done here
            if (status == AppServiceConnectionStatus.Success)
            {
                rootPage.NotifyUser("Connection is open", NotifyType.StatusMessage);
            }
            else
            {
                //Something went wrong. Lets figure out what it was and show the 
                //user a meaningful message
                switch (status)
                {
                    case AppServiceConnectionStatus.AppNotInstalled:
                        rootPage.NotifyUser("The app AppServicesProvider is not installed. Deploy AppServicesProvider to this device and try again.", NotifyType.ErrorMessage);
                        break;

                    case AppServiceConnectionStatus.AppUnavailable:
                        rootPage.NotifyUser("The app AppServicesProvider is not available. This could be because it is currently being updated or was installed to a removable device that is no longer available.", NotifyType.ErrorMessage);
                        break;

                    case AppServiceConnectionStatus.AppServiceUnavailable:
                        rootPage.NotifyUser(string.Format("The app AppServicesProvider is installed but it does not provide the app service {0}.", connection.AppServiceName), NotifyType.ErrorMessage);
                        break;

                    case AppServiceConnectionStatus.Unknown:
                        rootPage.NotifyUser("An unkown error occurred while we were trying to open an AppServiceConnection.", NotifyType.ErrorMessage);
                        break;
                }

                //Clean up before we go
                connection.Dispose();
                connection = null;
            }
        }

        private async void Connection_ServiceClosed(AppServiceConnection sender, AppServiceClosedEventArgs args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                //Dispose the connection reference we're holding
                if (connection != null)
                {
                    connection.Dispose();
                    connection = null;
                }
            });
        }

        private void CloseConnection_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            //Is there an open connection?
            if (connection == null)
            {
                rootPage.NotifyUser("There's no open connection to close", NotifyType.ErrorMessage);
                return;
            }

            //Close the open connection
            connection.Dispose();
            connection = null;

            //Let the user know we closed the connection
            rootPage.NotifyUser("Connection is closed", NotifyType.StatusMessage);
        }

        private async void GenerateRandomNumber_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            //Is there an open connection?
            if (connection == null)
            {
                rootPage.NotifyUser("You need to open a connection before trying to generate a random number.", NotifyType.ErrorMessage);
                return;
            }

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

            //Send a message to the app service
            var inputs = new ValueSet();
            inputs.Add("minvalue", minValueInput);
            inputs.Add("maxvalue", maxValueInput);
            AppServiceResponse response = await connection.SendMessageAsync(inputs);

            //If the service responded display the message. We're done!
            if (response.Status == AppServiceResponseStatus.Success)
            {
                if (!response.Message.ContainsKey("result"))
                {
                    rootPage.NotifyUser("The app service response message does not contain a key called \"result\"", NotifyType.StatusMessage);
                    return;
                }

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
            }
            else
            {
                //Something went wrong. Show the user a meaningful
                //message depending upon the status
                switch (response.Status)
                {
                    case AppServiceResponseStatus.Failure:
                        rootPage.NotifyUser("The service failed to acknowledge the message we sent it. It may have been terminated because the client was suspended.", NotifyType.ErrorMessage);
                        break;

                    case AppServiceResponseStatus.ResourceLimitsExceeded:
                        rootPage.NotifyUser("The service exceeded the resources allocated to it and had to be terminated.", NotifyType.ErrorMessage);
                        break;

                    case AppServiceResponseStatus.Unknown:
                    default:
                        rootPage.NotifyUser("An unkown error occurred while we were trying to send a message to the service.", NotifyType.ErrorMessage);
                        break;
                }
            }
        }
    }
}
