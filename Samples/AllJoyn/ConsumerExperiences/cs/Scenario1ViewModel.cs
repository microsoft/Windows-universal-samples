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

using com.microsoft.Samples.SecureInterface;
using SDKTemplate;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using System.Windows.Input;
using Windows.Data.Xml.Dom;
using Windows.Devices.AllJoyn;
using Windows.Devices.Enumeration;
using Windows.UI.Core;
using Windows.UI.Notifications;
using Windows.UI.Xaml;

namespace AllJoynConsumerExperiences
{
    class Scenario1ViewModel : INotifyPropertyChanged
    {
        private MainPage m_rootPage;
        private CoreDispatcher m_dispatcher;
        private AllJoynBusAttachment m_busAttachment = null;
        private DeviceWatcher m_watcher = null;
        private SecureInterfaceConsumer m_consumer = null;
        private TaskCompletionSource<bool> m_authenticateClicked = null;
        private Visibility m_authVisibility = Visibility.Collapsed;
        private Visibility m_consumerOptionsVisibility = Visibility.Collapsed;
        private bool m_isAuthenticated = false;
        private bool m_isCredentialsRequested = false;
        private bool m_isUpperCaseEnabled = false;
        private bool m_callSetProperty = true;
        private string m_key = string.Empty;

        public Scenario1ViewModel()
        {
            m_dispatcher = CoreWindow.GetForCurrentThread().Dispatcher;
            m_rootPage = MainPage.Current;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public Visibility AuthenticationVisibility
        {
            get
            {
                return m_authVisibility;
            }
            private set
            {
                m_authVisibility = value;
                RaisePropertyChangedEventAsync();
            }
        }

        public Visibility ConsumerOptionsVisibility
        {
            get
            {
                return m_consumerOptionsVisibility;
            }
            private set
            {
                m_consumerOptionsVisibility = value;
                RaisePropertyChangedEventAsync();
            }
        }

        public string EnteredKey
        {
            get { return m_key; }
            set
            {
                if (value != m_key)
                {
                    m_key = value;
                    RaisePropertyChangedEventAsync();
                }
            }
        }

        public string InputString1 { get; set; }

        public string InputString2 { get; set; }

        public bool IsUpperCaseEnabled
        {
            get
            {
                return m_isUpperCaseEnabled;
            }
            set
            {
                m_isUpperCaseEnabled = value;
                RaisePropertyChangedEventAsync();
                if (m_callSetProperty)
                {
                    SetIsUpperCaseEnabledAsync(m_isUpperCaseEnabled);
                }
                else
                {
                    m_callSetProperty = true;
                }
            }
        }

        public ICommand ConnectToProducer
        {
            get
            {
                return new RelayCommand((object args) =>
                {
                    Start();
                });
            }
        }

        public ICommand AttemptAuthentication
        {
            get
            {
                return new RelayCommand((object args) =>
                {
                    Authenticate();
                });
            }
        }

        public ICommand RequestConcatenate
        {
            get
            {
                return new RelayCommand((object args) =>
                {
                    ConcatenateAsync();
                });
            }
        }

        public void ScenarioCleanup()
        {
            DisposeConsumer();
            DisposeWatcher();

            if (m_busAttachment != null)
            {
                m_busAttachment.CredentialsRequested -= BusAttachment_CredentialsRequested;
                m_busAttachment.AuthenticationComplete -= BusAttachment_AuthenticationComplete;
                m_busAttachment.StateChanged -= BusAttachment_StateChanged;
                m_busAttachment.Disconnect();
                m_busAttachment = null;
            }
        }

        protected async void RaisePropertyChangedEventAsync([CallerMemberName] string name = "")
        {
            await m_dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }

        private void Start()
        {
            ScenarioCleanup();

            m_busAttachment = new AllJoynBusAttachment();
            m_busAttachment.StateChanged += BusAttachment_StateChanged;
            m_busAttachment.AuthenticationMechanisms.Clear();
            m_busAttachment.AuthenticationMechanisms.Add(AllJoynAuthenticationMechanism.EcdheSpeke);
            m_busAttachment.AuthenticationComplete += BusAttachment_AuthenticationComplete;
            m_busAttachment.CredentialsRequested += BusAttachment_CredentialsRequested;

            // Initialize a watcher object to listen for about interfaces.
            m_watcher = AllJoynBusAttachment.GetWatcher(new List<string> { "com.microsoft.Samples.SecureInterface" });

            // Subscribing to the added event that will be raised whenever a producer for this service is found.
            m_watcher.Added += Watcher_Added;

            UpdateStatusAsync("Searching...", NotifyType.StatusMessage);

            // Start watching for producers advertising this service.
            m_watcher.Start();
        }

        private void Authenticate()
        {
            if (string.IsNullOrWhiteSpace(m_key))
            {
                UpdateStatusAsync("Please enter a key.", NotifyType.ErrorMessage);
            }
            else
            {
                if (m_authenticateClicked != null)
                {
                    UpdateStatusAsync("Authenticating...", NotifyType.StatusMessage);
                    m_authenticateClicked.SetResult(true);
                }
            }
        }

        private async void ConcatenateAsync()
        {
            if (m_consumer != null)
            {
                if (string.IsNullOrWhiteSpace(InputString1) || string.IsNullOrWhiteSpace(InputString2))
                {
                    UpdateStatusAsync("Input strings cannot be empty.", NotifyType.ErrorMessage);
                }
                else
                {
                    // Call the Concatenate method with the input strings arguments.
                    SecureInterfaceConcatenateResult catResult = await m_consumer.ConcatenateAsync(InputString1, InputString2);

                    if (catResult.Status == AllJoynStatus.Ok)
                    {
                        UpdateStatusAsync(string.Format("Concatenation output : \"{0}\".", catResult.OutStr), NotifyType.StatusMessage);
                    }
                    else
                    {
                        UpdateStatusAsync(string.Format("AllJoyn Error : 0x{0:X}.", catResult.Status), NotifyType.ErrorMessage);
                    }
                }
            }
            else
            {
                UpdateStatusAsync("Consumer not connected.", NotifyType.ErrorMessage);
            }
        }

        private void BusAttachment_StateChanged(AllJoynBusAttachment sender, AllJoynBusAttachmentStateChangedEventArgs args)
        {
            if (args.State == AllJoynBusAttachmentState.Disconnected)
            {
                UpdateStatusAsync(string.Format("AllJoyn bus attachment has disconnected with AllJoyn error: 0x{0:X}.", args.Status), NotifyType.StatusMessage);
                ConsumerOptionsVisibility = Visibility.Collapsed;
            }
        }

        private void BusAttachment_AuthenticationComplete(AllJoynBusAttachment sender, AllJoynAuthenticationCompleteEventArgs args)
        {
            if (args.Succeeded)
            {
                UpdateStatusAsync("Authentication was successful.", NotifyType.StatusMessage);
            }
            else
            {
                UpdateStatusAsync("Authentication failed.", NotifyType.ErrorMessage);
            }

            m_isAuthenticated = args.Succeeded;
            EnteredKey = "";
            AuthenticationVisibility = Visibility.Collapsed;
        }

        private async void BusAttachment_CredentialsRequested(AllJoynBusAttachment sender, AllJoynCredentialsRequestedEventArgs args)
        {
            Windows.Foundation.Deferral credentialsDeferral = args.GetDeferral();
            m_authenticateClicked = new TaskCompletionSource<bool>();
            m_isCredentialsRequested = true;
            AuthenticationVisibility = Visibility.Visible;

            // Wait for the user to provide key and click authenticate.
            UpdateStatusAsync("Please enter the key.", NotifyType.StatusMessage);
            await m_authenticateClicked.Task;
            m_authenticateClicked = null;

            if (args.Credentials.AuthenticationMechanism == AllJoynAuthenticationMechanism.EcdheSpeke)
            {
                if (!string.IsNullOrEmpty(m_key))
                {
                    args.Credentials.PasswordCredential.Password = m_key;
                }
                else
                {
                    UpdateStatusAsync("Please enter a key.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                UpdateStatusAsync("Unexpected authentication mechanism.", NotifyType.ErrorMessage);
            }

            credentialsDeferral.Complete();
        }

        private async void Watcher_Added(DeviceWatcher sender, DeviceInformation args)
        {
            // Optional - Get the About data of the producer. 
            AllJoynAboutDataView aboutData = await m_busAttachment.GetAboutDataAsync(await AllJoynServiceInfo.FromIdAsync(args.Id));

            // Check to see if device name is populated in the about data, since device name is not a mandatory field.
            if (string.IsNullOrEmpty(aboutData.DeviceName))
            {
                UpdateStatusAsync(string.Format("Found {0} from manufacturer: {1}. Connecting...", aboutData.AppName, aboutData.Manufacturer), NotifyType.StatusMessage);
            }
            else
            {
                UpdateStatusAsync(string.Format("Found {0} on {1} from manufacturer: {2}. Connecting...", aboutData.AppName, aboutData.DeviceName, aboutData.Manufacturer), NotifyType.StatusMessage);
            }

            DisposeConsumer();
            UpdateStatusAsync("Joining session...", NotifyType.StatusMessage);
            m_consumer = await SecureInterfaceConsumer.FromIdAsync(args.Id, m_busAttachment);
            if (m_consumer != null)
            {
                m_consumer.IsUpperCaseEnabledChanged += Consumer_IsUpperCaseEnabledChanged;
                m_consumer.Signals.TextSentReceived += Signals_TextSentReceived;
                m_consumer.Session.Lost += Consumer_SessionLost;

                // At the time of connection, the request credentials callback not being invoked is an
                // indication that the consumer and producer are already authenticated from a previous session.
                if (!m_isCredentialsRequested)
                {
                    UpdateStatusAsync("Connected and already authenticated from previous session.", NotifyType.StatusMessage);
                    UpdateIsUpperCaseEnabledAsync();
                }
                else
                {
                    if (m_isAuthenticated)
                    {
                        UpdateStatusAsync("Connected with authentication.", NotifyType.StatusMessage);
                        UpdateIsUpperCaseEnabledAsync();
                    }
                    else
                    {
                        UpdateStatusAsync("Connected but authentication failed.", NotifyType.ErrorMessage);
                    }
                }
                ConsumerOptionsVisibility = Visibility.Visible;
            }
            else
            {
                UpdateStatusAsync("Attempt to join session failed.", NotifyType.ErrorMessage);
            }
        }

        private void Consumer_SessionLost(AllJoynSession sender, AllJoynSessionLostEventArgs args)
        {
            UpdateStatusAsync(string.Format("AllJoyn session with the producer lost due to {0}.", args.Reason.ToString()), NotifyType.StatusMessage);
            ConsumerOptionsVisibility = Visibility.Collapsed;
            DisposeConsumer();
        }

        private void Signals_TextSentReceived(SecureInterfaceSignals sender, SecureInterfaceTextSentReceivedEventArgs args)
        {
            // Show UI Toast.
            ToastTemplateType toastTemplate = ToastTemplateType.ToastText02;
            XmlDocument toastXml = ToastNotificationManager.GetTemplateContent(toastTemplate);

            // Populate UI Toast.
            XmlNodeList toastTextElements = toastXml.GetElementsByTagName("text");
            toastTextElements[0].AppendChild(toastXml.CreateTextNode("Signal Received - " + args.Message));

            // Create and Send UI Toast.
            ToastNotification toast = new ToastNotification(toastXml);
            ToastNotificationManager.CreateToastNotifier().Show(toast);

            UpdateStatusAsync("Signal Received - " + args.Message, NotifyType.StatusMessage);
        }

        private void Consumer_IsUpperCaseEnabledChanged(SecureInterfaceConsumer sender, object args)
        {
            UpdateIsUpperCaseEnabledAsync();
        }

        private async void UpdateIsUpperCaseEnabledAsync()
        {
            SecureInterfaceGetIsUpperCaseEnabledResult getIsUpperCaseEnabledResult = await m_consumer.GetIsUpperCaseEnabledAsync();
            if (getIsUpperCaseEnabledResult.Status == AllJoynStatus.Ok)
            {
                m_callSetProperty = false;
                IsUpperCaseEnabled = getIsUpperCaseEnabledResult.IsUpperCaseEnabled;
            }
            else
            {
                UpdateStatusAsync(string.Format("Get property failed with AllJoyn error: 0x{0:X}.", getIsUpperCaseEnabledResult.Status), NotifyType.ErrorMessage);
            }
        }

        private async void SetIsUpperCaseEnabledAsync(bool value)
        {
            SecureInterfaceSetIsUpperCaseEnabledResult setIsUpperCaseEnabledResult = await m_consumer.SetIsUpperCaseEnabledAsync(value);
            if (setIsUpperCaseEnabledResult.Status == AllJoynStatus.Ok)
            {
                UpdateStatusAsync("\"IsUpperCaseEnabled\" property successfully set.", NotifyType.StatusMessage);
            }
            else
            {
                UpdateStatusAsync(string.Format("Set property failed with AllJoyn error: 0x{0:X}.", setIsUpperCaseEnabledResult.Status), NotifyType.ErrorMessage);
                m_callSetProperty = false;
                IsUpperCaseEnabled = !value;
            }
        }

        private void DisposeConsumer()
        {
            if (m_consumer != null)
            {
                m_consumer.Session.Lost -= Consumer_SessionLost;
                m_consumer.Signals.TextSentReceived -= Signals_TextSentReceived;
                m_consumer.IsUpperCaseEnabledChanged -= Consumer_IsUpperCaseEnabledChanged;
                m_consumer.Dispose();
                m_consumer = null;
            }
        }

        private void DisposeWatcher()
        {
            if (m_watcher != null)
            {
                m_watcher.Added -= Watcher_Added;
                m_watcher.Stop();
                m_watcher = null;
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
