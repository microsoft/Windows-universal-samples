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
using HotspotAuthenticationTask;
using System;
using Windows.ApplicationModel.Background;
using Windows.Data.Xml.Dom;
using Windows.Networking.NetworkOperators;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_Initialization : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_Initialization()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ConfigStore.UseNativeWISPr = true;

            // Configure background task handler to perform authentication as default
            ConfigStore.AuthenticateThroughBackgroundTask = true;

            // Setup completion handler
            var isTaskRegistered = ScenarioCommon.Instance.RegisteredCompletionHandlerForBackgroundTask();

            // Initialize button state
            UpdateButtonState(isTaskRegistered);

        }

        private async void ProvisionButton_Click(object sender, RoutedEventArgs e)
        {
            ProvisionButton.IsEnabled = false;

            // Open the installation folder
            var installLocation = Windows.ApplicationModel.Package.Current.InstalledLocation;

            // Access the provisioning file
            var provisioningFile = await installLocation.GetFileAsync("ProvisioningData.xml");

            // Load with XML parser
            var xmlDocument = await XmlDocument.LoadFromFileAsync(provisioningFile);

            // Get raw XML
            var provisioningXml = xmlDocument.GetXml();

            // Create ProvisiongAgent Object
            var provisioningAgent = new ProvisioningAgent();

            try
            {
                // Create ProvisionFromXmlDocumentResults Object
                var result = await provisioningAgent.ProvisionFromXmlDocumentAsync(provisioningXml);

                if (result.AllElementsProvisioned)
                {
                    rootPage.NotifyUser("Provisioning was successful", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Provisioning result: " + result.ProvisionResultsXml, NotifyType.ErrorMessage);
                }
            }
            catch (System.Exception ex)
            {
                // See https://docs.microsoft.com/en-us/uwp/api/windows.networking.networkoperators.provisioningagent.provisionfromxmldocumentasync
                // for list of possible exceptions.
                rootPage.NotifyUser($"Unable to provision: {ex}", NotifyType.ErrorMessage);
            }

            ProvisionButton.IsEnabled = true;
        }

        private void RegisterButton_Click(object sender, RoutedEventArgs e)
        {
            // Create a new background task builder.
            var taskBuilder = new BackgroundTaskBuilder();

            // Create a new NetworkOperatorHotspotAuthentication trigger.
            var trigger = new NetworkOperatorHotspotAuthenticationTrigger();

            // Associate the NetworkOperatorHotspotAuthentication trigger with the background task builder.
            taskBuilder.SetTrigger(trigger);

            // Specify the background task to run when the trigger fires.
            taskBuilder.TaskEntryPoint = ScenarioCommon.BackgroundTaskEntryPoint;

            // Name the background task.
            taskBuilder.Name = ScenarioCommon.BackgroundTaskName;

            try
            {
                // Register the background task.
                var task = taskBuilder.Register();

                // Associate progress and completed event handlers with the new background task.
                task.Completed += ScenarioCommon.Instance.OnBackgroundTaskCompleted;

                UpdateButtonState(true);

                rootPage.NotifyUser("Register was successful", NotifyType.StatusMessage);
            }
            catch (System.Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private void UnregisterButton_Click(object sender, RoutedEventArgs e)
        {
            if (UnregisterBackgroundTask())
            {
                UpdateButtonState(false);
                rootPage.NotifyUser("Unregister was successful.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(String.Format("Background {0} task is not registered", ScenarioCommon.BackgroundTaskName), NotifyType.StatusMessage);
            }

        }

        private bool UnregisterBackgroundTask()
        {
            bool unregister = false;
            // Loop through all background tasks and unregister any.
            foreach (var cur in BackgroundTaskRegistration.AllTasks)
            {
                if (cur.Value.Name == ScenarioCommon.BackgroundTaskName)
                {
                    cur.Value.Unregister(true);
                    unregister = true;
                }
            }
            return unregister;
            
        }

        /// <summary>
        /// Enables or disables register and unregister buttons
        /// </summary>
        /// <param name="registered">True if background task is registered, false otherwise</param>
        private void UpdateButtonState(bool registered)
        {
            if (registered)
            {
                RegisterButton.IsEnabled = false;
                UnregisterButton.IsEnabled = true;
            }
            else
            {
                RegisterButton.IsEnabled = true;
                UnregisterButton.IsEnabled = false;
            }
        }
    }
}
