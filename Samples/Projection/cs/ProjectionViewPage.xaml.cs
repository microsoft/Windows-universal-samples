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
    public sealed partial class ProjectionViewPage : Page
    {
        ViewLifetimeControl thisViewControl;
        CoreDispatcher mainDispatcher;
        int mainViewId;

        public ProjectionViewPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var initData = (ProjectionViewPageInitializationData)e.Parameter;

            // The ViewLifetimeControl is a convenient wrapper that ensures the
            // view is closed only when the user is done with it
            thisViewControl = initData.ProjectionViewPageControl;
            mainDispatcher = initData.MainDispatcher;
            mainViewId = initData.MainViewId;

            // Listen for when it's time to close this view
            thisViewControl.Released += thisViewControl_Released;
        }

        private async void thisViewControl_Released(object sender, EventArgs e)
        {
            // There are two major cases where this event will get invoked:
            // 1. The view goes unused for some time, and the system cleans it up
            // 2. The app calls "StopProjectingAsync"
            thisViewControl.Released -= thisViewControl_Released;
            await mainDispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                MainPage.Current.ProjectionViewPageControl = null;
            });
            Window.Current.Close();
        }

        private async void SwapViews_Click(object sender, RoutedEventArgs e)
        {
            // The view might arrive on the wrong display. The user can
            // easily swap the display on which the view appears
            thisViewControl.StartViewInUse();
            await ProjectionManager.SwapDisplaysForViewsAsync(
                ApplicationView.GetForCurrentView().Id,
                mainViewId
            );
            thisViewControl.StopViewInUse();
        }

        private async void StopProjecting_Click(object sender, RoutedEventArgs e)
        {
            // There may be cases to end the projection from the projected view
            // (e.g. the presentation hosted in that view concludes)
            thisViewControl.StartViewInUse();
            await ProjectionManager.StopProjectingAsync(
                ApplicationView.GetForCurrentView().Id,
                mainViewId);
            thisViewControl.StopViewInUse();
        }
    }
}
