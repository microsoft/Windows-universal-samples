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
using Windows.Networking.Connectivity;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_NetworkConnectivityChanges : Page
    {
        private bool m_isNetworkStatusChangeCallbackRegistered = false;
        private bool m_isUpdatingNetworkStatus = false;
        private bool m_isNetworkStatusUpdatePending = false;

        public Scenario3_NetworkConnectivityChanges()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            UpdateButtonStates();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (m_isNetworkStatusChangeCallbackRegistered)
            {
                UnregisterNetworkStatusChanged();
            }
        }

        async void UpdateNetworkStatus()
        {
            // This code runs on the UI thread, so there are no race conditions on these
            // member variables.
            if (m_isUpdatingNetworkStatus)
            {
                // Update is already in progress. Let it finish before we re-evaluate.
                m_isNetworkStatusUpdatePending = true;
                return;
            }

            m_isUpdatingNetworkStatus = true;

            do
            {
                m_isNetworkStatusUpdatePending = false;
                ResultsText.Text = string.Empty;
                ConnectionProfile internetConnectionProfile = NetworkInformation.GetInternetConnectionProfile();
                NetworkConnectivityLevel connectivityLevel = NetworkConnectivityLevel.None;
                if (internetConnectionProfile != null)
                {
                    connectivityLevel = internetConnectionProfile.GetNetworkConnectivityLevel();
                }

                Helpers.AppendLine(ResultsText, $"Current connectivity is {connectivityLevel}.");
                if (Helpers.ShouldAttemptToConnectToInternet(connectivityLevel))
                {
                    Helpers.AppendLine(ResultsText, "Checking network cost before connecting to the Internet.");
                    await Helpers.EvaluateCostAndConnectAsync(internetConnectionProfile.GetConnectionCost(), OptedInToNetworkUsageToggle.IsOn, ResultsText);
                }
                else
                {
                    Helpers.AppendLine(ResultsText, "Not attempting to connect to the Internet.");
                }
            }
            while (m_isNetworkStatusUpdatePending);
            m_isUpdatingNetworkStatus = false;
        }

        async private void OnNetworkStatusChanged(object sender)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                UpdateNetworkStatus();
            });
        }

        private void RegisterNetworkStatusChanged()
        {
            NetworkInformation.NetworkStatusChanged += OnNetworkStatusChanged;
            m_isNetworkStatusChangeCallbackRegistered = true;
            UpdateNetworkStatus();
        }

        private void UnregisterNetworkStatusChanged()
        {
            NetworkInformation.NetworkStatusChanged -= OnNetworkStatusChanged;
            m_isNetworkStatusChangeCallbackRegistered = false;
        }

        private void RegisterUnregisterButton_Click(object sender, RoutedEventArgs e)
        {
            if (m_isNetworkStatusChangeCallbackRegistered)
            {
                UnregisterNetworkStatusChanged();
            }
            else
            {
                RegisterNetworkStatusChanged();
            }
            UpdateButtonStates();
        }

        /// <summary>
        /// Update the UI based on whether we are registered for the event.
        /// </summary>
        private void UpdateButtonStates()
        {
            VisualStateManager.GoToState(this, m_isNetworkStatusChangeCallbackRegistered ? "Registered" : "Unregistered", false);
        }
    }
}
