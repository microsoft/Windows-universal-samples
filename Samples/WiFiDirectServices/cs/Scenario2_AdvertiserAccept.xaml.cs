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
using Windows.Devices.WiFiDirect.Services;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        /// <summary>
        /// Helper class to display session requests in list with extra data
        /// </summary>
        public class SessionRequestWrapper
        {
            public SessionRequestWrapper(WiFiDirectServiceSessionRequest request, AdvertisementWrapper advertisement)
            {
                this.Id = request.DeviceInformation.Id;
                this.Advertisement = advertisement;
                this.Pin = "";
                this.ProvisioningInfo = "GroupFormation? " + (request.ProvisioningInfo.IsGroupFormationNeeded ? "Yes" : "No")
                    + ", ConfigMethod: " + request.ProvisioningInfo.SelectedConfigurationMethod.ToString();

                if (request.ProvisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinDisplay)
                {
                    this.Pin = advertisement.Pin;
                }

                this.SessionInfo = request.SessionInfo;
            }

            public string Id { get; set; }
            public string ProvisioningInfo { get; set; }
            public string Pin { get; set; }
            public IBuffer SessionInfo { get; set; }

            public AdvertisementWrapper Advertisement { get; set; }
        }

        /// <summary>
        /// UI exposing an interface to accept/decline incoming connection requests as an advertiser
        /// and to interact with connected sessions
        ///
        /// Most of the actual logic for Wi-Fi Direct Services is in WiFiDirectServicesManager.cs and WiFiDirectServicesWrappers.cs
        /// </summary>
        public sealed partial class Scenario2 : Page
        {
            private Object thisLock = new Object();

            private MainPage rootPage;

            public Scenario2()
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

                    // Fill in all existing data
                    if (WiFiDirectServiceManager.Instance.Advertisers.Count > 0)
                    {
                        // Session Requests
                        SessionRequests.Items.Clear();

                        // Advertisers
                        Advertisements.Items.Clear();
                        foreach (var advertiser in WiFiDirectServiceManager.Instance.Advertisers)
                        {
                            Advertisements.Items.Add(advertiser);

                            foreach (var request in advertiser.SessionRequestList)
                            {
                                SessionRequestWrapper sessionRequest = new SessionRequestWrapper(request, advertiser);
                                SessionRequests.Items.Add(sessionRequest);
                            }
                        }
                        
                        // Connected Sessions
                        ConnectedSessions.Items.Clear();
                        foreach (var session in WiFiDirectServiceManager.Instance.ConnectedSessionList)
                        {
                            ConnectedSessions.Items.Add(session);
                        }

                        MainUI.Visibility = Windows.UI.Xaml.Visibility.Visible;
                        PlaceholderUI.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                    }
                }
                WiFiDirectServiceManager.Instance.Scenario2 = this;
            }

            protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
            {
                WiFiDirectServiceManager.Instance.Scenario2 = null;
            }

            public async void AddSessionRequest(WiFiDirectServiceSessionRequest request, AdvertisementWrapper advertiser)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    lock (thisLock)
                    {
                        SessionRequestWrapper sessionRequest = new SessionRequestWrapper(request, advertiser);
                        SessionRequests.Items.Add(sessionRequest);
                    }
                });
            }

            public async void RemoveSessionRequest(WiFiDirectServiceSessionRequest request, AdvertisementWrapper advertiser)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    lock (thisLock)
                    {
                        foreach (SessionRequestWrapper r in SessionRequests.Items)
                        {
                            if (r.Advertisement.InternalId == advertiser.InternalId &&
                                r.Id == request.DeviceInformation.Id)
                            {
                                SessionRequests.Items.Remove(r);
                                break;
                            }
                        }
                    }
                });
            }

            public async void AddAdvertiser(AdvertisementWrapper advertiser)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    lock (thisLock)
                    {
                        Advertisements.Items.Add(advertiser);
                    }
                });
            }

            public async void RemoveAdvertiser(int index)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    lock (thisLock)
                    {
                        if (index > Advertisements.Items.Count - 1)
                        {
                            throw new IndexOutOfRangeException("Attempted to remove advertiser no longer in list");
                        }

                        Advertisements.Items.RemoveAt(index);
                        
                        if (Advertisements.Items.Count == 0)
                        {
                            // Force navigation to the previous page for smoother experience
                            // This takes us to the scenario to create a new advertisement
                            rootPage.GoToScenario(0);
                        }
                    }
                });
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

            public void UpdateAdvertiserStatus(AdvertisementWrapper advertiser)
            {
                // Update list
            }

            private void Accept_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
            {
                if (SessionRequests.Items.Count > 0)
                {
                    int requestIndex = SessionRequests.SelectedIndex;
                    if (requestIndex >= 0)
                    {
                        SessionRequestWrapper request = SessionRequests.Items[requestIndex] as SessionRequestWrapper;
                        request.Advertisement.AcceptSessionRequest(request.Id, AcceptPin.Text);
                    }
                }
            }

            private void Decline_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
            {
                if (SessionRequests.Items.Count > 0)
                {
                    int requestIndex = SessionRequests.SelectedIndex;
                    if (requestIndex >= 0)
                    {
                        SessionRequestWrapper request = SessionRequests.Items[requestIndex] as SessionRequestWrapper;
                        request.Advertisement.DeclineSessionRequest(request.Id);
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

            private void StopAdvertisement_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
            {
                if (Advertisements.Items.Count > 0)
                {
                    int index = Advertisements.SelectedIndex;
                    if (index >= 0)
                    {
                        WiFiDirectServiceManager.Instance.UnpublishService(index);
                    }
                }
            }
        }
    }
}
