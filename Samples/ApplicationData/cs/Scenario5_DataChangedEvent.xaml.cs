//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.Storage;

namespace ApplicationDataSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class DataChangedEvent : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        ApplicationData applicationData = null;
        ApplicationDataContainer roamingSettings = null;
        TypedEventHandler<ApplicationData, object> dataChangedHandler = null;

        const string settingName = "userName";

        public DataChangedEvent()
        {
            this.InitializeComponent();

            applicationData = ApplicationData.Current;
            roamingSettings = applicationData.RoamingSettings;

            DisplayOutput();
        }

        void SimulateRoaming_Click(Object sender, RoutedEventArgs e)
        {
            roamingSettings.Values[settingName] = UserName.Text;

            // Simulate roaming by intentionally signaling a data changed event.
            applicationData.SignalDataChanged();
        }

        async void DataChangedHandler(Windows.Storage.ApplicationData appData, object o)
        {
            // DataChangeHandler may be invoked on a background thread, so use the Dispatcher to invoke the UI-related code on the UI thread.
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                DisplayOutput();
            });
        }

        void DisplayOutput()
        {
            Object value = roamingSettings.Values[settingName];
            OutputTextBlock.Text = "Name: " + (value == null ? "<empty>" : ("\"" + value + "\"")); ;
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            dataChangedHandler = new TypedEventHandler<ApplicationData, object>(DataChangedHandler);
            applicationData.DataChanged += dataChangedHandler;
        }

        /// <summary>
        /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        /// </summary>
        /// <param name="e">
        /// Event data that can be examined by overriding code. The event data is representative
        /// of the navigation that will unload the current Page unless canceled. The
        /// navigation can potentially be canceled by setting Cancel.
        /// </param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            base.OnNavigatingFrom(e);
            applicationData.DataChanged -= dataChangedHandler;
            dataChangedHandler = null;
        }
    }
}
