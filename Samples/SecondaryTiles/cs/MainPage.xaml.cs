// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.ApplicationSettings;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using SDKTemplate.Common;
using Windows.UI.Core;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public static MainPage Current;
        public string LaunchParam { get; set; }
        partial void GetScenarioIdForLaunch(string launchParam, ref int index);

        public MainPage()
        {
            this.InitializeComponent();
            SampleTitle.Text = FEATURE_NAME;

            // This is a static public property that allows downstream pages to get a handle to the MainPage instance
            // in order to call methods that are in this class.
            Current = this;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Populate the scenario list from the SampleConfiguration.cs file
            if (Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
            {
                ScenarioControl.ItemsSource = mobilescenarios;
            }
            else
            {
                ScenarioControl.ItemsSource = desktopscenarios;
            }

            // If we have saved state return to the previously selected scenario  
            if (SuspensionManager.SessionState.ContainsKey("SelectedScenarioIndex") && String.IsNullOrEmpty(e.Parameter.ToString()))
            {
                ScenarioControl.SelectedIndex = Convert.ToInt32(SuspensionManager.SessionState["SelectedScenarioIndex"]);
                ScenarioControl.ScrollIntoView(ScenarioControl.SelectedItem);
            }
            else if (!String.IsNullOrEmpty(e.Parameter.ToString()))
            {
                this.LaunchParam = e.Parameter.ToString();
                int index = 0;
                this.GetScenarioIdForLaunch(this.LaunchParam, ref index);
                ScenarioControl.SelectedIndex = index;
            }
            else
            {
                ScenarioControl.SelectedIndex = 0;
            }

        }

        /// <summary>
        /// Called whenever the user changes selection in the scenarios list.  This method will navigate to the respective
        /// sample scenario page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Clear the status block when navigating scenarios.
            NotifyUser(String.Empty, NotifyType.StatusMessage);

            ListBox scenarioListBox = sender as ListBox;
            Scenario s = scenarioListBox.SelectedItem as Scenario;
            if (s != null)
            {
                SuspensionManager.SessionState["SelectedScenarioIndex"] = scenarioListBox.SelectedIndex;
                ScenarioFrame.Navigate(s.ClassType);
            }
        }

        public List<Scenario> DesktopScenarios
        {
            get { return this.desktopscenarios; }
        }
        public List<Scenario> MobileScenarios
        {
            get
            {
                return this.mobilescenarios;
            }
        }

        /// <summary>
        /// Used to display messages to the user
        /// </summary>
        /// <param name="strMessage"></param>
        /// <param name="type"></param>
        public void NotifyUser(string strMessage, NotifyType type)
        {
            switch (type)
            {
                case NotifyType.StatusMessage:
                    StatusBorder.Background = new SolidColorBrush(Windows.UI.Colors.Green);
                    break;
                case NotifyType.ErrorMessage:
                    StatusBorder.Background = new SolidColorBrush(Windows.UI.Colors.Red);
                    break;
            }
            StatusBlock.Text = strMessage;

            // Collapse the StatusBlock if it has no text to conserve real estate.
            if (StatusBlock.Text != String.Empty)
            {
                StatusBorder.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else
            {
                StatusBorder.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }
        }

        async void Footer_Click(object sender, RoutedEventArgs e)
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri(((HyperlinkButton)sender).Tag.ToString()));
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Splitter.IsPaneOpen = (Splitter.IsPaneOpen == true) ? false : true;
            StatusBorder.Visibility = Visibility.Collapsed;
        }

    }

    public enum NotifyType
    {
        StatusMessage,
        ErrorMessage
    };

    public class ScenarioBindingConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if (Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
            {
                Scenario s = value as Scenario;
                return (MainPage.Current.MobileScenarios.IndexOf(s) + 1) + ") " + s.Title;
            }
            else
            {
                Scenario s = value as Scenario;
                return (MainPage.Current.DesktopScenarios.IndexOf(s) + 1) + ") " + s.Title;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return true;
        }
    }

}
