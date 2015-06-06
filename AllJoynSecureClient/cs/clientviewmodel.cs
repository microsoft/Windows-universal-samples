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

using org.alljoyn.Bus.Samples.Secure;
using SDKTemplate;
using System;
using System.ComponentModel;
using System.Threading.Tasks;
using System.Windows.Input;
using Windows.Devices.AllJoyn;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace AllJoynSecureClient
{
    class ClientViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        private MainPage rootPage;
        private CoreDispatcher m_dispatcher;
        private SecureInterfaceConsumer m_consumer = null;
        private TaskCompletionSource<bool> m_authenticateClicked = null;
        private Visibility m_authVisibility = Visibility.Collapsed;
        private bool isConnected = false;
        private bool isAuthenticated = false;
        private bool isCredentialsRequested = false;
        private string m_key = "";
        private string m_inStr1;
        private string m_inStr2;

        protected async void OnPropertyChanged(string name)
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

        public ClientViewModel()
        {
            m_dispatcher = Windows.UI.Core.CoreWindow.GetForCurrentThread().Dispatcher;
            rootPage = MainPage.Current;
        }

        public Visibility AuthenticationVisibility
        {
            get
            {
                return m_authVisibility;
            }
            private set
            {
                m_authVisibility = value;
                OnPropertyChanged("AuthenticationVisibility");
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
                    OnPropertyChanged("EnteredKey");
                }
            }
        }

        public string InputString1
        {
            get { return m_inStr1; }
            set
            {
                if (value != m_inStr1)
                {
                    m_inStr1 = value;
                    OnPropertyChanged("InputString1");
                }
            }
        }

        public string InputString2
        {
            get { return m_inStr2; }
            set
            {
                if (value != m_inStr2)
                {
                    m_inStr2 = value;
                    OnPropertyChanged("InputString2");
                }
            }
        }

        public ICommand ConnectToServer
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
                    Concatenate();
                });
            }
        }

        public void Start()
        {
            AllJoynBusAttachment busAttachment = new AllJoynBusAttachment();
            busAttachment.AuthenticationMechanisms.Clear();
            busAttachment.AuthenticationMechanisms.Add(AllJoynAuthenticationMechanism.EcdhePsk);
            busAttachment.AuthenticationComplete += BusAttachment_AuthenticationComplete;
            busAttachment.CredentialsRequested += BusAttachment_CredentialsRequested;

            // Initialize a watcher object to listen for about interfaces.
            SecureInterfaceWatcher watcher = new SecureInterfaceWatcher(busAttachment);

            // Subscribing to the added event that will be raised whenever a producer for this service is found.
            watcher.Added += Watcher_Added;

            UpdateStatus("Searching...", NotifyType.StatusMessage);

            // Start watching for producers advertising this service.
            watcher.Start();
        }

        public void Authenticate()
        {
            if (String.IsNullOrWhiteSpace(m_key))
            {
                UpdateStatus("Please enter a key.", NotifyType.ErrorMessage);
            }
            else
            {
                if (m_authenticateClicked != null)
                {
                    UpdateStatus("Authenticating...", NotifyType.StatusMessage);
                    m_authenticateClicked.SetResult(true);
                }
            }
        }

        public async void Concatenate()
        {
            if (isConnected && m_consumer != null)
            {
                if (String.IsNullOrWhiteSpace(m_inStr1) || String.IsNullOrWhiteSpace(m_inStr2))
                {
                    UpdateStatus("Input strings cannot be empty.", NotifyType.ErrorMessage);
                }
                else
                {
                    // Call the Cat method with the input strings arguments.
                    SecureInterfaceCatResult catResult = await m_consumer.CatAsync(m_inStr1, m_inStr2);

                    if (catResult.Status == AllJoynStatus.Ok)
                    {
                        UpdateStatus(String.Format("Concatenation Ouput : {0}.", catResult.OutStr), NotifyType.StatusMessage);
                    }
                    else
                    {
                        UpdateStatus(String.Format("AllJoyn Error : 0x{0:X}.", catResult.Status), NotifyType.ErrorMessage);
                    }
                }
            }
            else
            {
                UpdateStatus("Client not connected.", NotifyType.ErrorMessage);
            }
        }

        private void BusAttachment_AuthenticationComplete(AllJoynBusAttachment sender, AllJoynAuthenticationCompleteEventArgs args)
        {
            if (args.Succeeded)
            {
                UpdateStatus("Authentication was successful.", NotifyType.StatusMessage);
            }
            else
            {
                UpdateStatus("Authentication failed.", NotifyType.ErrorMessage);
            }

            isAuthenticated = args.Succeeded;
            AuthenticationVisibility = Visibility.Collapsed;
        }

        private async void BusAttachment_CredentialsRequested(AllJoynBusAttachment sender, AllJoynCredentialsRequestedEventArgs args)
        {
            Windows.Foundation.Deferral credentialsDeferral = args.GetDeferral();
            m_authenticateClicked = new TaskCompletionSource<bool>();
            isCredentialsRequested = true;
            AuthenticationVisibility = Visibility.Visible;

            // Wait for the user to provide key and click authenticate.
            UpdateStatus("Please enter the key.", NotifyType.StatusMessage);
            await m_authenticateClicked.Task;
            m_authenticateClicked = null;

            if (args.Credentials.AuthenticationMechanism == AllJoynAuthenticationMechanism.EcdhePsk)
            {
                if (!String.IsNullOrEmpty(m_key))
                {
                    args.Credentials.PasswordCredential.Password = m_key;
                }
                else
                {
                    UpdateStatus("Please enter a key.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                UpdateStatus("Unexpected authentication mechanism.", NotifyType.ErrorMessage);
            }

            credentialsDeferral.Complete();
        }

        private async void Watcher_Added(SecureInterfaceWatcher sender, AllJoynServiceInfo args)
        {
            UpdateStatus("Connecting...", NotifyType.StatusMessage);

            // Attempt to join the session when a producer is discovered.
            SecureInterfaceJoinSessionResult joinSessionResult = await SecureInterfaceConsumer.JoinSessionAsync(args, sender);

            if (joinSessionResult.Status == AllJoynStatus.Ok)
            {
                m_consumer = joinSessionResult.Consumer;
                isConnected = true;

                // At the time of connection, the request credentials callback not being invoked is an
                // indication that the client and server are already authenticated from a previous session.
                if (!isCredentialsRequested)
                {
                    UpdateStatus("Connected and already authenticated from previous session.", NotifyType.StatusMessage);
                }
                else
                {
                    if (isAuthenticated)
                    {
                        UpdateStatus("Connected with authentication.", NotifyType.StatusMessage);
                    }
                    else
                    {
                        UpdateStatus("Connected but authentication failed.", NotifyType.ErrorMessage);
                    }
                }
            }
            else
            {
                UpdateStatus(String.Format("Attempt to connect failed with AllJoyn error: 0x{0:X}.", joinSessionResult.Status), NotifyType.ErrorMessage);
            }
        }

        private async void UpdateStatus(string status, NotifyType statusType)
        {
            await m_dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser(status, statusType);
            });
        }
    }
}
