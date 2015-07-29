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
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.Networking;
using Windows.Networking.Connectivity;
using Windows.Networking.Sockets;
using Windows.Security.Cryptography.Certificates;
using Windows.Storage.Streams;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace StreamSocketSample
{
    /// <summary>
    /// A page for fifth scenario.
    /// </summary>
    public sealed partial class Scenario5 : Page
    {
        private const int continueButtonId = 1;
        private const int abortButtonId = 0;

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser().
        MainPage rootPage = MainPage.Current;

        public Scenario5()
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
        }

        /// <summary>
        /// This is the click handler for the 'ConnectSocket' button.
        /// </summary>
        /// <param name="sender">Object for which the event was generated.</param>
        /// <param name="e">Event's parameters.</param>
        private async void ConnectSocket_Click(object sender, RoutedEventArgs e)
        {
            // Validating the host name is required since it was received 
            // from an untrusted source (user input). The host name is validated by catching ArgumentExceptions thrown
            // by the HostName constructor for invalid input.
            if (String.IsNullOrEmpty(ServiceNameForConnect.Text))
            {
                rootPage.NotifyUser("Please provide a service name.", NotifyType.ErrorMessage);
                return;
            }

            HostName hostName;
            try
            {
                hostName = new HostName(HostNameForConnect.Text);
            }
            catch (ArgumentException)
            {
                rootPage.NotifyUser("Error: Invalid host name.", NotifyType.ErrorMessage);
                return;
            }

            rootPage.NotifyUser("Connecting to: " + HostNameForConnect.Text, NotifyType.StatusMessage);

            using (StreamSocket socket = new StreamSocket())
            {
                // If necessary, tweak the socket's control options before carrying out the connect operation.
                // Refer to the StreamSocketControl class' MSDN documentation for the full list of control options.
                socket.Control.ClientCertificate = null;

                bool shouldRetry = await TryConnectSocketWithRetryAsync(socket, hostName);
                if (shouldRetry)
                {
                    // Retry if the first attempt failed because of SSL errors.
                    await TryConnectSocketWithRetryAsync(socket, hostName);
                }
            }
        }

        private async Task<bool> TryConnectSocketWithRetryAsync(StreamSocket socket, HostName hostName)
        {
            try
            {
                // Establish a secure connection to the server (by default, the local IIS server).
                await socket.ConnectAsync(hostName, ServiceNameForConnect.Text, SocketProtectionLevel.Tls12);

                string certInformation = GetCertificateInformation(
                    socket.Information.ServerCertificate,
                    socket.Information.ServerIntermediateCertificates);

                rootPage.NotifyUser(
                    "Connected to server. Certificate information: " + Environment.NewLine + certInformation,
                    NotifyType.StatusMessage);
                return false;
            }
            catch (Exception exception)
            {
                // If this is an unknown status it means that the error is fatal and retry will likely fail.
                if (SocketError.GetStatus(exception.HResult) == SocketErrorStatus.Unknown)
                {
                    throw;
                }

                // If the exception was caused by an SSL error that is ignorable we are going to prompt the user
                // with an enumeration of the errors and ask for permission to ignore.
                if (socket.Information.ServerCertificateErrorSeverity != SocketSslErrorSeverity.Ignorable)
                {
                    rootPage.NotifyUser("Connect failed with error: " + exception.Message, NotifyType.ErrorMessage);
                    return false;
                }
            }

            // Present the certificate issues and ask the user if we should continue.
            if (await ShouldIgnoreCertificateErrorsAsync(socket.Information.ServerCertificateErrors))
            {
                // -----------------------------------------------------------------------------------------------
                // WARNING: Only test applications should ignore SSL errors.
                // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle attacks.
                // -----------------------------------------------------------------------------------------------
                socket.Control.IgnorableServerCertificateErrors.Clear();
                foreach (var ignorableError in socket.Information.ServerCertificateErrors)
                {
                    socket.Control.IgnorableServerCertificateErrors.Add(ignorableError);
                }
                rootPage.NotifyUser("Retrying connection", NotifyType.StatusMessage);
                return true;
            }
            else
            {
                rootPage.NotifyUser("Connection aborted by user (certificate not trusted)", NotifyType.ErrorMessage);
                return false;
            }
        }

        /// <summary>
        /// Allows the user to abort the connection in case of SSL/TLS errors
        /// </summary>
        /// <param name="serverCertificateErrors">The server certificate errors</param>
        /// <returns>False if the connection should be aborted</returns>
        private async Task<bool> ShouldIgnoreCertificateErrorsAsync(
            IReadOnlyList<ChainValidationResult> serverCertificateErrors)
        {
            string connectionErrors = String.Join(", ", serverCertificateErrors);

            string dialogMessage = 
                "The remote server certificate validation failed with the following errors: " +
                connectionErrors + Environment.NewLine + "Security certificate problems may" +
                " indicate an attempt to fool you or intercept any data you send to the server.";

            MessageDialog dialog = new MessageDialog(dialogMessage, "Server Certificate Validation Errors");

            dialog.Commands.Add(new UICommand("Continue (not recommended)", null, continueButtonId));
            dialog.Commands.Add(new UICommand("Cancel", null, abortButtonId));
            dialog.DefaultCommandIndex = 1;
            dialog.CancelCommandIndex = 1;
            
            IUICommand selected = await dialog.ShowAsync();
            return (selected.Id is int) && ((int)selected.Id == continueButtonId);
        }

        /// <summary>
        /// Gets detailed certificate information
        /// </summary>
        /// <param name="serverCert">The server certificate</param>
        /// <param name="intermediateCertificates">The server certificate chain</param>
        /// <returns>A string containing certificate details</returns>
        private string GetCertificateInformation(
            Certificate serverCert, 
            IReadOnlyList<Certificate> intermediateCertificates)
        {
            StringBuilder stringBuilder = new StringBuilder();

            stringBuilder.AppendLine("\tFriendly Name: " + serverCert.FriendlyName);
            stringBuilder.AppendLine("\tSubject: " + serverCert.Subject);
            stringBuilder.AppendLine("\tIssuer: " + serverCert.Issuer);
            stringBuilder.AppendLine("\tValidity: " + serverCert.ValidFrom + " - " + serverCert.ValidTo);

            // Enumerate the entire certificate chain.
            if (intermediateCertificates.Count > 0)
            {
                stringBuilder.AppendLine("\tCertificate chain: ");
                foreach (var cert in intermediateCertificates)
                {
                    stringBuilder.AppendLine("\t\tIntermediate Certificate Subject: " + cert.Subject);
                }
            }
            else
            {
                stringBuilder.AppendLine("\tNo certificates within the intermediate chain.");
            }

            return stringBuilder.ToString();
        }
    }
}
