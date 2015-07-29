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
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKSample
{
    internal class ProjectionViewPageInitializationData
    {
        public CoreDispatcher MainDispatcher;
        public ViewLifetimeControl ProjectionViewPageControl;
        public int MainViewId;
    }

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1
    {
        MainPage rootPage = MainPage.Current;
        int thisViewId;

        public Scenario1()
        {
            this.InitializeComponent();
            thisViewId = ApplicationView.GetForCurrentView().Id;
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

    }
}
