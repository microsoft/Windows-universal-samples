//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using SDKTemplate;
using System;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;


namespace MediaProjection
{
    public sealed partial class Scenario2 : Page
    {
        MainPage rootPage = MainPage.Current;
        CoreDispatcher dispatcher;

        public Scenario2()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // An app can query if a second display is available...
            UpdateTextBlock(ProjectionManager.ProjectionDisplayAvailable);
            // ...or listen for when a display is attached or detached
            ProjectionManager.ProjectionDisplayAvailableChanged += ProjectionManager_ProjectionDisplayAvailableChanged;
            dispatcher = Window.Current.Dispatcher;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            ProjectionManager.ProjectionDisplayAvailableChanged -= ProjectionManager_ProjectionDisplayAvailableChanged;
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
                ScreenAvailabilityBlock.Text = "A second screen is available";
            }
            else
            {
                ScreenAvailabilityBlock.Text = "No second screen is available";
            }
        }
    }
}
