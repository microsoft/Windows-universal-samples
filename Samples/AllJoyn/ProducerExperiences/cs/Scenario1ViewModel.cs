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
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Input;
using Windows.Devices.AllJoyn;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace AllJoynProducerExperiences
{
    class Scenario1ViewModel : INotifyPropertyChanged
    {
        private MainPage rootPage;
        private CoreDispatcher m_dispatcher;
        private string m_key = "";
        private bool m_isBusAttachmentConnected = false;
        private AllJoynBusAttachment m_busAttachment = null;
        private SecureInterfaceProducer m_producer = null;
        private Visibility m_producerOptionsVisibility = Visibility.Collapsed;

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
                return m_key;
            }
            private set
            {
                m_key = value;
                RaisePropertyChangedEventAsync();
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
                    m_producer.EmitIsUpperCaseEnabledChanged();
                }
            }
        }

        public string SignalText { get; set; }

        public Visibility ProducerOptionsVisibility
        {
            get
            {
                return m_producerOptionsVisibility;
            }
            private set
            {
                m_producerOptionsVisibility = value;
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

        protected async void RaisePropertyChangedEventAsync([CallerMemberName] string name = "")
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

                // Optional - Populate About data for the producer.
                m_busAttachment.AboutData.DefaultManufacturer = "Contoso";
                m_busAttachment.AboutData.SupportUrl = new Uri("http://www.contoso.com/support");
                m_busAttachment.AboutData.ModelNumber = "ContosoTestModel";
                m_busAttachment.AboutData.DefaultDescription = "A secure AllJoyn sample";

                // Generate the one-time 6-digit numeric pre-shared key.
                Random rnd = new Random();
                Key = rnd.Next(100000, 1000000).ToString();

                // Initialize the producer object generated by the AllJoynCodeGenerator tool.
                m_producer = new SecureInterfaceProducer(m_busAttachment);

                // Instantiate SecureInterfaceService which will handle the concatenation method calls.
                m_producer.Service = new SecureInterfaceService();

                // Start advertising the service.
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
                    UpdateStatusAsync("Signal sent.", NotifyType.StatusMessage);
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
                    ProducerOptionsVisibility = Visibility.Collapsed;
                    AppData.IsUpperCaseEnabledPropertyDataChanged -= AppData_IsUpperCaseEnabledPropertyDataChanged;
                    break;
                case AllJoynBusAttachmentState.Connected:
                    UpdateStatusAsync("Launched.", NotifyType.StatusMessage);
                    m_isBusAttachmentConnected = true;
                    ProducerOptionsVisibility = Visibility.Visible;
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
            if (args.Credentials.AuthenticationMechanism == AllJoynAuthenticationMechanism.EcdheSpeke)
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
