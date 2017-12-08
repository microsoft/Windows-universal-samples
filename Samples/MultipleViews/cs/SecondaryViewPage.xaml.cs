//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using SDKTemplate;
using SecondaryViewsHelpers;
using System;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.Core.AnimationMetrics;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    // This page is shown in secondary views created by this app.
    // See Scenario 1 for details on how to create a secondary view
    public sealed partial class SecondaryViewPage : Page
    {
        const int ANIMATION_TRANSLATION_START = 100;
        const int ANIMATION_TRANSLATION_END = 0;
        const int ANIMATION_OPACITY_START = 0;
        const int ANIMATION_OPACITY_END = 1;

        const string EMPTY_TITLE = "<title cleared>";

        ViewLifetimeControl thisViewControl;
        int mainViewId;
        CoreDispatcher mainDispatcher;

        public SecondaryViewPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            thisViewControl = (ViewLifetimeControl) e.Parameter;
            mainViewId = ((App)App.Current).MainViewId;
            mainDispatcher = ((App)App.Current).MainDispatcher;

            // When this view is finally release, clean up state
            thisViewControl.Released += ViewLifetimeControl_Released;
        }

        private async void GoToMain_Click(object sender, RoutedEventArgs e)
        {
            // Switch to the main view without explicitly requesting
            // that this view be hidden
            thisViewControl.StartViewInUse();
            await ApplicationViewSwitcher.SwitchAsync(mainViewId);
            thisViewControl.StopViewInUse();
        }

        private async void HideView_Click(object sender, RoutedEventArgs e)
        {
            // Switch to main and hide this view entirely from the user
            thisViewControl.StartViewInUse();
            await ApplicationViewSwitcher.SwitchAsync(mainViewId,
                ApplicationView.GetForCurrentView().Id,
                ApplicationViewSwitchingOptions.ConsolidateViews);
            thisViewControl.StopViewInUse();
        }

        private void SetTitle_Click(object sender, RoutedEventArgs e)
        {
            // Set a title for the window. This title is visible
            // in system switchers
            SetTitle(TitleBox.Text);
        }

        private void ClearTitle_Click(object sender, RoutedEventArgs e)
        {
            // Clear the title by setting it to blank
            SetTitle("");
            TitleBox.Text = "";
        }

        private async void SetTitle(string newTitle)
        {
            var thisViewId = ApplicationView.GetForCurrentView().Id;
            ApplicationView.GetForCurrentView().Title = newTitle;
            thisViewControl.StartViewInUse();

            // The title contained in the ViewLifetimeControl object is bound to
            // UI elements on the main thread. So, updating the title
            // in this object must be done on the main thread
            await mainDispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Setting the title on ApplicationView to blank will clear the title in
                // the system switcher. It would be good to still have a title in the app's UI.
                if (newTitle == "")
                {
                    newTitle = EMPTY_TITLE;
                }

                ((App)App.Current).UpdateTitle(newTitle, thisViewId);
            });
            thisViewControl.StopViewInUse();
        }

        private async void ViewLifetimeControl_Released(Object sender, EventArgs e)
        {
            ((ViewLifetimeControl)sender).Released -= ViewLifetimeControl_Released;
            // The ViewLifetimeControl object is bound to UI elements on the main thread
            // So, the object must be removed from that thread
            await mainDispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                ((App)App.Current).SecondaryViews.Remove(thisViewControl);
            });

            // The released event is fired on the thread of the window
            // it pertains to.
            //
            // It's important to make sure no work is scheduled on this thread
            // after it starts to close (no data binding changes, no changes to
            // XAML, creating new objects in destructors, etc.) since
            // that will throw exceptions
            Window.Current.Close();
        }

        public void HandleProtocolLaunch(Uri uri)
        {
            // This code should only get executed if DisableSystemActivationPolicy
            // has not been called.
            ProtocolText.Visibility = Visibility.Visible;
            ProtocolText.Text = uri.AbsoluteUri;
        }

        private async void ProtocolLaunch_Click(object sender, RoutedEventArgs e)
        {
            // The activation will always end up on the same page unless you 
            // create an external protocol activation.
            thisViewControl.StartViewInUse();
            await Launcher.LaunchUriAsync(new Uri("multiple-view-sample://basiclaunch/"));
            thisViewControl.StopViewInUse();
        }

        public async void SwitchAndAnimate(int fromViewId)
        {
            // This continues the flow from Scenario 3
            thisViewControl.StartViewInUse();

            // Calculate the entrance animation. Recalculate this every time,
            // because the animation description can vary (for example,
            // if the user changes accessibility settings).
            Storyboard enterAnimation = CreateEnterAnimation(LayoutRoot);

            // Before switching, make this view match the outgoing window
            // (go to a blank background)
            enterAnimation.Begin();
            enterAnimation.Pause();
            enterAnimation.Seek(TimeSpan.FromMilliseconds(0));

            // Bring this view onto screen. Since the two view are drawing
            // the same visual, the user will not be able to perceive the switch
            await ApplicationViewSwitcher.SwitchAsync(ApplicationView.GetForCurrentView().Id,
                fromViewId,
                ApplicationViewSwitchingOptions.SkipAnimation);

            // Now that this window is on screen, animate in its contents
            enterAnimation.Begin();
            thisViewControl.StopViewInUse();
        }

        private Storyboard CreateEnterAnimation(Panel layoutRoot)
        {
            var enterAnimation = new Storyboard();

            // Use the AnimationDescription object if available. Otherwise, return an empty storyboard (no animation).
            if (Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.UI.Core.AnimationMetrics.AnimationDescription"))
            {
                Storyboard.SetTarget(enterAnimation, layoutRoot);

                var ad = new AnimationDescription(AnimationEffect.EnterPage, AnimationEffectTarget.Primary);
                for (int i = 0; i < layoutRoot.Children.Count; i++)
                {
                    // Add a render transform to the existing one just for animations
                    var element = layoutRoot.Children[i];
                    var tg = new TransformGroup();
                    tg.Children.Add(new TranslateTransform());
                    tg.Children.Add(element.RenderTransform);
                    element.RenderTransform = tg;

                    // Calculate the stagger for each animation. Note that this has a max
                    var delayMs = Math.Min(ad.StaggerDelay.TotalMilliseconds * i * ad.StaggerDelayFactor, ad.DelayLimit.Milliseconds);
                    var delay = TimeSpan.FromMilliseconds(delayMs);

                    foreach (var description in ad.Animations)
                    {
                        var animation = new DoubleAnimationUsingKeyFrames();

                        // Start the animation at the right offset
                        var startSpline = new SplineDoubleKeyFrame();
                        startSpline.KeyTime = TimeSpan.FromMilliseconds(0);
                        Storyboard.SetTarget(animation, element);

                        // Hold at that offset until the stagger delay is hit
                        var middleSpline = new SplineDoubleKeyFrame();
                        middleSpline.KeyTime = delay;

                        // Animation from delayed time to last time
                        var endSpline = new SplineDoubleKeyFrame();
                        endSpline.KeySpline = new KeySpline() { ControlPoint1 = description.Control1, ControlPoint2 = description.Control2 };
                        endSpline.KeyTime = description.Duration + delay;

                        // Do the translation
                        if (description.Type == PropertyAnimationType.Translation)
                        {
                            startSpline.Value = ANIMATION_TRANSLATION_START;
                            middleSpline.Value = ANIMATION_TRANSLATION_START;
                            endSpline.Value = ANIMATION_TRANSLATION_END;

                            Storyboard.SetTargetProperty(animation, "(UIElement.RenderTransform).(TransformGroup.Children)[0].X");
                        }
                        // Opacity
                        else if (description.Type == PropertyAnimationType.Opacity)
                        {
                            startSpline.Value = ANIMATION_OPACITY_START;
                            middleSpline.Value = ANIMATION_OPACITY_START;
                            endSpline.Value = ANIMATION_OPACITY_END;

                            Storyboard.SetTargetProperty(animation, "Opacity");
                        }
                        else
                        {
                            throw new Exception("Encountered an unexpected animation type.");
                        }

                        // Put the final animation together
                        animation.KeyFrames.Add(startSpline);
                        animation.KeyFrames.Add(middleSpline);
                        animation.KeyFrames.Add(endSpline);
                        enterAnimation.Children.Add(animation);
                    }
                }
            }
            return enterAnimation;
        }
    }
}
