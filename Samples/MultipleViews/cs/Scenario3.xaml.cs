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
using System.Threading.Tasks;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        Storyboard fadeOutStoryboard = new Storyboard();
        DateTime lastFadeOutTime = DateTime.Now;
        TaskCompletionSource<bool> animationTask;
        const int TIMEOUT_LENGTH = 500;

        public Scenario3()
        {
            this.InitializeComponent();
            ViewChooser.ItemsSource = ((App)App.Current).SecondaryViews;

            var fadeOut = new FadeOutThemeAnimation();
            fadeOutStoryboard.Children.Add(fadeOut);

            // Normally you can point directly to an object named in your XAML. Since
            // the sample hosts multiple pages, it's convenient for this scenario to
            // get the root element
            Storyboard.SetTarget(fadeOut, (DependencyObject) ((FrameworkElement)rootPage.Content).FindName("Splitter"));
        }

        void Fade_Completed(object sender, object e)
        {
            // This event always fires on the UI thread, along with Current_VisibilityChanged,
            // so there is no race condition with the two methods both changing this
            // value
            if (animationTask != null)
            {
                animationTask.TrySetResult(true);
            }
            animationTask = null;
        }

        Task<bool> FadeOutContents()
        {
            if (animationTask != null)
            {
                animationTask.TrySetCanceled();
            }
            animationTask = new TaskCompletionSource<bool>();
            fadeOutStoryboard.Begin();
            return animationTask.Task;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            Window.Current.VisibilityChanged += Current_VisibilityChanged;
            fadeOutStoryboard.Completed += Fade_Completed;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            Window.Current.VisibilityChanged -= Current_VisibilityChanged;
            fadeOutStoryboard.Completed -= Fade_Completed;
        }

        void Current_VisibilityChanged(object sender, VisibilityChangedEventArgs e)
        {
            // Timeout the animation if the secondary window fails to respond in 500
            // ms. Since this animation clears out the main view of the app, it's not desirable
            // to leave it unusable
            if (e.Visible || (DateTime.Now - lastFadeOutTime).TotalMilliseconds > TIMEOUT_LENGTH)
            {

                // This event always fires on the UI thread, along with Fade_Completed,
                // so there is no race condition with the two methods both changing this
                // value
                if (animationTask != null)
                {
                    animationTask.TrySetCanceled();
                    animationTask = null;
                }
                fadeOutStoryboard.Stop();
            }
        }

        private async void AnimatedSwitch_Click(object sender, RoutedEventArgs e)
        {
            // The sample demonstrates a general strategy for doing custom animations when switching view
            // It's technically only possible to animate the contents of a particular view. But, it is possible
            // to animate the outgoing view to some common visual (like a blank background), have the incoming
            // view draw that same visual, switch in the incoming window (which will be imperceptible to the
            // user since both windows will be showing the same thing), then animate the contents of the incoming
            // view in from the common visual.
            var selectedItem = ViewChooser.SelectedItem as ViewLifetimeControl;
            if (selectedItem != null)
            {
                try
                {
                    // Prevent the view from being closed while switching to it
                    selectedItem.StartViewInUse();

                    // Signal that the window is about to be switched to
                    // If the view is already shown to the user, then the app
                    // shouldn't run any extra animations
                    var currentId = ApplicationView.GetForCurrentView().Id;

                    bool isViewVisible;
                    try
                    {
                        isViewVisible = await ApplicationViewSwitcher.PrepareForCustomAnimatedSwitchAsync(
                            selectedItem.Id,
                            currentId,
                            ApplicationViewSwitchingOptions.SkipAnimation);
                    }
                    catch (NotImplementedException)
                    {
                        // The device family does not support PrepareForCustomAnimatedSwitchAsync.
                        // Fall back to switching with standard animation.
                        isViewVisible = true;
                        await ApplicationViewSwitcher.SwitchAsync(selectedItem.Id, currentId);
                    }

                    // The view may already be on screen, in which case there's no need to animate its
                    // contents (or animate out the contents of the current window)
                    if (!isViewVisible)
                    {
                        // The view isn't visible, so animate it on screen
                        lastFadeOutTime = DateTime.Now;

                        // Make the current view totally blank. The incoming view is also
                        // going to be totally blank when the two windows switch
                        await FadeOutContents();
                        // Once the current view is blank, switch in the other window
                        await selectedItem.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                        {
                            var currentPage = (SecondaryViewPage)((Frame)Window.Current.Content).Content;
                            // More details are in SecondaryViewPage.xaml.cs
                            // This function makes the view totally blank, swaps the two view (which
                            // is not apparent to the user since both windows are blank), then animates
                            // in the content of newly visible view
                            currentPage.SwitchAndAnimate(currentId);
                        });
                    }
                    selectedItem.StopViewInUse();
                }
                catch (InvalidOperationException)
                {
                    // The view could be in the process of closing, and
                    // this thread just hasn't updated. As part of being closed,
                    // this thread will be informed to clean up its list of
                    // views (see SecondaryViewPage.xaml.cs)
                }
                catch (TaskCanceledException)
                {
                    rootPage.NotifyUser("The animation was canceled. It may have timed out", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Select a window to see a switch animation. You can create a window in scenario 1", NotifyType.ErrorMessage);
            }
        }
    }
}
