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

using SecondaryViewsHelpers;
using System;
using System.Collections.Generic;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed class SizePreferenceString
    {
        public string Title { get; set;}
        public ViewSizePreference Preference { get; set; }
    }

    public sealed partial class Scenario1
    {
        const string DEFAULT_TITLE = "New window";
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario1()
        {
            this.InitializeComponent();

            // "UseNone" is not a valid choice for the incoming view, so only include
            // it in the anchor size preference chooser
            var anchorSizeChoices = GenerateSizePreferenceBinding();
            anchorSizeChoices.Add(new SizePreferenceString() { Preference = ViewSizePreference.UseNone, Title = "UseNone" });
            AnchorSizePreferenceChooser.ItemsSource = anchorSizeChoices;
            AnchorSizePreferenceChooser.SelectedIndex = 0;

            SizePreferenceChooser.ItemsSource = GenerateSizePreferenceBinding();
            SizePreferenceChooser.SelectedIndex = 0;

            // This collection is being bound to the current thread.
            // So, make sure you only update the collection and items
            // contained in it from this thread.
            ViewChooser.ItemsSource = ((App)App.Current).SecondaryViews;
        }

        private List<SizePreferenceString> GenerateSizePreferenceBinding()
        {
            var sizeChoices = new List<SizePreferenceString>();
            sizeChoices.Add(new SizePreferenceString() { Preference = ViewSizePreference.Default,    Title = "Default" });
            sizeChoices.Add(new SizePreferenceString() { Preference = ViewSizePreference.UseHalf,    Title = "UseHalf" });
            sizeChoices.Add(new SizePreferenceString() { Preference = ViewSizePreference.UseLess,    Title = "UseLess" });
            sizeChoices.Add(new SizePreferenceString() { Preference = ViewSizePreference.UseMinimum, Title = "UseMinimum" });
            sizeChoices.Add(new SizePreferenceString() { Preference = ViewSizePreference.UseMore,    Title = "UseMore" });

            return sizeChoices;
        }

        private async void CreateView_Click(object sender, RoutedEventArgs e)
        {
            // Set up the secondary view, but don't show it yet
            ViewLifetimeControl viewControl = null;
            await CoreApplication.CreateNewView().Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // This object is used to keep track of the views and important
                // details about the contents of those views across threads
                // In your app, you would probably want to track information
                // like the open document or page inside that window
                viewControl = ViewLifetimeControl.CreateForCurrentView();
                viewControl.Title = DEFAULT_TITLE;
                // Increment the ref count because we just created the view and we have a reference to it                
                viewControl.StartViewInUse();

                var frame = new Frame();
                frame.Navigate(typeof(SecondaryViewPage), viewControl);
                Window.Current.Content = frame;
                // This is a change from 8.1: In order for the view to be displayed later it needs to be activated.
                Window.Current.Activate();
                ApplicationView.GetForCurrentView().Title = viewControl.Title;
            });

            // Be careful! This collection is bound to the current thread,
            // so make sure to update it only from this thread
            ((App)App.Current).SecondaryViews.Add(viewControl);
        }

        private async void ShowAsStandalone_Click(object sender, RoutedEventArgs e)
        {
            var selectedView = ViewChooser.SelectedItem as ViewLifetimeControl;
            var sizePreference = SizePreferenceChooser.SelectedItem as SizePreferenceString;
            var anchorSizePreference = AnchorSizePreferenceChooser.SelectedItem as SizePreferenceString;

            if (selectedView != null && sizePreference != null && anchorSizePreference != null)
            {
                try
                {
                    // Prevent the view from closing while
                    // switching to it
                    selectedView.StartViewInUse();

                    // Show the previously created secondary view, using the size
                    // preferences the user specified. In your app, you should
                    // choose a size that's best for your scenario and code it,
                    // instead of requiring the user to decide.
                    var viewShown = await ApplicationViewSwitcher.TryShowAsStandaloneAsync(
                        selectedView.Id,
                        sizePreference.Preference,
                        ApplicationView.GetForCurrentView().Id,
                        anchorSizePreference.Preference);

                    if (!viewShown)
                    {
                        // The window wasn't actually shown, so release the reference to it
                        // This may trigger the window to be destroyed
                        rootPage.NotifyUser("The view was not shown. Make sure it has focus", NotifyType.ErrorMessage);
                    }

                    // Signal that switching has completed and let the view close
                    selectedView.StopViewInUse();
                }
                catch (InvalidOperationException)
                {
                    // The view could be in the process of closing, and
                    // this thread just hasn't updated. As part of being closed,
                    // this thread will be informed to clean up its list of
                    // views (see SecondaryViewPage.xaml.cs)
                }
            }
            else
            {
                rootPage.NotifyUser("Please choose a view to show, a size preference for each view", NotifyType.ErrorMessage);
            }
        }
    }
}
