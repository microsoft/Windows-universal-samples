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
using System.Threading.Tasks;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

using SDKTemplate;

namespace ActivitySensorCS
{
    public sealed partial class Scenario3_ChangeEvents : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private ActivitySensor _activitySensor;

        public Scenario3_ChangeEvents()
        {
            this.InitializeComponent();
            _activitySensor = null;
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ScenarioEnableReadingChangedButton.IsEnabled = true;
            ScenarioDisableReadingChangedButton.IsEnabled = false;
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
            if (ScenarioDisableReadingChangedButton.IsEnabled)
            {
                _activitySensor.ReadingChanged -= new TypedEventHandler<ActivitySensor, ActivitySensorReadingChangedEventArgs>(ReadingChanged);
            }

            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// This is the event handler for ReadingChanged events.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void ReadingChanged(object sender, ActivitySensorReadingChangedEventArgs e)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                ActivitySensorReading reading = e.Reading;
                ScenarioOutput_Activity.Text = reading.Activity.ToString();
                ScenarioOutput_Confidence.Text = reading.Confidence.ToString();
                ScenarioOutput_ReadingTimestamp.Text = reading.Timestamp.ToString("u");
            }); 
        }

        /// <summary>
        /// This is the click handler for the 'Reading Changed On' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void ScenarioEnableReadingChanged(object sender, RoutedEventArgs e)
        {
            try
            {
                if (null == _activitySensor)
                {
                    _activitySensor = await ActivitySensor.GetDefaultAsync();
                }

                if (null != _activitySensor)
                {
                    _activitySensor.SubscribedActivities.Add(ActivityType.Walking);
                    _activitySensor.SubscribedActivities.Add(ActivityType.Running);
                    _activitySensor.SubscribedActivities.Add(ActivityType.InVehicle);
                    _activitySensor.SubscribedActivities.Add(ActivityType.Biking);
                    _activitySensor.SubscribedActivities.Add(ActivityType.Fidgeting);

                    _activitySensor.ReadingChanged += new TypedEventHandler<ActivitySensor, ActivitySensorReadingChangedEventArgs>(ReadingChanged);

                    ScenarioEnableReadingChangedButton.IsEnabled = false;
                    ScenarioDisableReadingChangedButton.IsEnabled = true;
                }
                else
                {
                    rootPage.NotifyUser("No activity sensor found", NotifyType.ErrorMessage);
                }
            }
            catch (UnauthorizedAccessException)
            {
                rootPage.NotifyUser("User has denied access to the activity sensor", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Reading Changed Off' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioDisableReadingChanged(object sender, RoutedEventArgs e)
        {
            _activitySensor.ReadingChanged -= new TypedEventHandler<ActivitySensor, ActivitySensorReadingChangedEventArgs>(ReadingChanged);

            ScenarioEnableReadingChangedButton.IsEnabled = true;
            ScenarioDisableReadingChangedButton.IsEnabled = false;
        }
    }
}
