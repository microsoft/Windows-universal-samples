//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using SDKTemplate;
using System;
using Windows.ApplicationModel.Core;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace MediaProjection
{
    public sealed partial class Scenario1 : Page
    {
        MainPage rootPage = MainPage.Current;
        int thisViewId;
        public Scenario1()
        {
            this.InitializeComponent();
            thisViewId = Windows.UI.ViewManagement.ApplicationView.GetForCurrentView().Id;
        }

        private async void StartProjecting_Click(object sender, RoutedEventArgs e)
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
                
                

                if (ProjectionManager.ProjectionDisplayAvailable)
                {
                    // Show the view on a second display (if available) or on the primary display
                    await ProjectionManager.StartProjectingAsync(rootPage.ProjectionViewPageControl.Id, thisViewId);
                }
                else
                {
                    // Show the view on a second display (if available) or on the primary display
                    await ProjectionManager.RequestStartProjectingAsync(rootPage.ProjectionViewPageControl.Id, thisViewId, new Rect());
                }

                rootPage.ProjectionViewPageControl.StopViewInUse();
            }
            catch (InvalidOperationException)
            {
                rootPage.NotifyUser("The projection view is being disposed", NotifyType.ErrorMessage);
            }
        }

        private async void StopProjecting_Click(object sender, RoutedEventArgs e)
        {
            // Only stop a presentation if one is already in progress
            if (rootPage.ProjectionViewPageControl != null)
            {  
                try
                {
                    rootPage.ProjectionViewPageControl.StartViewInUse();
                    // Note that as a result of making this call, the projection view will be "Consolidated"
                    // ViewLifetimeControl, in turn, will close the view
                    await ProjectionManager.StopProjectingAsync(rootPage.ProjectionViewPageControl.Id, thisViewId);
                    rootPage.ProjectionViewPageControl.StopViewInUse();
                }
                catch (InvalidOperationException)
                {
                    rootPage.NotifyUser("The projection view is being disposed", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("A presentation has not been started", NotifyType.ErrorMessage);
            }
        }
    }
}
