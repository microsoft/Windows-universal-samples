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
using System.Globalization;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        /// <summary>
        /// UI that exposes a way to open TCP/UDP ports and connect sockets between two devices connected over Wi-Fi Direct Services
        /// This sample just demonstrates setting up the connection and sending strings
        /// A real application would likely open pre-determined ports for application communication
        ///
        /// Most of the actual logic for Wi-Fi Direct Services is in WiFiDirectServicesManager.cs and WiFiDirectServicesWrappers.cs
        /// </summary>
        public sealed partial class Scenario5 : Page
        {
            private Object thisLock = new Object();

            private MainPage rootPage;
            private SessionWrapper session = null;

            public Scenario5()
            {
                this.InitializeComponent();
            }

            protected override void OnNavigatedTo(NavigationEventArgs e)
            {
                lock (thisLock)
                {
                    rootPage = MainPage.Current;

                    MainUI.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                    PlaceholderUI.Visibility = Windows.UI.Xaml.Visibility.Visible;

                    session = WiFiDirectServiceManager.Instance.CurrentSession;

                    if (session != null)
                    {
                        AdvertisementId.Text = session.AdvertisementId.ToString(CultureInfo.InvariantCulture);
                        ServiceAddress.Text = session.ServiceAddress;
                        SessionId.Text = session.SessionId.ToString(CultureInfo.InvariantCulture);
                        SessionAddress.Text = session.SessionAddress;
                        SessionStatus.Text = session.Session.Status.ToString();

                        ConnectedSockets.Items.Clear();
                        foreach (var socket in session.SocketList)
                        {
                            ConnectedSockets.Items.Add(socket);
                        }

                        MainUI.Visibility = Windows.UI.Xaml.Visibility.Visible;
                        PlaceholderUI.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                    }
                    else
                    {
                        AdvertisementId.Text = "##";
                        ServiceAddress.Text = "XX:XX:XX:XX:XX:XX";
                        SessionId.Text = "##";
                        SessionAddress.Text = "XX:XX:XX:XX:XX:XX";
                        SessionStatus.Text = "???";

                        ConnectedSockets.Items.Clear();
                    }
                }
                WiFiDirectServiceManager.Instance.Scenario5 = this;
            }

            protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
            {
                WiFiDirectServiceManager.Instance.Scenario5 = null;
            }

            public async void AddSocket(SocketWrapper socket)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    lock (thisLock)
                    {
                        ConnectedSockets.Items.Add(socket);
                    }
                });
            }

            public async void RemoveSocket(int index)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    lock (thisLock)
                    {
                        if (index > ConnectedSockets.Items.Count - 1)
                        {
                            throw new IndexOutOfRangeException("Attempted to remove socket no longer in list");
                        }

                        ConnectedSockets.Items.RemoveAt(index);
                    }
                });
            }

            private void AddPort_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
            {
                if (session != null)
                {
                    ushort port = 0;
                    if (ushort.TryParse(Port.Text, out port))
                    {
                        // Create socket asynchronously, will get callback when it is complete to add the socket to the list
                        if (((ComboBoxItem)Protocol.SelectedItem).Content.ToString() == "TCP")
                        {
                            session.AddStreamSocketListenerAsync(port);
                        }
                        else
                        {
                            session.AddDatagramSocketAsync(port);
                        }
                    }
                }
            }

            private async void DoSend_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
            {
                if (ConnectedSockets.Items.Count > 0 && session != null)
                {
                    int index = ConnectedSockets.SelectedIndex;
                    if (index >= 0)
                    {
                        string message = SendData.Text;
                        SendData.Text = "";

                        await session.SocketList[index].SendMessageAsync(message);
                    }
                }
            }
        }
    }
}
