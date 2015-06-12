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
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        /// <summary>
        /// UI that shows connected Wi-Fi Direct service sessions and allows a user to choose one to close or interact with
        ///
        /// Most of the actual logic for Wi-Fi Direct Services is in WiFiDirectServicesManager.cs and WiFiDirectServicesWrappers.cs
        /// </summary>
        public sealed partial class Scenario4 : Page
        {
            private Object thisLock = new Object();

            private MainPage rootPage;

            public Scenario4()
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

                    // Cleanup old entries
                    ConnectedSessions.Items.Clear();
                    foreach (var session in WiFiDirectServiceManager.Instance.ConnectedSessionList)
                    {
                        ConnectedSessions.Items.Add(session);
                    }

                    if (ConnectedSessions.Items.Count > 0)
                    {
                        MainUI.Visibility = Windows.UI.Xaml.Visibility.Visible;
                        PlaceholderUI.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                    }
                }
                WiFiDirectServiceManager.Instance.Scenario4 = this;
            }

            protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
            {
                WiFiDirectServiceManager.Instance.Scenario4 = null;
            }

            public async void AddSession(SessionWrapper session)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    lock (thisLock)
                    {
                        ConnectedSessions.Items.Add(session);
                    }
                });
            }

            public async void RemoveSession(int index)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    lock (thisLock)
                    {
                        if (index > ConnectedSessions.Items.Count - 1)
                        {
                            throw new IndexOutOfRangeException("Attempted to remove session no longer in list");
                        }

                        ConnectedSessions.Items.RemoveAt(index);
                    }
                });
            }

            private void SelectSession_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
            {
                if (ConnectedSessions.Items.Count > 0)
                {
                    int index = ConnectedSessions.SelectedIndex;
                    if (index >= 0)
                    {
                        WiFiDirectServiceManager.Instance.SelectSession(index);

                        // Force navigation to the next page for smoother experience
                        // This takes us to the scenario to send and receive data for a selected session
                        rootPage.GoToScenario(4);
                    }
                }
            }

            private void CloseSession_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
            {
                if (ConnectedSessions.Items.Count > 0)
                {
                    int index = ConnectedSessions.SelectedIndex;
                    if (index >= 0)
                    {
                        WiFiDirectServiceManager.Instance.CloseSession(index);
                    }
                }
            }
        }
    }
}
