//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.Core;
using Windows.Storage.Provider;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Markup;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class CachedFileUpdaterPage : Page
    {
        public const string FEATURE_NAME = "Cached File Updater Page";
        public event System.EventHandler ScenarioLoaded;
        public event EventHandler<CachedFileUpdaterPageSizeChangedEventArgs> CachedFileUpdaterPageResized;

        public Windows.ApplicationModel.Activation.LaunchActivatedEventArgs LaunchArgs;

        public static CachedFileUpdaterPage Current;

        internal CachedFileUpdaterUI cachedFileUpdaterUI = null;
        internal FileUpdateRequest fileUpdateRequest = null;
        internal FileUpdateRequestDeferral fileUpdateRequestDeferral = null;

        private CoreDispatcher dispatcher = Window.Current.Dispatcher;

        private Frame HiddenFrame = null;

        List<Scenario> scenarios = null;

        public void Activate(CachedFileUpdaterActivatedEventArgs args)
        {
            // cache CachedFileUpdaterUI
            cachedFileUpdaterUI = args.CachedFileUpdaterUI;

            // Currently, the Desktop device family activates the app in the background. Other device families
            // use a background task activation. We need to handle both cases.
            //
            // We can check to see if there is UI on activation to determine which case we are in, because
            // we receive the FileUpdateRequest differently.
            //
            // If activated in the foreground (visible), then the FileUpdateRequest is passed explicitly as
            // part of the activation.
            //
            // If activated in the background (not visible), then we subscribe to the FileUpdateRequested
            // and UIRequest events. The FileUpdateRequest is received as part of the FileUpdatedRequested event,
            // and UIRequested is raised when user input is needed to resolve a conflict.

            bool isForegroundActivated = (cachedFileUpdaterUI.UIStatus == UIStatus.Visible);
            if (isForegroundActivated)
            {
                fileUpdateRequest = cachedFileUpdaterUI.UpdateRequest;
                fileUpdateRequestDeferral = cachedFileUpdaterUI.GetDeferral();
            }
            else
            {
                cachedFileUpdaterUI.FileUpdateRequested += CachedFileUpdaterUI_FileUpdateRequested;
                cachedFileUpdaterUI.UIRequested += CachedFileUpdaterUI_UIRequested;
            }

            switch (cachedFileUpdaterUI.UpdateTarget)
            {
                case CachedFileTarget.Local:
                    scenarios = new List<Scenario> { new Scenario() { Title = "Get latest version", ClassType = typeof(FilePickerContracts.CachedFileUpdater_Local) } };
                    break;
                case CachedFileTarget.Remote:
                    scenarios = new List<Scenario> { new Scenario() { Title = "Remote file update", ClassType = typeof(FilePickerContracts.CachedFileUpdater_Remote) } };
                    break;
            }

            if (isForegroundActivated)
            {
                Window.Current.Content = this;
                this.OnNavigatedTo(null);
                Window.Current.Activate();
            }
        }

        async void CachedFileUpdaterUI_UIRequested(CachedFileUpdaterUI sender, object args)
        {
            // The event handler may be invoked on a background thread, so use the Dispatcher to run the UI-related code on the UI thread.
            await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                Window.Current.Content = this;
                this.OnNavigatedTo(null);
            });
        }

        void CachedFileUpdaterUI_FileUpdateRequested(CachedFileUpdaterUI sender, FileUpdateRequestedEventArgs args)
        {
            fileUpdateRequest = args.Request;
            fileUpdateRequestDeferral = fileUpdateRequest.GetDeferral();
            switch (cachedFileUpdaterUI.UIStatus)
            {
                case UIStatus.Hidden:
                    if (fileUpdateRequest.File.Name.Contains("ConflictingFile"))
                    {
                        fileUpdateRequest.Status = FileUpdateStatus.UserInputNeeded;
                    }
                    else
                    {
                        fileUpdateRequest.Status = FileUpdateStatus.Complete;
                    }
                    fileUpdateRequestDeferral.Complete();
                    break;
                case UIStatus.Visible:
                    break;
                case UIStatus.Unavailable:
                    fileUpdateRequest.Status = FileUpdateStatus.Failed;
                    fileUpdateRequestDeferral.Complete();
                    break;
            }
        }

        public CachedFileUpdaterPage()
        {
            this.InitializeComponent();

            // This is a static public property that will allow downstream pages to get
            // a handle to the CachedFileUpdaterPage instance in order to call methods that are in this class.
            Current = this;

            // This frame is hidden, meaning it is never shown.  It is simply used to load
            // each scenario page and then pluck out the input and output sections and
            // place them into the UserControls on the main page.
            HiddenFrame = new Windows.UI.Xaml.Controls.Frame();
            HiddenFrame.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            LayoutRoot.Children.Add(HiddenFrame);

            // Populate the sample title from the constant in the GlobalVariables.cs file.
            SetFeatureName(FEATURE_NAME);

            Scenarios.SelectionChanged += Scenarios_SelectionChanged;
            SizeChanged += CachedFileUpdaterPage_SizeChanged;
        }

        void CachedFileUpdaterPage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            InvalidateSize();
            if (CachedFileUpdaterPageResized != null)
            {
                CachedFileUpdaterPageSizeChangedEventArgs args = new CachedFileUpdaterPageSizeChangedEventArgs();
                args.Width = this.ActualWidth;
                CachedFileUpdaterPageResized(this, args);
            }
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            PopulateScenarios();
            InvalidateLayout();
        }

        private void InvalidateSize()
        {
            // Get the window width
            double windowWidth = this.ActualWidth;

            if (windowWidth != 0.0)
            {
                // Get the width of the ListBox.
                double listBoxWidth = Scenarios.ActualWidth;

                // Is the ListBox using any margins that we need to consider?
                double listBoxMarginLeft = Scenarios.Margin.Left;
                double listBoxMarginRight = Scenarios.Margin.Right;

                // Figure out how much room is left after considering the list box width
                double availableWidth = windowWidth - listBoxWidth;

                // Is the top most child using margins?
                double layoutRootMarginLeft = ContentRoot.Margin.Left;
                double layoutRootMarginRight = ContentRoot.Margin.Right;

                // We have different widths to use depending on the view state
                if (this.ActualWidth >= 768)
                {
                    // Make us as big as the the left over space, factoring in the ListBox width, the ListBox margins.
                    // and the LayoutRoot's margins
                    InputSection.Width = ((availableWidth) - (layoutRootMarginLeft + layoutRootMarginRight + listBoxMarginLeft + listBoxMarginRight));
                }
                else
                {
                    // Make us as big as the left over space, factoring in just the LayoutRoot's margins.
                        InputSection.Width = (windowWidth - (layoutRootMarginLeft + layoutRootMarginRight));
                    
                }
            }
            InvalidateLayout();
        }

        private void InvalidateLayout()
        {

            if (this.ActualWidth < 768)
            {
                Grid.SetRow(DescriptionText, 3);
                Grid.SetColumn(DescriptionText, 0);

                Grid.SetRow(InputSection, 4);
                Grid.SetColumn(InputSection, 0);

                Grid.SetRow(FooterPanel, 2);
                Grid.SetColumn(FooterPanel, 0);
            }
            else
            {
                Grid.SetRow(DescriptionText, 1);
                Grid.SetColumn(DescriptionText, 1);

                Grid.SetRow(InputSection, 2);
                Grid.SetColumn(InputSection, 1);

                Grid.SetRow(FooterPanel, 1);
                Grid.SetColumn(FooterPanel, 1);
            }
        }

        private void PopulateScenarios()
        {
            System.Collections.ObjectModel.ObservableCollection<object> ScenarioList = new System.Collections.ObjectModel.ObservableCollection<object>();
            int i = 0;

            // Populate the ListBox with the list of scenarios as defined in Constants.cs.
            foreach (Scenario s in scenarios)
            {
                ListBoxItem item = new ListBoxItem();
                s.Title = (++i).ToString() + ") " + s.Title;
                item.Content = s;
                item.Name = s.ClassType.FullName;
                ScenarioList.Add(item);
            }

            // Bind the ListBox to the scenario list.
            Scenarios.ItemsSource = ScenarioList;
            Scenarios.SelectedIndex = 0;
        }

        /// <summary>
        /// This method is responsible for loading the individual input and output sections for each scenario.  This
        /// is based on navigating a hidden Frame to the ScenarioX.xaml page and then extracting out the input
        /// and output sections into the respective UserControl on the main page.
        /// </summary>
        /// <param name="scenarioName"></param>
        public void LoadScenario(Type scenarioClass)
        {

            // Load the ScenarioX.xaml file into the Frame.
            HiddenFrame.Navigate(scenarioClass, this);

            // Get the top element, the Page, so we can look up the elements
            // that represent the input and output sections of the ScenarioX file.
            Page hiddenPage = HiddenFrame.Content as Page;

            // Get each element.
            UIElement input = hiddenPage.FindName("Input") as UIElement;
            UIElement output = hiddenPage.FindName("Output") as UIElement;

            if (input == null)
            {
                // Malformed input section.
                NotifyUser(String.Format(
                    "Cannot load scenario input section for {0}.  Make sure root of input section markup has x:Name of 'Input'",
                    scenarioClass.Name), NotifyType.ErrorMessage);
                return;
            }

            if (output == null)
            {
                // Malformed output section.
                NotifyUser(String.Format(
                    "Cannot load scenario output section for {0}.  Make sure root of output section markup has x:Name of 'Output'",
                    scenarioClass.Name), NotifyType.ErrorMessage);
                return;
            }

            // Find the LayoutRoot which parents the input and output sections in the main page.
            Panel panel = hiddenPage.FindName("LayoutRoot") as Panel;

            if (panel != null)
            {
                // Get rid of the content that is currently in the intput and output sections.
                panel.Children.Remove(input);
                panel.Children.Remove(output);

                // Populate the input and output sections with the newly loaded content.
                InputSection.Content = input;
                OutputSection.Content = output;

                if (ScenarioLoaded != null)
                {
                    ScenarioLoaded(this, new EventArgs());
                }
            }
            else
            {
                // Malformed Scenario file.
                NotifyUser(String.Format(
                    "Cannot load scenario: '{0}'.  Make sure root tag in the '{0}' file has an x:Name of 'LayoutRoot'",
                    scenarioClass.Name), NotifyType.ErrorMessage);
            }

        }

        void Scenarios_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (Scenarios.SelectedItem != null)
            {
                NotifyUser("", NotifyType.StatusMessage);

                ListBoxItem selectedListBoxItem = Scenarios.SelectedItem as ListBoxItem;
                Scenario scenario = selectedListBoxItem.Content as Scenario;
                LoadScenario(scenario.ClassType);
                InvalidateSize();
            }
        }

        public void NotifyUser(string strMessage, NotifyType type)
        {
            switch (type)
            {
                // Use the status message style.
                case NotifyType.StatusMessage:
                    StatusBlock.Style = Resources["StatusStyle"] as Style;
                    break;
                // Use the error message style.
                case NotifyType.ErrorMessage:
                    StatusBlock.Style = Resources["ErrorStyle"] as Style;
                    break;
            }
            StatusBlock.Text = strMessage;

            // Collapse the StatusBlock if it has no text to conserve real estate.
            if (StatusBlock.Text != String.Empty)
            {
                StatusBlock.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else
            {
                StatusBlock.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }
        }

        async void Footer_Click(object sender, RoutedEventArgs e)
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri(((HyperlinkButton)sender).Tag.ToString()));
        }

        private void SetFeatureName(string str)
        {
            FeatureName.Text = str;
        }
    }

    public class CachedFileUpdaterPageSizeChangedEventArgs : EventArgs
    {
        private double width;

        public double Width
        {
            get { return width; }
            set { width = value; }
        }
    }
}
