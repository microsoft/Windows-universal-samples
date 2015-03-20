//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
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
using System.Windows.Input;
using Windows.Devices.AllJoyn;
using Windows.UI.Core;

namespace AllJoynSecureClient
{
    class ClientViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        private MainPage rootPage;
        private CoreDispatcher m_dispatcher;
        private SecureInterfaceConsumer m_consumer = null;
        private bool isConnected = false;
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

        private async void UpdateStatus(string status, NotifyType statusType)
        {
            await m_dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser(status, statusType);
            });
        }

        public ClientViewModel()
        {
            m_dispatcher = Windows.UI.Core.CoreWindow.GetForCurrentThread().Dispatcher;
            rootPage = MainPage.Current;
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

        private async void Watcher_Added(SecureInterfaceWatcher sender, AllJoynServiceInfo args)
        {
            // Attempt to join the session when a producer is discovered.
            SecureInterfaceJoinSessionResult joinSessionResult = await SecureInterfaceConsumer.JoinSessionAsync(args, sender);

            if (joinSessionResult.Status == AllJoynStatus.Ok)
            {
                m_consumer = joinSessionResult.Consumer;
                isConnected = true;
                UpdateStatus("Connected.", NotifyType.StatusMessage);
            }
            else
            {
                UpdateStatus(String.Format("Attempt to connect failed with error: {0}", joinSessionResult.Status), NotifyType.ErrorMessage);
            }
        }

        public void Start()
        {
            UpdateStatus("Connecting...", NotifyType.StatusMessage);

            // Create a new AllJoyn bus attachment.
            AllJoynBusAttachment busAttachment = new AllJoynBusAttachment();

            // Initialize a watcher object to listen for about interfaces.
            SecureInterfaceWatcher watcher = new SecureInterfaceWatcher(busAttachment);

            // Subscribing to the added event that will fire whenever a producer for this service is found.
            watcher.Added += Watcher_Added;

            // Start watching for producers advertising this service.
            watcher.Start();
        }

        public async void Concatenate()
        {
            if (isConnected && m_consumer != null)
            {
                if (String.IsNullOrWhiteSpace(m_inStr1) || String.IsNullOrWhiteSpace(m_inStr2))
                {
                    UpdateStatus("Input strings cannot be empty", NotifyType.ErrorMessage);
                }
                else
                {
                    // Call the Cat method with the input strings arguments.
                    SecureInterfaceCatResult catResult = await m_consumer.CatAsync(m_inStr1, m_inStr2);

                    if (catResult.Status == AllJoynStatus.Ok)
                    {
                        UpdateStatus(String.Format("Concatenation Ouput : {0}", catResult.outStr), NotifyType.StatusMessage);
                    }
                    else
                    {
                        UpdateStatus(String.Format("Error : {0}", catResult.Status.ToString()), NotifyType.ErrorMessage);
                    }
                }
            }
            else
            {
                UpdateStatus("Client not connected.", NotifyType.ErrorMessage);
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
    }
}
