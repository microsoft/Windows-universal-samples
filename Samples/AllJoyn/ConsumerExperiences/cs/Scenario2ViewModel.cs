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
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Input;
using Windows.Devices.AllJoyn;
using Windows.Devices.Enumeration;
using Windows.Devices.WiFi;
using Windows.Networking.Connectivity;
using Windows.Security.Credentials;
using Windows.Security.Cryptography;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace AllJoynConsumerExperiences
{
    class Scenario2ViewModel : INotifyPropertyChanged
    {
        private MainPage m_rootPage;
        private CoreDispatcher m_dispatcher;
        private WiFiAdapter m_wiFiAdapter = null;
        private IReadOnlyList<WiFiAdapter> m_wiFiAdapterList;
        private ObservableCollection<string> m_wiFiAdapterDisplayNames;
        private ObservableCollection<WiFiAvailableNetwork> m_onboarderNetworkList;
        private ObservableCollection<OnboardingScanListItem> m_onboardeeNetworkList;
        private WiFiAvailableNetwork m_selectedSoftAPNetwork;
        private WiFiAvailableNetwork m_selectedOnboarderNetwork;
        private OnboardingScanListItem m_selectedOnboardeeNetwork;
        private Visibility m_passwordVisibility = Visibility.Collapsed;
        private Visibility m_onboardeeSsidListVisibility = Visibility.Visible;
        private Visibility m_onboarderSsidListVisibility = Visibility.Collapsed;
        private Visibility m_manualSsidTextBoxVisibility = Visibility.Collapsed;
        private Visibility m_connectPanelVisibility = Visibility.Collapsed;
        private Visibility m_onboardingConfigurationVisibility = Visibility.Collapsed;
        private Visibility m_scanPanelVisibility = Visibility.Collapsed;
        private Visibility m_wiFiAdapterListVisibility = Visibility.Collapsed;
        private Visibility m_authenticationVisibility = Visibility.Collapsed;
        private int m_selectedWiFiAdapterIndex = -1;
        private string m_savedProfileName;
        private string m_softAPPassword = "";
        private string m_onboardingPassword = "";
        private string m_manualSsid = "";
        private string m_key = "";
        private bool? m_manualSsidIsChecked = null;
        private bool? m_onboardingProducerIsChecked = null;
        private bool? m_physicalDeviceIsChecked = null;
        private bool m_showOnboardeeSsidList = true;
        private bool m_showOnboarderSsidList = false;
        private AllJoynBusAttachment m_busAttachment = null;
        private DeviceWatcher m_watcher = null;
        private OnboardingConsumer m_consumer = null;
        private OnboardingAuthenticationType m_selectedAuthType = OnboardingAuthenticationType.Any;
        private TaskCompletionSource<bool> m_authenticateClicked = null;
        private int m_onboardSessionAlreadyJoined;

        public Scenario2ViewModel()
        {
            m_dispatcher = CoreWindow.GetForCurrentThread().Dispatcher;
            m_rootPage = MainPage.Current;
            m_wiFiAdapterDisplayNames = new ObservableCollection<string>();
            m_onboarderNetworkList = new ObservableCollection<WiFiAvailableNetwork>();
            m_onboardeeNetworkList = new ObservableCollection<OnboardingScanListItem>();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public ObservableCollection<string> WiFiAdapterDisplayNames
        {
            get
            {
                return m_wiFiAdapterDisplayNames;
            }
        }

        public ObservableCollection<WiFiAvailableNetwork> OnboarderNetworkList
        {
            get
            {
                return m_onboarderNetworkList;
            }
        }

        public ObservableCollection<OnboardingScanListItem> OnboardeeNetworkList
        {
            get
            {
                return m_onboardeeNetworkList;
            }
        }

        public int SelectedWiFiAdapterIndex
        {
            get
            {
                return m_selectedWiFiAdapterIndex;
            }
            set
            {
                m_selectedWiFiAdapterIndex = value;
                ScanForWiFiNetworksAsync();
            }
        }

        public WiFiAvailableNetwork SelectedSoftAPNetwork
        {
            get
            {
                return m_selectedSoftAPNetwork;
            }
            set
            {
                m_selectedSoftAPNetwork = value;
                if (m_selectedSoftAPNetwork != null)
                {
                    if (m_selectedSoftAPNetwork.SecuritySettings.NetworkAuthenticationType == NetworkAuthenticationType.Open80211)
                    {
                        PasswordVisibility = Visibility.Collapsed;
                    }
                    else
                    {
                        PasswordVisibility = Visibility.Visible;
                    }
                }
                else
                {
                    PasswordVisibility = Visibility.Collapsed;
                }
                ClearPasswords();
            }
        }

        public WiFiAvailableNetwork SelectedOnboarderNetwork
        {
            get
            {
                return m_selectedOnboarderNetwork;
            }
            set
            {
                m_selectedOnboarderNetwork = value;
            }
        }

        public OnboardingScanListItem SelectedOnboardeeNetwork
        {
            get
            {
                return m_selectedOnboardeeNetwork;
            }
            set
            {
                m_selectedOnboardeeNetwork = value;
                if (m_selectedOnboardeeNetwork != null)
                {
                    SelectedAuthType = (OnboardingAuthenticationType)m_selectedOnboardeeNetwork.Value2;
                }
            }
        }

        public Visibility PasswordVisibility
        {
            get
            {
                return m_passwordVisibility;
            }
            private set
            {
                m_passwordVisibility = value;
                RaisePropertyChangedEventAsync();
            }
        }

        public Visibility OnboardeeSsidListVisibility
        {
            get
            {
                return m_onboardeeSsidListVisibility;
            }
            private set
            {
                m_onboardeeSsidListVisibility = value;
                if (m_manualSsidIsChecked != true)
                {
                    switch (m_onboardeeSsidListVisibility)
                    {
                        case Visibility.Visible:
                            m_showOnboardeeSsidList = true;
                            break;
                        case Visibility.Collapsed:
                            m_showOnboardeeSsidList = false;
                            break;
                        default:
                            break;
                    }
                }
                RaisePropertyChangedEventAsync();
            }
        }

        public Visibility OnboarderSsidListVisibility
        {
            get
            {
                return m_onboarderSsidListVisibility;
            }
            private set
            {
                m_onboarderSsidListVisibility = value;
                if (m_manualSsidIsChecked != true)
                {
                    switch (m_onboarderSsidListVisibility)
                    {
                        case Visibility.Visible:
                            m_showOnboarderSsidList = true;
                            break;
                        case Visibility.Collapsed:
                            m_showOnboarderSsidList = false;
                            break;
                        default:
                            break;
                    }
                }
                RaisePropertyChangedEventAsync();
            }
        }

        public Visibility ManualSsidTextBoxVisibility
        {
            get
            {
                return m_manualSsidTextBoxVisibility;
            }
            private set
            {
                m_manualSsidTextBoxVisibility = value;
                RaisePropertyChangedEventAsync();
            }
        }

        public Visibility ConnectPanelVisibility
        {
            get
            {
                return m_connectPanelVisibility;
            }
            private set
            {
                m_connectPanelVisibility = value;
                RaisePropertyChangedEventAsync();
            }
        }

        public Visibility OnboardingConfigurationVisibility
        {
            get
            {
                return m_onboardingConfigurationVisibility;
            }
            private set
            {
                m_onboardingConfigurationVisibility = value;
                RaisePropertyChangedEventAsync();
            }
        }

        public Visibility ScanPanelVisibility
        {
            get
            {
                return m_scanPanelVisibility;
            }
            private set
            {
                m_scanPanelVisibility = value;
                RaisePropertyChangedEventAsync();
            }
        }

        public Visibility WiFiAdapterListVisibility
        {
            get
            {
                return m_wiFiAdapterListVisibility;
            }
            private set
            {
                m_wiFiAdapterListVisibility = value;
                RaisePropertyChangedEventAsync();
            }
        }

        public Visibility AuthenticationVisibility
        {
            get
            {
                return m_authenticationVisibility;
            }
            private set
            {
                m_authenticationVisibility = value;
                RaisePropertyChangedEventAsync();
            }
        }

        public bool? ManualSsidIsChecked
        {
            get
            {
                return m_manualSsidIsChecked;
            }
            set
            {
                m_manualSsidIsChecked = value;
                if (m_manualSsidIsChecked == true)
                {
                    OnboardeeSsidListVisibility = Visibility.Collapsed;
                    OnboarderSsidListVisibility = Visibility.Collapsed;
                    ManualSsidTextBoxVisibility = Visibility.Visible;
                }
                else
                {
                    if (m_showOnboardeeSsidList)
                    {
                        OnboardeeSsidListVisibility = Visibility.Visible;
                    }
                    else
                    {
                        OnboarderSsidListVisibility = Visibility.Visible;
                    }
                    ManualSsidTextBoxVisibility = Visibility.Collapsed;
                }
            }
        }

        public bool? OnboardingProducerIsChecked
        {
            get
            {
                return m_onboardingProducerIsChecked;
            }
            set
            {
                m_onboardingProducerIsChecked = value;
                if (m_onboardingProducerIsChecked == true)
                {
                    ResetControls();
                    ScenarioCleanup();
                }
            }
        }

        public bool? PhysicalDeviceIsChecked
        {
            get
            {
                return m_physicalDeviceIsChecked;
            }
            set
            {
                m_physicalDeviceIsChecked = value;
                if (m_physicalDeviceIsChecked == true)
                {
                    ResetControls();
                    ScenarioCleanup();
                }
            }
        }

        public string SoftAPPassword
        {
            get
            {
                return m_softAPPassword;
            }
            set
            {
                if (value != m_softAPPassword)
                {
                    m_softAPPassword = value;
                    RaisePropertyChangedEventAsync();
                }
            }
        }

        public string OnboardingPassword
        {
            get
            {
                return m_onboardingPassword;
            }
            set
            {
                if (value != m_onboardingPassword)
                {
                    m_onboardingPassword = value;
                    RaisePropertyChangedEventAsync();
                }
            }
        }

        public string ManualSsid
        {
            get
            {
                return m_manualSsid;
            }
            set
            {
                m_manualSsid = value;
            }
        }

        public string EnteredKey
        {
            get
            {
                return m_key;
            }
            set
            {
                if (value != m_key)
                {
                    m_key = value;
                    RaisePropertyChangedEventAsync();
                }
            }
        }

        public string[] AuthTypeArray
        {
            get
            {
                return Enum.GetNames(typeof(OnboardingAuthenticationType));
            }
        }

        public OnboardingAuthenticationType SelectedAuthType
        {
            get
            {
                return m_selectedAuthType;
            }
            set
            {
                if (value != m_selectedAuthType)
                {
                    m_selectedAuthType = value;
                    RaisePropertyChangedEventAsync();
                }
            }
        }

        public ICommand StartScan
        {
            get
            {
                return new RelayCommand((object args) =>
                {
                    StartScanning();
                });
            }
        }

        public ICommand Connect
        {
            get
            {
                return new RelayCommand((object args) =>
                {
                    ConnectToSoftAPAsync();
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

        public ICommand Onboard
        {
            get
            {
                return new RelayCommand((object args) =>
                {
                    GetOnboardingConfiguration();
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

        private void StartScanning()
        {
            if (m_onboardingProducerIsChecked == true)
            {
                ScanForOnboardingInterfaces();
            }
            else if (m_physicalDeviceIsChecked == true)
            {
                ScanForWiFiAdaptersAsync();
            }
        }

        private async void ScanForWiFiAdaptersAsync()
        {
            if (m_wiFiAdapter == null)
            {
                UpdateStatusAsync("Requesting WiFi access...", NotifyType.StatusMessage);
                WiFiAccessStatus accessStatus = await WiFiAdapter.RequestAccessAsync();
                if (accessStatus != WiFiAccessStatus.Allowed)
                {
                    UpdateStatusAsync("WiFi access denied.", NotifyType.ErrorMessage);
                }
                else
                {
                    m_wiFiAdapterList = await WiFiAdapter.FindAllAdaptersAsync();
                    if (m_wiFiAdapterList.Count > 0)
                    {
                        for (int i = 0; i < m_wiFiAdapterList.Count; i++)
                        {
                            m_wiFiAdapterDisplayNames.Add(string.Format("Adapter {0}", (i + 1)));
                        }
                        WiFiAdapterListVisibility = Visibility.Visible;
                        UpdateStatusAsync("Please select a WiFi adapter.", NotifyType.StatusMessage);
                    }
                    else
                    {
                        UpdateStatusAsync("No WiFi adapters detected on this machine.", NotifyType.ErrorMessage);
                    }
                }
            }
        }

        private async void ScanForWiFiNetworksAsync()
        {
            if (m_selectedWiFiAdapterIndex >= 0)
            {
                m_wiFiAdapter = m_wiFiAdapterList[m_selectedWiFiAdapterIndex];
            }

            if (m_wiFiAdapter != null)
            {
                UpdateConnectivityStatusAsync();
                NetworkInformation.NetworkStatusChanged += NetworkInformation_NetworkStatusChanged;
                UpdateStatusAsync("Scanning for available WiFi networks...", NotifyType.StatusMessage);
                await m_wiFiAdapter.ScanAsync();
                m_onboarderNetworkList.Clear();
                if (m_wiFiAdapter.NetworkReport.AvailableNetworks.Count > 0)
                {
                    foreach (WiFiAvailableNetwork network in m_wiFiAdapter.NetworkReport.AvailableNetworks)
                    {
                        m_onboarderNetworkList.Add(network);
                    }
                    ConnectPanelVisibility = Visibility.Visible;
                    UpdateStatusAsync("Scan complete. Please select a network to connect.", NotifyType.StatusMessage);
                }
                else
                {
                    UpdateStatusAsync("Scan complete. No WiFi networks found.", NotifyType.ErrorMessage);
                }
            }
        }

        private async void UpdateConnectivityStatusAsync()
        {
            if (m_wiFiAdapter != null)
            {
                ConnectionProfile connectedProfile = await m_wiFiAdapter.NetworkAdapter.GetConnectedProfileAsync();
                if ((connectedProfile != null) && (!connectedProfile.ProfileName.Equals(m_savedProfileName)))
                {
                    m_savedProfileName = connectedProfile.ProfileName;
                }
                else if ((connectedProfile == null) && (m_savedProfileName != null))
                {
                    UpdateStatusAsync("WiFi adapter disconnected from: " + m_savedProfileName, NotifyType.StatusMessage);
                    m_savedProfileName = null;
                }
            }
        }

        private void NetworkInformation_NetworkStatusChanged(object sender)
        {
            UpdateConnectivityStatusAsync();
        }

        private async void ConnectToSoftAPAsync()
        {
            if (m_wiFiAdapter != null)
            {
                if (m_selectedSoftAPNetwork == null)
                {
                    UpdateStatusAsync("Network not selected. Please select a network.", NotifyType.ErrorMessage);
                }
                else
                {
                    UpdateStatusAsync(string.Format("Attempting to connect to {0}...", m_selectedSoftAPNetwork.Ssid), NotifyType.StatusMessage);
                    WiFiConnectionResult result = null;
                    if (m_selectedSoftAPNetwork.SecuritySettings.NetworkAuthenticationType == NetworkAuthenticationType.Open80211)
                    {
                        result = await m_wiFiAdapter.ConnectAsync(m_selectedSoftAPNetwork, WiFiReconnectionKind.Manual);
                    }
                    else
                    {
                        if (string.IsNullOrWhiteSpace(m_softAPPassword))
                        {
                            UpdateStatusAsync("No password entered.", NotifyType.ErrorMessage);
                        }
                        else
                        {
                            PasswordCredential credential = new PasswordCredential();
                            credential.Password = m_softAPPassword;
                            result = await m_wiFiAdapter.ConnectAsync(m_selectedSoftAPNetwork, WiFiReconnectionKind.Manual, credential);
                        }
                        ClearPasswords();
                    }

                    if (result != null)
                    {
                        if (result.ConnectionStatus == WiFiConnectionStatus.Success)
                        {
                            UpdateStatusAsync(string.Format("Successfully connected to {0}.", m_selectedSoftAPNetwork.Ssid), NotifyType.StatusMessage);
                            ScanForOnboardingInterfaces();
                        }
                        else
                        {
                            UpdateStatusAsync(string.Format("Failed to connect to {0} with error: {1}.", m_selectedSoftAPNetwork.Ssid, result.ConnectionStatus), NotifyType.ErrorMessage);
                        }
                    }
                }
            }
        }

        private void ScanForOnboardingInterfaces()
        {
            ScenarioCleanup();

            // Allow re-joining of a new session
            Interlocked.Exchange(ref m_onboardSessionAlreadyJoined, 0);

            m_busAttachment = new AllJoynBusAttachment();
            m_busAttachment.StateChanged += BusAttachment_StateChanged;
            m_busAttachment.AuthenticationMechanisms.Clear();

            // EcdhePsk authentication is deprecated as of the AllJoyn 16.04 release.
            // Newly added EcdheSpeke should be used instead. EcdhePsk authentication is
            // added here to maintain compatibility with devices running older AllJoyn versions.
            m_busAttachment.AuthenticationMechanisms.Add(AllJoynAuthenticationMechanism.EcdheNull);
            m_busAttachment.AuthenticationMechanisms.Add(AllJoynAuthenticationMechanism.EcdhePsk);
            m_busAttachment.AuthenticationMechanisms.Add(AllJoynAuthenticationMechanism.EcdheSpeke);
            m_busAttachment.AuthenticationComplete += BusAttachment_AuthenticationComplete;
            m_busAttachment.CredentialsRequested += BusAttachment_CredentialsRequested;
            m_watcher = AllJoynBusAttachment.GetWatcher(new List<string> { "org.alljoyn.Onboarding" });
            m_watcher.Added += Watcher_Added;
            UpdateStatusAsync("Searching for onboarding interface...", NotifyType.StatusMessage);
            m_watcher.Start();
        }

        private void BusAttachment_StateChanged(AllJoynBusAttachment sender, AllJoynBusAttachmentStateChangedEventArgs args)
        {
            if (args.State == AllJoynBusAttachmentState.Disconnected)
            {
                UpdateStatusAsync(string.Format("AllJoyn bus attachment has disconnected with AllJoyn error: 0x{0:X}.", args.Status), NotifyType.StatusMessage);
                ResetControls();
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

            EnteredKey = "";
            AuthenticationVisibility = Visibility.Collapsed;
        }

        private async void BusAttachment_CredentialsRequested(AllJoynBusAttachment sender, AllJoynCredentialsRequestedEventArgs args)
        {
            Windows.Foundation.Deferral credentialsDeferral = args.GetDeferral();

            if ((args.Credentials.AuthenticationMechanism == AllJoynAuthenticationMechanism.EcdhePsk) ||
                (args.Credentials.AuthenticationMechanism == AllJoynAuthenticationMechanism.EcdheSpeke))
            {
                m_authenticateClicked = new TaskCompletionSource<bool>();
                AuthenticationVisibility = Visibility.Visible;

                UpdateStatusAsync("Please enter the key.", NotifyType.StatusMessage);
                await m_authenticateClicked.Task;
                m_authenticateClicked = null;

                if (!string.IsNullOrEmpty(m_key))
                {
                    args.Credentials.PasswordCredential.Password = m_key;
                }
                else
                {
                    UpdateStatusAsync("Please enter a key.", NotifyType.ErrorMessage);
                }
            }
            else if (args.Credentials.AuthenticationMechanism == AllJoynAuthenticationMechanism.EcdheNull)
            {
            }
            else
            {
                UpdateStatusAsync("Unexpected authentication mechanism.", NotifyType.ErrorMessage);
            }

            credentialsDeferral.Complete();
        }

        private async void Watcher_Added(DeviceWatcher sender, DeviceInformation args)
        {
            // This demo supports a single onboarding producer, if there are multiple onboarding producers found, then they are ignored.
            // Another approach would be to create a list of all producers found and then allow the user to choose the one they want
            bool bAlreadyJoined = (Interlocked.CompareExchange(ref m_onboardSessionAlreadyJoined, 1, 0) == 1);
            if (bAlreadyJoined)
            {
                return;
            }

            DisposeConsumer();
            UpdateStatusAsync("Joining session...", NotifyType.StatusMessage);
            m_consumer = await OnboardingConsumer.FromIdAsync(args.Id, m_busAttachment);

            if (m_consumer != null)
            {
                m_consumer.Session.Lost += Consumer_SessionLost;
                GetOnboardeeNetworkListAsync();
            }
            else
            {
                UpdateStatusAsync("Attempt to join session failed.", NotifyType.ErrorMessage);
            }
        }

        private void Consumer_SessionLost(AllJoynSession sender, AllJoynSessionLostEventArgs args)
        {
            UpdateStatusAsync(string.Format("AllJoyn session with the onboardee lost due to {0}.", args.Reason.ToString()), NotifyType.StatusMessage);
            DisposeConsumer();
            ResetControls();
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

        private async void GetOnboardeeNetworkListAsync()
        {
            UpdateStatusAsync("Requesting network list from the onboardee...", NotifyType.StatusMessage);
            OnboardingGetScanInfoResult getScanResult = await m_consumer.GetScanInfoAsync();
            if (getScanResult.Status == AllJoynStatus.Ok)
            {
                if (getScanResult.ScanList.Count > 0)
                {
                    await m_dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        foreach (OnboardingScanListItem item in getScanResult.ScanList)
                        {
                            m_onboardeeNetworkList.Add(item);
                        }
                    });

                    ShowOnboardeeSsidList();
                    UpdateStatusAsync(string.Format("{0} network(s) returned. Scan age: {1} minutes.", getScanResult.ScanList.Count, getScanResult.Age), NotifyType.StatusMessage);
                }
                else
                {
                    // No networks returned by the onboardee. Provide the user option to configure onboardee with networks visible from onboarder.
                    UpdateStatusAsync("Please select a network or enter one manually.", NotifyType.StatusMessage);
                    ShowOnboarderSsidList();
                }
            }
            else
            {
                // Unable to get scan information from the onboardee. Provide the user option to configure onboardee with networks visible from onboarder.
                UpdateStatusAsync("Please select a network or enter one manually.", NotifyType.StatusMessage);
                ShowOnboarderSsidList();
            }
            OnboardingConfigurationVisibility = Visibility.Visible;
        }

        private void GetOnboardingConfiguration()
        {
            string configureSsid = null;

            if (m_manualSsidIsChecked == true)
            {
                if (!string.IsNullOrWhiteSpace(m_manualSsid))
                {
                    configureSsid = m_manualSsid;
                }
            }
            else
            {
                if (m_showOnboardeeSsidList)
                {
                    if (m_selectedOnboardeeNetwork != null)
                    {
                        configureSsid = m_selectedOnboardeeNetwork.Value1;
                    }
                }
                else
                {
                    if (m_selectedOnboarderNetwork != null)
                    {
                        configureSsid = m_selectedOnboarderNetwork.Ssid;
                    }
                }
            }

            AttemptOnboardingAsync(configureSsid, m_onboardingPassword, (short)m_selectedAuthType);
        }

        private async void AttemptOnboardingAsync(string ssid, string password, short authType)
        {
            if (string.IsNullOrWhiteSpace(ssid))
            {
                UpdateStatusAsync("No SSID selected. Please select a network or manually enter SSID.", NotifyType.ErrorMessage);
            }
            else
            {
                UpdateStatusAsync("Attempting to configure onboardee...", NotifyType.StatusMessage);

                OnboardingConfigureWiFiResult configureWifiResult = await m_consumer.ConfigureWiFiAsync(ssid, ConvertUtf8ToHex(password), authType);
                if (configureWifiResult.Status == AllJoynStatus.Ok)
                {
                    UpdateStatusAsync("Onboardee sucessfully configured.", NotifyType.StatusMessage);
                    if (configureWifiResult.Status2 == (short)ConfigureWiFiResultStatus.Concurrent)
                    {
                        // The Onboardee has indicated that it will attempt to connect to the desired AP concurrently, while the SoftAP is enabled. In this case, 
                        // the Onboarder application must wait for the ConnectionResult signal to arrive over the AllJoyn session established over the SoftAP link.
                        // If the Onboardee does not connect to the desired AP concurrently, then there is no guaranteed way for the Onboarder application to find
                        // out if the connection attempt was successful or not. In the NotConcurrent connection attempt, if the Onboardee fails to connect to
                        // the desired AP, the Onboarder application will have to again start over with scanning and connecting to the Onboardee SoftAP.
                        // For more information please visit http://go.microsoft.com/fwlink/?LinkId=817239
                        m_consumer.Signals.ConnectionResultReceived += Signals_ConnectionResultReceived;
                    }
                    AttemptConnectionAsync();
                }
                else
                {
                    UpdateStatusAsync(string.Format("Attempt to configure WiFi failed with AllJoyn error: 0x{0:X}.", configureWifiResult.Status), NotifyType.ErrorMessage);
                }
            }
            ClearPasswords();
        }

        private static string ConvertUtf8ToHex(string inputString)
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

        private async void AttemptConnectionAsync()
        {
            OnboardingConnectResult connectResult = await m_consumer.ConnectAsync();
            UpdateStatusAsync("Connect request sent.", NotifyType.StatusMessage);
        }

        private void Signals_ConnectionResultReceived(OnboardingSignals sender, OnboardingConnectionResultReceivedEventArgs args)
        {
            if (args.Arg.Value1 == (short)ConnectionResultCode.Validated)
            {
                UpdateStatusAsync("Onboarding successful.", NotifyType.StatusMessage);
            }
            else
            {
                UpdateStatusAsync(string.Format("Connection attempt failed with result code: {0} and message: {1}.", ((ConnectionResultCode)args.Arg.Value1).ToString(), args.Arg.Value2), NotifyType.ErrorMessage);
            }
        }

        private void ClearPasswords()
        {
            SoftAPPassword = "";
            OnboardingPassword = "";
        }

        private void ShowOnboardeeSsidList()
        {
            if (m_manualSsidIsChecked == true)
            {
                m_showOnboardeeSsidList = true;
                m_showOnboarderSsidList = false;
            }
            else
            {
                OnboardeeSsidListVisibility = Visibility.Visible;
                OnboarderSsidListVisibility = Visibility.Collapsed;
            }
        }

        private void ShowOnboarderSsidList()
        {
            if (m_manualSsidIsChecked == true)
            {
                m_showOnboardeeSsidList = false;
                m_showOnboarderSsidList = true;
            }
            else
            {
                OnboardeeSsidListVisibility = Visibility.Collapsed;
                OnboarderSsidListVisibility = Visibility.Visible;
            }
        }

        private void ResetControls()
        {
            ScanPanelVisibility = Visibility.Visible;
            WiFiAdapterListVisibility = Visibility.Collapsed;
            ConnectPanelVisibility = Visibility.Collapsed;
            AuthenticationVisibility = Visibility.Collapsed;
            OnboardingConfigurationVisibility = Visibility.Collapsed;
        }

        private void DisposeConsumer()
        {
            if (m_consumer != null)
            {
                m_consumer.Session.Lost -= Consumer_SessionLost;
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
