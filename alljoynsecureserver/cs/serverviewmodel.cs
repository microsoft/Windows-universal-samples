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

namespace AllJoynSecureServer
{
    class ServerViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        private MainPage rootPage;
        private CoreDispatcher m_dispatcher;

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

        public ServerViewModel()
        {
            m_dispatcher = Windows.UI.Core.CoreWindow.GetForCurrentThread().Dispatcher;
            rootPage = MainPage.Current;
        }

        public void Start()
        {
            UpdateStatus("Launching...", NotifyType.StatusMessage);

            // Create a new AllJoyn bus attachment.
            AllJoynBusAttachment alljoynBusAttachment = new AllJoynBusAttachment();

            // Initialize the producer object - where SecureInterfaceProducer is the generated class using AllJoynCodeGen tool.
            SecureInterfaceProducer producer = new SecureInterfaceProducer(alljoynBusAttachment);

            // Initialize an object of SecureInterfaceService - the implementation of ISecureInterfaceService that will handle the concatenation method calls.
            producer.Service = new SecureInterfaceService();

            // Create interface as defined in the introspect xml, create AllJoyn bus object
            // and announce the about interface.
            producer.Start();

            UpdateStatus("Launched.", NotifyType.StatusMessage);
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
    }

}
