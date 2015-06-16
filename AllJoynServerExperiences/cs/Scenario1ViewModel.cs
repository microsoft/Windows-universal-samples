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

using org.alljoyn.example.Samples.Secure.SecureInterface;
using SDKTemplate;
using System;
using System.ComponentModel;
using System.Threading.Tasks;
using System.Windows.Input;
using Windows.Devices.AllJoyn;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace AllJoynServerExperiences
{
    class Scenario1ViewModel : INotifyPropertyChanged
    {
        private MainPage rootPage;
        private CoreDispatcher m_dispatcher;
        private string m_key = "";
        private bool m_isBusAttachmentConnected = false;
        private AllJoynBusAttachment m_busAttachment = null;
        private SecureInterfaceProducer m_producer = null;
        private Visibility m_serverOptionsVisibility = Visibility.Collapsed;
        private bool? m_hyphenSeparatedIsChecked = null;
        private const int DesiredKeyLength = 24;
        private const int InsertHyphenAfterPosition = 4;

        public Scenario1ViewModel()
        {
            m_dispatcher = CoreWindow.GetForCurrentThread().Dispatcher;
            rootPage = MainPage.Current;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public string Key
        {
            get
            {
                if (m_hyphenSeparatedIsChecked == true)
                {
                    return CryptographicHelpers.InsertHyphens(m_key, InsertHyphenAfterPosition);
                }
                else
                {
                    return m_key;
                }
            }
            private set
            {
                m_key = value;
                RaisePropertyChangedEventAsync("Key");
            }
        }

        public bool? HyphenSeparatedIsChecked
        {
            get
            {
                return m_hyphenSeparatedIsChecked;
            }
            set
            {
                m_hyphenSeparatedIsChecked = value;
                RaisePropertyChangedEventAsync("Key");
            }
        }

        public bool IsUpperCaseEnabled
        {
            get
            {
                return AppData.IsUpperCaseEnabled;
            }
            set
            {
                AppData.IsUpperCaseEnabled = value;
                if (m_producer != null)
                {
                    Task task = new Task(() =>
                    {
                        // Note: This method blocks the calling thread, and hence should never be called from a UI thread. 
                        m_producer.EmitIsUpperCaseEnabledChanged();
                    });
                    task.Start();
                }
            }
        }

        public string SignalText { get; set; }

        public Visibility ServerOptionsVisibility
        {
            get
            {
                return m_serverOptionsVisibility;
            }
            private set
            {
                m_serverOptionsVisibility = value;
                RaisePropertyChangedEventAsync("ServerOptionsVisibility");
            }
        }

        public ICommand LaunchServer
        {
            get
            {
                return new RelayCommand((object args) =>
                {
                    Start();
                });
            }
        }

        public ICommand SendSignal
        {
            get
            {
                return new RelayCommand((object args) =>
                {
                    SendTextSignal();
                });
            }
        }

        public void ScenarioCleanup()
        {
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

        protected async void RaisePropertyChangedEventAsync(string name)
        {
            await m_dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                PropertyChangedEventHandler handler = PropertyChanged;
                if (handler != null)
                {
                    handler(this, new PropertyChangedEventArgs(name));
                }
            });
        }

        private void Start()
        {
            // Prevent from Launching the server again if it is already launched.
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
                m_busAttachment.AuthenticationMechanisms.Add(AllJoynAuthenticationMechanism.EcdhePsk);
                m_busAttachment.AuthenticationComplete += AlljoynBusAttachment_AuthenticationComplete;
                m_busAttachment.CredentialsRequested += AlljoynBusAttachment_CredentialsRequested;

                // Generate the one-time pre-shared key.
                Key = CryptographicHelpers.GenerateHighEntropyKey(DesiredKeyLength);

                // Initialize the producer object generated by the AllJoynCodeGen tool.
                m_producer = new SecureInterfaceProducer(m_busAttachment);

                // Instantiate SecureInterfaceService which will handle the concatenation method calls.
                m_producer.Service = new SecureInterfaceService();

                // Start listening for consumers.
                m_producer.Start();
            }
        }

        private void SendTextSignal()
        {
            if (m_producer != null)
            {
                if (!string.IsNullOrWhiteSpace(SignalText))
                {
                    m_producer.Signals.TextSent(SignalText);
                    UpdateStatusAsync("Signal Sent.", NotifyType.StatusMessage);
                }
                else
                {
                    UpdateStatusAsync("No text entered.", NotifyType.ErrorMessage);
                }
            }
        }

        private void AlljoynBusAttachment_StateChanged(AllJoynBusAttachment sender, AllJoynBusAttachmentStateChangedEventArgs args)
        {
            switch (args.State)
            {
                case AllJoynBusAttachmentState.Disconnected:
                    UpdateStatusAsync(string.Format("Disconnected from the AllJoyn bus attachment with AllJoyn status: 0x{0:X}.", args.Status), NotifyType.StatusMessage);
                    m_isBusAttachmentConnected = false;
                    ServerOptionsVisibility = Visibility.Collapsed;
                    AppData.IsUpperCaseEnabledPropertyDataChanged -= AppData_IsUpperCaseEnabledPropertyDataChanged;
                    break;
                case AllJoynBusAttachmentState.Connected:
                    UpdateStatusAsync("Launched.", NotifyType.StatusMessage);
                    m_isBusAttachmentConnected = true;
                    ServerOptionsVisibility = Visibility.Visible;
                    AppData.IsUpperCaseEnabledPropertyDataChanged += AppData_IsUpperCaseEnabledPropertyDataChanged;
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
            if (args.Credentials.AuthenticationMechanism == AllJoynAuthenticationMechanism.EcdhePsk)
            {
                args.Credentials.PasswordCredential.Password = m_key;
            }
            else
            {
                UpdateStatusAsync("Unexpected authentication mechanism.", NotifyType.ErrorMessage);
            }
        }

        private void AppData_IsUpperCaseEnabledPropertyDataChanged(object sender, EventArgs e)
        {
            RaisePropertyChangedEventAsync("IsUpperCaseEnabled");
        }

        private async void UpdateStatusAsync(string status, NotifyType statusType)
        {
            await m_dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser(status, statusType);
            });
        }
    }
}
