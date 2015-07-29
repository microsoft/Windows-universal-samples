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

using SDKTemplate;
using SecondaryViewsHelpers;
using System;
using Windows.ApplicationModel.Core;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2:Page
    {
        MainPage rootPage = MainPage.Current;
        CoreDispatcher dispatcher;
        int thisViewId;

        public Scenario2()
        {
            InitializeComponent();
            thisViewId = ApplicationView.GetForCurrentView().Id;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            ProjectionManager.ProjectionDisplayAvailableChanged -= ProjectionManager_ProjectionDisplayAvailableChanged;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // An app can query if a second display is available...
            UpdateTextBlock(ProjectionManager.ProjectionDisplayAvailable);
            // ...or listen for when a display is attached or detached
            ProjectionManager.ProjectionDisplayAvailableChanged += ProjectionManager_ProjectionDisplayAvailableChanged;
            dispatcher = Window.Current.Dispatcher;
        }

        private async void ProjectionManager_ProjectionDisplayAvailableChanged(object sender, object e)
        {
            await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                UpdateTextBlock(ProjectionManager.ProjectionDisplayAvailable);
            });
        }

        private void UpdateTextBlock(bool screenAvailable)
        {
            if (screenAvailable)
            {
                this.displayAvailableStatus.Text = "Display is available";
                this.startprojecting_button.Visibility = Visibility.Visible;
                this.requestandstartprojecting_button.Visibility = Visibility.Collapsed;
            }
            else
            {
                this.displayAvailableStatus.Text = "Display is not available, select one:";
                this.startprojecting_button.Visibility = Visibility.Collapsed;
                this.requestandstartprojecting_button.Visibility = Visibility.Visible;
            }
        }

        private async void StartProjecting_Click(object sender, RoutedEventArgs e)
        {
            // If projection is already in progress, then it could be shown on the monitor again
            // Otherwise, we need to create a new view to show the presentation
            if (rootPage.ProjectionViewPageControl == null)
            {
                // First, create a new, blank view
                await CoreApplication.CreateNewView().Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    // ViewLifetimeControl is a wrapper to make sure the view is closed only
                    // when the app is done with it
                    rootPage.ProjectionViewPageControl = ViewLifetimeControl.CreateForCurrentView();

                    // Assemble some data necessary for the new page
                    var initData = new ProjectionViewPageInitializationData();
                    initData.MainDispatcher = dispatcher;
                    initData.ProjectionViewPageControl = rootPage.ProjectionViewPageControl;
                    initData.MainViewId = thisViewId;

                    // Display the page in the view. Note that the view will not become visible
                    // until "StartProjectingAsync" is called
                    var rootFrame = new Frame();
                    rootFrame.Navigate(typeof(ProjectionViewPage), initData);
                    Window.Current.Content = rootFrame;

                    // The call to Window.Current.Activate is required starting in Windos 10.
                    // Without it, the view will never appear.
                    Window.Current.Activate();
                });
            }

            try
            {
                // Start/StopViewInUse are used to signal that the app is interacting with the
                // view, so it shouldn't be closed yet, even if the user loses access to it
                rootPage.ProjectionViewPageControl.StartViewInUse();

                // Show the view on a second display (if available) or on the primary display
                await ProjectionManager.StartProjectingAsync(rootPage.ProjectionViewPageControl.Id, thisViewId);

                rootPage.ProjectionViewPageControl.StopViewInUse();

                rootPage.NotifyUser("Projection started with success", NotifyType.StatusMessage);
            }
            catch (InvalidOperationException)
            {
                rootPage.NotifyUser("The projection view is being disposed", NotifyType.ErrorMessage);
            }
        }

        private async void RequestAndStartProjecting_Click(object sender, RoutedEventArgs e)
        {
            // If projection is already in progress, then it could be shown on the monitor again
            // Otherwise, we need to create a new view to show the presentation
            if (rootPage.ProjectionViewPageControl == null)
            {
                // First, create a new, blank view
                var thisDispatcher = Window.Current.Dispatcher;
                await CoreApplication.CreateNewView().Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    // ViewLifetimeControl is a wrapper to make sure the view is closed only
                    // when the app is done with it
                    rootPage.ProjectionViewPageControl = ViewLifetimeControl.CreateForCurrentView();

                    // Assemble some data necessary for the new page
                    var initData = new ProjectionViewPageInitializationData();
                    initData.MainDispatcher = thisDispatcher;
                    initData.ProjectionViewPageControl = rootPage.ProjectionViewPageControl;
                    initData.MainViewId = thisViewId;

                    // Display the page in the view. Note that the view will not become visible
                    // until "StartProjectingAsync" is called
                    var rootFrame = new Frame();
                    rootFrame.Navigate(typeof(ProjectionViewPage), initData);
                    Window.Current.Content = rootFrame;
                    Window.Current.Activate();
                });
            }

            try
            {
                // Start/StopViewInUse are used to signal that the app is interacting with the
                // view, so it shouldn't be closed yet, even if the user loses access to it
                rootPage.ProjectionViewPageControl.StartViewInUse();

                // Show the view on a second display (if available) or on the primary display
                Rect pickerLocation = new Rect(470.0, 0.0, 200.0, 300.0);
                await ProjectionManager.RequestStartProjectingAsync(rootPage.ProjectionViewPageControl.Id, thisViewId, pickerLocation, Windows.UI.Popups.Placement.Above);

                rootPage.ProjectionViewPageControl.StopViewInUse();

                rootPage.NotifyUser("Projection started with success", NotifyType.StatusMessage);
            }
            catch (InvalidOperationException)
            {
                rootPage.NotifyUser("The projection view is being disposed", NotifyType.ErrorMessage);
            }
        }

    }
}
