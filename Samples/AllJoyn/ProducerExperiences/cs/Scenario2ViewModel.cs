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

using org.alljoyn.Onboarding;
using SDKTemplate;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using System.Windows.Input;
using Windows.Devices.AllJoyn;
using Windows.Security.Cryptography;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace AllJoynProducerExperiences
{
    class Scenario2ViewModel : INotifyPropertyChanged
    {
        private MainPage m_rootPage;
        private CoreDispatcher m_dispatcher;
        private bool m_isBusAttachmentConnected = false;
        private AllJoynBusAttachment m_busAttachment = null;
        private OnboardingProducer m_producer = null;
        private OnboardingScanListItem m_selectedSampleNetwork;
        private Visibility m_sampleNetworkInfoVisibility = Visibility.Collapsed;
        private string m_key = "";

        public Scenario2ViewModel()
        {
            m_dispatcher = CoreWindow.GetForCurrentThread().Dispatcher;
            m_rootPage = MainPage.Current;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public string Key
        {
            get
            {
                return m_key;
            }
            private set
            {
                m_key = value;
                RaisePropertyChangedEventAsync();
            }
        }

        public List<OnboardingScanListItem> SampleNetworkList
        {
            get
            {
                return AppData.SampleNetworkScanList;
            }
        }

        public OnboardingScanListItem SelectedSampleNetwork
        {
            get
            {
                return m_selectedSampleNetwork;
            }
            set
            {
                m_selectedSampleNetwork = value;
                RaisePropertyChangedEventAsync("SelectedSampleNetworkPassword");
                RaisePropertyChangedEventAsync("SelectedSampleNetworkAuthentication");
            }
        }

        public string SelectedSampleNetworkPassword
        {
            get
            {
                if (m_selectedSampleNetwork != null)
                {
                    if (m_selectedSampleNetwork.Value2 == (short)OnboardingAuthenticationType.Open)
                    {
                        return "";
                    }
                    else
                    {
                        return AppData.SampleNetworkPassword;
                    }
                }
                else
                {
                    return "";
                }
            }
        }

        public string SelectedSampleNetworkAuthentication
        {
            get
            {
                if (m_selectedSampleNetwork != null)
                {
                    return ((OnboardingAuthenticationType)m_selectedSampleNetwork.Value2).ToString();
                }
                else
                {
                    return "";
                }
            }
        }

        public Visibility SampleNetworkInfoVisibility
        {
            get
            {
                return m_sampleNetworkInfoVisibility;
            }
            private set
            {
                m_sampleNetworkInfoVisibility = value;
                RaisePropertyChangedEventAsync();
            }
        }

        public ICommand LaunchProducer
        {
            get
            {
                return new RelayCommand((object args) =>
                {
                    Start();
                });
            }
        }

        public void ScenarioCleanup()
        {
            OnboardingService.ConnectRequestRecieved -= OnboardingService_ConnectRequestRecieved;

            if (m_producer != null)
            {
                m_producer.Stop();
                m_producer.Dispose();
            }

            if (m_busAttachment != null)
            {
                m_busAttachment.CredentialsRequested -= AlljoynBusAttachment_CredentialsRequested;
                m_busAttachment.AuthenticationComplete -= AlljoynBusAttachment_AuthenticationComplete;
                m_busAttachment.StateChanged -= AlljoynBusAttachment_StateChanged;
                m_busAttachment.Disconnect();
            }
        }

        protected async void RaisePropertyChangedEventAsync([CallerMemberName]string name = "")
        {
            await m_dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }

        private void Start()
        {
            // Prevent launching the producer again if it is already launched.
            if (m_isBusAttachmentConnected)
            {
                UpdateStatusAsync("Already launched.", NotifyType.StatusMessage);
            }
            else
            {
                UpdateStatusAsync("Launching...", NotifyType.StatusMessage);

                m_busAttachment = new AllJoynBusAttachment();
                m_busAttachment.StateChanged += AlljoynBusAttachment_StateChanged;
                m_busAttachment.AuthenticationMechanisms.Clear();
                m_busAttachment.AuthenticationMechanisms.Add(AllJoynAuthenticationMechanism.EcdheSpeke);
                m_busAttachment.AuthenticationComplete += AlljoynBusAttachment_AuthenticationComplete;
                m_busAttachment.CredentialsRequested += AlljoynBusAttachment_CredentialsRequested;

                // Generate the one-time 6-digit numeric pre-shared key.
                Random rnd = new Random();
                Key = rnd.Next(100000, 1000000).ToString();

                // Initialize the producer object generated by the AllJoynCodeGenerator tool.
                m_producer = new OnboardingProducer(m_busAttachment);

                // Instantiate OnboardingService which will handle the onboarding method calls.
                m_producer.Service = new OnboardingService();
                OnboardingService.ConnectRequestRecieved += OnboardingService_ConnectRequestRecieved;

                // Start listening for consumers.
                m_producer.Start();
            }
        }

        private void AlljoynBusAttachment_StateChanged(AllJoynBusAttachment sender, AllJoynBusAttachmentStateChangedEventArgs args)
        {
            switch (args.State)
            {
                case AllJoynBusAttachmentState.Disconnected:
                    UpdateStatusAsync(string.Format("Disconnected from the AllJoyn bus attachment with AllJoyn status: 0x{0:X}.", args.Status), NotifyType.StatusMessage);
                    m_isBusAttachmentConnected = false;
                    SampleNetworkInfoVisibility = Visibility.Collapsed;
                    break;
                case AllJoynBusAttachmentState.Connected:
                    UpdateStatusAsync("Launched.", NotifyType.StatusMessage);
                    m_isBusAttachmentConnected = true;
                    SampleNetworkInfoVisibility = Visibility.Visible;
                    break;
                default:
                    break;
            }
        }

        private void AlljoynBusAttachment_AuthenticationComplete(AllJoynBusAttachment sender, AllJoynAuthenticationCompleteEventArgs args)
        {
            if (args.Succeeded)
            {
                UpdateStatusAsync("Authentication was successful.", NotifyType.StatusMessage);
            }
            else
            {
                UpdateStatusAsync("Authentication failed.", NotifyType.ErrorMessage);
            }
        }

        private void AlljoynBusAttachment_CredentialsRequested(AllJoynBusAttachment sender, AllJoynCredentialsRequestedEventArgs args)
        {
            if (args.Credentials.AuthenticationMechanism == AllJoynAuthenticationMechanism.EcdheSpeke)
            {
                args.Credentials.PasswordCredential.Password = m_key;
            }
            else
            {
                UpdateStatusAsync("Unexpected authentication mechanism.", NotifyType.ErrorMessage);
            }
        }

        private async void OnboardingService_ConnectRequestRecieved(object sender, EventArgs e)
        {
            UpdateStatusAsync("Connect Request received. Attempting to connect...", NotifyType.StatusMessage);
            AppData.OnboardingConfigurationState = (short)ConfigurationState.Validating;

            // 5 second delay to simulate connection attempt time.
            await Task.Delay(TimeSpan.FromSeconds(5));
            Onboarding returnArg = new Onboarding();

            if (string.IsNullOrWhiteSpace(AppData.OnboardingConfigureSsid))
            {
                returnArg.Value1 = (short)ConnectionResultCode.ErrorMessage;
                returnArg.Value2 = "Invalid SSID";
                AppData.OnboardingConfigurationState = (short)ConfigurationState.Error;
            }
            else
            {
                bool foundSsid = false;
                bool supportedProtocol = false;

                foreach (OnboardingScanListItem item in AppData.SampleNetworkScanList)
                {
                    if (item.Value1.Equals(AppData.OnboardingConfigureSsid))
                    {
                        foundSsid = true;
                        if (AppData.OnboardingConfigureAuthType == (short)OnboardingAuthenticationType.Any)
                        {
                            supportedProtocol = true;
                        }
                        else if (AppData.OnboardingConfigureAuthType == (short)OnboardingAuthenticationType.WpaAuto)
                        {
                            if ((item.Value2 == (short)OnboardingAuthenticationType.WpaTkip) || (item.Value2 == (short)OnboardingAuthenticationType.WpaCcmp))
                            {
                                supportedProtocol = true;
                            }
                        }
                        else if (AppData.OnboardingConfigureAuthType == (short)OnboardingAuthenticationType.Wpa2Auto)
                        {
                            if ((item.Value2 == (short)OnboardingAuthenticationType.Wpa2Tkip) || (item.Value2 == (short)OnboardingAuthenticationType.Wpa2Ccmp))
                            {
                                supportedProtocol = true;
                            }
                        }
                        else
                        {
                            if (item.Value2 == AppData.OnboardingConfigureAuthType)
                            {
                                supportedProtocol = true;
                            }
                        }
                        break;
                    }
                }

                if (foundSsid)
                {
                    if (supportedProtocol)
                    {
                        if (AppData.OnboardingConfigureAuthType == (short)OnboardingAuthenticationType.Open)
                        {
                            returnArg.Value1 = (short)ConnectionResultCode.Validated;
                            returnArg.Value2 = "Connected successfully";
                            AppData.OnboardingConfigurationState = (short)ConfigurationState.Validated;
                        }
                        else
                        {
                            if (AppData.OnboardingConfigurePassphrase.Equals(ConvertUtf8ToHex(AppData.SampleNetworkPassword), StringComparison.OrdinalIgnoreCase))
                            {
                                returnArg.Value1 = (short)ConnectionResultCode.Validated;
                                returnArg.Value2 = "Connected successfully";
                                AppData.OnboardingConfigurationState = (short)ConfigurationState.Validated;
                            }
                            else
                            {
                                returnArg.Value1 = (short)ConnectionResultCode.Unauthorized;
                                returnArg.Value2 = "Invalid password";
                                AppData.OnboardingConfigurationState = (short)ConfigurationState.Error;
                            }
                        }
                    }
                    else
                    {
                        returnArg.Value1 = (short)ConnectionResultCode.UnsupportedProtocol;
                        returnArg.Value2 = "Unsupported authentication mechanism";
                        AppData.OnboardingConfigurationState = (short)ConfigurationState.Error;
                    }
                }
                else
                {
                    returnArg.Value1 = (short)ConnectionResultCode.Unreachable;
                    returnArg.Value2 = "Configured SSID unreachable";
                    AppData.OnboardingConfigurationState = (short)ConfigurationState.Error;
                }
            }

            m_producer.Signals.ConnectionResult(returnArg);
            AppData.LastErrorCode = returnArg.Value1;
            AppData.LastErrorMessage = returnArg.Value2;

            if (returnArg.Value1 == (short)ConnectionResultCode.Validated)
            {
                UpdateStatusAsync("Onboarding successful. Connected to " + AppData.OnboardingConfigureSsid, NotifyType.StatusMessage);
            }
            else
            {
                UpdateStatusAsync(string.Format("Onboarding Failed. Attempt to connect failed with result code: {0} and message: {1}.", ((ConnectionResultCode)returnArg.Value1).ToString(), returnArg.Value2), NotifyType.ErrorMessage);
            }
        }

        static private string ConvertUtf8ToHex(string inputString)
        {
            if (string.IsNullOrEmpty(inputString))
            {
                return string.Empty;
            }
            else
            {
                var tempBuffer = CryptographicBuffer.ConvertStringToBinary(inputString, BinaryStringEncoding.Utf8);
                var hexString = CryptographicBuffer.EncodeToHexString(tempBuffer);
                return hexString;
            }
        }

        private async void UpdateStatusAsync(string status, NotifyType statusType)
        {
            await m_dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                m_rootPage.NotifyUser(status, statusType);
            });
        }
    }
}
