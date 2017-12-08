// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace DeviceEnumeration
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario7 : Page
    {
        private MainPage rootPage;

        private List<DeviceWatcher> deviceWatchers = null;
        private TypedEventHandler<DeviceWatcher, DeviceInformation> handlerAdded = null;
        private TypedEventHandler<DeviceWatcher, DeviceInformationUpdate> handlerUpdated = null;
        private TypedEventHandler<DeviceWatcher, DeviceInformationUpdate> handlerRemoved = null;
        private TypedEventHandler<DeviceWatcher, Object> handlerEnumCompleted = null;
        private TypedEventHandler<DeviceWatcher, Object> handlerStopped = null;

        public ObservableCollection<DeviceInformationDisplay> ResultCollection
        {
            get;
            private set;
        }

        public Scenario7()
        {
            this.InitializeComponent();
            deviceWatchers = new List<DeviceWatcher>();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            ResultCollection = new ObservableCollection<DeviceInformationDisplay>();

            kindComboBox.DataContext = DeviceInformationKindChoices.Choices;
            kindComboBox.SelectedIndex = 0;

            DataContext = this;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            StopWatcher();
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
            startWatcherButton.IsEnabled = false;
            ResultCollection.Clear();
            deviceWatchers.Clear();

            // Create all the event handlers

            handlerAdded = new TypedEventHandler<DeviceWatcher, DeviceInformation>(async (watcher, deviceInfo) =>
            {
                // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
                await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                {
                    ResultCollection.Add(new DeviceInformationDisplay(deviceInfo));

                    UpdateStatusText();
                });
            });

            handlerUpdated = new TypedEventHandler<DeviceWatcher, DeviceInformationUpdate>(async (watcher, deviceInfoUpdate) =>
            {
                // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
                await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                {
                    // Find the corresponding updated DeviceInformation in the collection and pass the update object
                    // to the Update method of the existing DeviceInformation. This automatically updates the object
                    // for us.
                    foreach (DeviceInformationDisplay deviceInfoDisp in ResultCollection)
                    {
                        if (deviceInfoDisp.Id == deviceInfoUpdate.Id)
                        {
                            deviceInfoDisp.Update(deviceInfoUpdate);
                            break;
                        }
                    }
                });
            });

            handlerRemoved = new TypedEventHandler<DeviceWatcher, DeviceInformationUpdate>(async (watcher, deviceInfoUpdate) =>
            {
                // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
                await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                {
                    // Find the corresponding DeviceInformation in the collection and remove it
                    foreach (DeviceInformationDisplay deviceInfoDisp in ResultCollection)
                    {
                        if (deviceInfoDisp.Id == deviceInfoUpdate.Id)
                        {
                            ResultCollection.Remove(deviceInfoDisp);
                            break;
                        }
                    }

                    UpdateStatusText();
                });
            });

            handlerEnumCompleted = new TypedEventHandler<DeviceWatcher, Object>(async (watcher, obj) =>
            {
                await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                {
                    UpdateStatusText();
                });
            });

            handlerStopped = new TypedEventHandler<DeviceWatcher, Object>(async (watcher, obj) =>
            {
                await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                {
                    UpdateStatusText();
                });
            });

            // Create a watcher for each DeviceInformationKind selected by the user
            foreach (DeviceInformationKind deviceInfoKind in ((DeviceInformationKindChoice)kindComboBox.SelectedItem).DeviceInformationKinds)
            {
                DeviceWatcher deviceWatcher = DeviceInformation.CreateWatcher(
                        "", // AQS Filter string
                        null, // requested properties
                        deviceInfoKind);

                deviceWatchers.Add(deviceWatcher);

                // Hook up handlers for the watcher events before starting the watcher

                deviceWatcher.Added += handlerAdded;
                deviceWatcher.Updated += handlerUpdated;
                deviceWatcher.Removed += handlerRemoved;
                deviceWatcher.EnumerationCompleted += handlerEnumCompleted;
                deviceWatcher.Stopped += handlerStopped;

                deviceWatcher.Start();
            }

            stopWatcherButton.IsEnabled = true;
            stopWatcherButton.Focus(FocusState.Keyboard);
        }

        private void StopWatcher()
        {
            stopWatcherButton.IsEnabled = false;

            foreach (DeviceWatcher deviceWatcher in deviceWatchers)
            {
                // First unhook all event handlers except the stopped handler. This ensures our
                // event handlers don't get called after stop, as stop won't block for any "in flight" 
                // event handler calls.  We leave the stopped handler as it's guaranteed to only be called
                // once and we'll use it to know when the query is completely stopped. 
                deviceWatcher.Added -= handlerAdded;
                deviceWatcher.Updated -= handlerUpdated;
                deviceWatcher.Removed -= handlerRemoved;
                deviceWatcher.EnumerationCompleted -= handlerEnumCompleted;

                if (DeviceWatcherStatus.Started == deviceWatcher.Status ||
                    DeviceWatcherStatus.EnumerationCompleted == deviceWatcher.Status)
                {
                    deviceWatcher.Stop();
                }
            }

            startWatcherButton.IsEnabled = true;
            startWatcherButton.Focus(FocusState.Keyboard);
        }

        public void UpdateStatusText()
        {
            int watchersRunning = 0;
            string statusTxt = null;

            // Count running watchers
            foreach (DeviceWatcher deviceWatcher in deviceWatchers)
            {
                if (deviceWatcher.Status == DeviceWatcherStatus.Started ||
                    deviceWatcher.Status == DeviceWatcherStatus.EnumerationCompleted ||
                    deviceWatcher.Status == DeviceWatcherStatus.Stopping)
                {
                    watchersRunning++;
                }
            }

            // First add the collective state of the watchers
            statusTxt = String.Format(
                "{0}/{1} Watchers running. {2} devices found.",
                watchersRunning,
                deviceWatchers.Count,
                ResultCollection.Count);

            rootPage.NotifyUser(statusTxt, NotifyType.StatusMessage);
        }
    }
}
