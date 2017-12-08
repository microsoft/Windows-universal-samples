// Copyright (c) Microsoft. All rights reserved.

using System;
using Windows.Devices.Enumeration;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
// Needed for the DeviceWatcherTrigger
using Windows.ApplicationModel.Background;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace DeviceEnumeration
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3 : Page
    {
        private MainPage rootPage;

        private string backgroundTaskName = "DeviceEnumerationCs_BackgroundTaskName";
        private IBackgroundTaskRegistration backgroundTaskRegistration = null;

        public Scenario3()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            selectorComboBox.ItemsSource = DeviceSelectorChoices.BackgroundDeviceWatcherSelectors;
            selectorComboBox.SelectedIndex = 0;

            DataContext = this;

            // Determine if the background task is already active
            foreach (var task in BackgroundTaskRegistration.AllTasks)
            {
                if (backgroundTaskName == task.Value.Name)
                {
                    backgroundTaskRegistration = task.Value;
                    startWatcherButton.IsEnabled = false;
                    stopWatcherButton.IsEnabled = true;
                    break;
                }
            }
        }

        private void StartWatcherButton_Click(object sender, RoutedEventArgs e)
        {
            StartWatcher();
        }

        private void StopWatcherButton_Click(object sender, RoutedEventArgs e)
        {
            StopWatcher();
        }

        private void StartWatcher()
        {
            DeviceWatcherEventKind[] triggerEventKinds = { DeviceWatcherEventKind.Add, DeviceWatcherEventKind.Remove, DeviceWatcherEventKind.Update };
            DeviceWatcher deviceWatcher = null;

            startWatcherButton.IsEnabled = false;

            // First get the device selector chosen by the UI.
            DeviceSelectorInfo deviceSelectorInfo = (DeviceSelectorInfo)selectorComboBox.SelectedItem;

            if (null == deviceSelectorInfo.Selector)
            {
                // If the a pre-canned device class selector was chosen, call the DeviceClass overload
                deviceWatcher = DeviceInformation.CreateWatcher(deviceSelectorInfo.DeviceClassSelector);
            }
            else if (deviceSelectorInfo.Kind == DeviceInformationKind.Unknown)
            {
                // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
                // Kind will be determined by the selector
                deviceWatcher = DeviceInformation.CreateWatcher(
                    deviceSelectorInfo.Selector,
                    null // don't request additional properties for this sample
                    );
            }
            else
            {
                // Kind is specified in the selector info
                deviceWatcher = DeviceInformation.CreateWatcher(
                    deviceSelectorInfo.Selector,
                    null, // don't request additional properties for this sample
                    deviceSelectorInfo.Kind);
            }

            // Get the background trigger for this watcher
            DeviceWatcherTrigger deviceWatcherTrigger = deviceWatcher.GetBackgroundTrigger(triggerEventKinds);

            rootPage.NotifyUser("Starting Watcher...", NotifyType.StatusMessage);

            // Register this trigger for our background task
            RegisterBackgroundTask(deviceWatcherTrigger);
            
            stopWatcherButton.IsEnabled = true;
            rootPage.NotifyUser("Watcher started...", NotifyType.StatusMessage);
        }

        private void RegisterBackgroundTask(DeviceWatcherTrigger deviceWatcherTrigger)
        {
            BackgroundTaskBuilder taskBuilder;

            // First see if we already have this background task registered. If so, unregister
            // it before we register it with the new trigger.

            foreach (var task in BackgroundTaskRegistration.AllTasks)
            {
                if (backgroundTaskName == task.Value.Name)
                {
                    UnregisterBackgroundTask(task.Value);
                }
            }

            taskBuilder = new BackgroundTaskBuilder();
            taskBuilder.Name = backgroundTaskName;
            taskBuilder.TaskEntryPoint = "BackgroundDeviceWatcherTaskCs.BackgroundDeviceWatcher";
            taskBuilder.SetTrigger(deviceWatcherTrigger);

            backgroundTaskRegistration = taskBuilder.Register();
            backgroundTaskRegistration.Completed += new BackgroundTaskCompletedEventHandler(OnTaskCompleted);
        }

        private async void OnTaskCompleted(BackgroundTaskRegistration task, BackgroundTaskCompletedEventArgs args)
        {
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("Background task completed", NotifyType.StatusMessage);
            });
        }

        private void StopWatcher()
        {
            stopWatcherButton.IsEnabled = false;
            if (null != backgroundTaskRegistration)
            {
                UnregisterBackgroundTask(backgroundTaskRegistration);
            }
            startWatcherButton.IsEnabled = true;
        }

        private void UnregisterBackgroundTask(IBackgroundTaskRegistration taskReg)
        {
            taskReg.Unregister(true);
            ApplicationDataContainer settings = ApplicationData.Current.LocalSettings;
            settings.Values["eventCount"] = (uint)0;
        }
    }
}
