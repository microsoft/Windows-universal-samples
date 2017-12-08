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
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.Networking;
using Windows.System.RemoteSystems;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Discovery : Page
    {
        private MainPage m_rootPage;
        private RemoteSystemWatcher m_remoteSystemWatcher;

        public Scenario1_Discovery()
        {
            this.InitializeComponent();
            this.DataContext = this;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            m_rootPage = MainPage.Current;
            m_rootPage.systemList = new ObservableCollection<RemoteSystem>();
            m_rootPage.systemMap = new Dictionary<string, RemoteSystem>();
        }

        public ObservableCollection<RemoteSystem> SystemList => m_rootPage.systemList;

        private async void Search_Clicked(object sender, RoutedEventArgs e)
        {
            // Disable the Search button while watcher is being started to avoid a potential
            // race condition of having two RemoteSystemWatchers running in parallel. 
            Button searchButton = sender as Button;
            searchButton.IsHitTestVisible = false;

            // Cleaning up any existing systems from previous searches.
            SearchCleanup();

            // Verify access for Remote Systems. 
            // Note: RequestAccessAsync needs to called from the UI thread.
            RemoteSystemAccessStatus accessStatus = await RemoteSystem.RequestAccessAsync();

            if (accessStatus == RemoteSystemAccessStatus.Allowed)
            {
                if (HostNameSearch.IsChecked.Value)
                {
                    await SearchByHostNameAsync();
                }
                else
                {
                    SearchByRemoteSystemWatcher();
                }
            }
            else
            {
                UpdateStatus("Access to Remote Systems is " + accessStatus.ToString(), NotifyType.ErrorMessage);
            }

            searchButton.IsHitTestVisible = true;
        }

        private async Task SearchByHostNameAsync()
        {
            if (!string.IsNullOrWhiteSpace(HostNameTextBox.Text))
            {
                // Build hostname object.
                HostName hostName = new HostName(HostNameTextBox.Text);

                // Get Remote System from HostName.
                RemoteSystem remoteSystem = await RemoteSystem.FindByHostNameAsync(hostName);

                if (remoteSystem != null)
                {
                    m_rootPage.systemList.Add(remoteSystem);
                    SystemListBox.Visibility = Visibility.Visible;
                    UpdateStatus("Found system - " + remoteSystem.DisplayName, NotifyType.StatusMessage);
                }
                else
                {
                    UpdateStatus("Unable to find the system.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                UpdateStatus("Enter a valid host name", NotifyType.ErrorMessage);
            }
        }

        private void SearchByRemoteSystemWatcher()
        {
            if (FilterSearch.IsChecked.Value)
            {
                // Build a watcher to continuously monitor for filtered remote systems.
                m_remoteSystemWatcher = RemoteSystem.CreateWatcher(BuildFilters());
            }
            else
            {
                // Build a watcher to continuously monitor for all remote systems.
                m_remoteSystemWatcher = RemoteSystem.CreateWatcher();
            }

            // Subscribing to the event that will be raised when a new remote system is found by the watcher.
            m_remoteSystemWatcher.RemoteSystemAdded += RemoteSystemWatcher_RemoteSystemAdded;

            // Subscribing to the event that will be raised when a previously found remote system is no longer available.
            m_remoteSystemWatcher.RemoteSystemRemoved += RemoteSystemWatcher_RemoteSystemRemoved;

            // Subscribing to the event that will be raised when a previously found remote system is updated.
            m_remoteSystemWatcher.RemoteSystemUpdated += RemoteSystemWatcher_RemoteSystemUpdated;

            // Start the watcher.
            m_remoteSystemWatcher.Start();

            UpdateStatus("Searching for systems...", NotifyType.StatusMessage);
            SystemListBox.Visibility = Visibility.Visible;
        }

        private async void RemoteSystemWatcher_RemoteSystemUpdated(RemoteSystemWatcher sender, RemoteSystemUpdatedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (m_rootPage.systemMap.ContainsKey(args.RemoteSystem.Id))
                {
                    m_rootPage.systemList.Remove(m_rootPage.systemMap[args.RemoteSystem.Id]);
                    m_rootPage.systemMap.Remove(args.RemoteSystem.Id);
                }
                m_rootPage.systemList.Add(args.RemoteSystem);
                m_rootPage.systemMap.Add(args.RemoteSystem.Id, args.RemoteSystem);
                UpdateStatus("System updated with Id = " + args.RemoteSystem.Id, NotifyType.StatusMessage);
            });
        }

        private async void RemoteSystemWatcher_RemoteSystemRemoved(RemoteSystemWatcher sender, RemoteSystemRemovedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (m_rootPage.systemMap.ContainsKey(args.RemoteSystemId))
                {
                    m_rootPage.systemList.Remove(m_rootPage.systemMap[args.RemoteSystemId]);
                    UpdateStatus(m_rootPage.systemMap[args.RemoteSystemId].DisplayName + " removed.", NotifyType.StatusMessage);
                    m_rootPage.systemMap.Remove(args.RemoteSystemId);
                }
            });
        }

        private async void RemoteSystemWatcher_RemoteSystemAdded(RemoteSystemWatcher sender, RemoteSystemAddedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                m_rootPage.systemList.Add(args.RemoteSystem);
                m_rootPage.systemMap.Add(args.RemoteSystem.Id, args.RemoteSystem);
                UpdateStatus(string.Format("Found {0} systems.", m_rootPage.systemList.Count), NotifyType.StatusMessage);
            });
        }

        private List<IRemoteSystemFilter> BuildFilters()
        {
            List<IRemoteSystemFilter> filters = new List<IRemoteSystemFilter>();
            RemoteSystemDiscoveryTypeFilter discoveryFilter;
            RemoteSystemAuthorizationKindFilter authorizationKindFilter;
            List<string> kinds = new List<string>();
            RemoteSystemStatusTypeFilter statusFilter;

            if (DiscoveryTypeOptions.IsChecked.Value)
            {
                // Build discovery type filters
                if (ProximalRadioButton.IsChecked.Value)
                {
                    discoveryFilter = new RemoteSystemDiscoveryTypeFilter(RemoteSystemDiscoveryType.Proximal);
                }
                else if (CloudRadioButton.IsChecked.Value)
                {
                    discoveryFilter = new RemoteSystemDiscoveryTypeFilter(RemoteSystemDiscoveryType.Cloud);
                }
                else if (SpatiallyProximalRadioButton.IsChecked.Value)
                {
                    discoveryFilter = new RemoteSystemDiscoveryTypeFilter(RemoteSystemDiscoveryType.SpatiallyProximal);
                }
                else
                {
                    discoveryFilter = new RemoteSystemDiscoveryTypeFilter(RemoteSystemDiscoveryType.Any);
                }
                filters.Add(discoveryFilter);
            }

            if (AuthorizationTypeOptions.IsChecked.Value)
            {
                // Build authorization type filters
                if (AnonymousDiscoveryRadioButton.IsChecked.Value)
                {
                    authorizationKindFilter = new RemoteSystemAuthorizationKindFilter(RemoteSystemAuthorizationKind.Anonymous);
                }
                else
                {
                    authorizationKindFilter = new RemoteSystemAuthorizationKindFilter(RemoteSystemAuthorizationKind.SameUser);
                }
                filters.Add(authorizationKindFilter);
            }

            if (SystemTypeOptions.IsChecked.Value)
            {
                // Build system type filters
                if (DesktopCheckBox.IsChecked.Value)
                {
                    kinds.Add(RemoteSystemKinds.Desktop);
                }
                if (HolographicCheckBox.IsChecked.Value)
                {
                    kinds.Add(RemoteSystemKinds.Holographic);
                }
                if (HubCheckBox.IsChecked.Value)
                {
                    kinds.Add(RemoteSystemKinds.Hub);
                }
                if (PhoneCheckBox.IsChecked.Value)
                {
                    kinds.Add(RemoteSystemKinds.Phone);
                }
                if (XboxCheckBox.IsChecked.Value)
                {
                    kinds.Add(RemoteSystemKinds.Xbox);
                }
                if (kinds.Count == 0)
                {
                    UpdateStatus("Select a system type filter.", NotifyType.ErrorMessage);
                }
                else
                {
                    RemoteSystemKindFilter kindFilter = new RemoteSystemKindFilter(kinds);
                    filters.Add(kindFilter);
                }
            }

            if (StatusTypeOptions.IsChecked.Value)
            {
                // Build status type filters
                if (AvailableRadioButton.IsChecked.Value)
                {
                    statusFilter = new RemoteSystemStatusTypeFilter(RemoteSystemStatusType.Available);
                }
                else
                {
                    statusFilter = new RemoteSystemStatusTypeFilter(RemoteSystemStatusType.Any);
                }
                filters.Add(statusFilter);
            }

            return filters;
        }

        private void SearchCleanup()
        {
            if (m_remoteSystemWatcher != null)
            {
                m_remoteSystemWatcher.Stop();
                m_remoteSystemWatcher = null;
            }
            m_rootPage.systemList.Clear();
            m_rootPage.systemMap.Clear();
        }

        private void UpdateStatus(string status, NotifyType statusType)
        {
            m_rootPage.NotifyUser(status, statusType);
        }
    }
}