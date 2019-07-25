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

#include "pch.h"
#include "SecondaryViewPage.h"
#include "SecondaryViewPage.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::UI::Core::AnimationMetrics;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;

namespace
{
    const int ANIMATION_TRANSLATION_START = 100;
    const int ANIMATION_TRANSLATION_END = 0;
    const int ANIMATION_OPACITY_START = 0;
    const int ANIMATION_OPACITY_END = 1;
    hstring EMPTY_TITLE = L"<title cleared>";
}

namespace winrt::SDKTemplate::implementation
{
    SecondaryViewPage::SecondaryViewPage()
    {
        InitializeComponent();
        m_enterAnimation = CreateEnterAnimation();
    }

    void SecondaryViewPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        m_thisViewControl = e.Parameter().as<SDKTemplate::ViewLifetimeControl>();

        // When this view is finally released, clean up state
        m_releasedToken = m_thisViewControl.Released({ get_weak(), &SecondaryViewPage::View_Released });
    }

    fire_and_forget SecondaryViewPage::SwitchAndAnimate(int32_t fromViewId)
    {
        auto lifetime = get_strong();

        // This continues the flow from Scenario 3
        m_thisViewControl.StartViewInUse();

        // Before switching, make this view match the outgoing window
        // (go to a blank background)
        m_enterAnimation.Begin();
        m_enterAnimation.Pause();
        m_enterAnimation.Seek(TimeSpan::zero());

        // Bring this view onto screen. Since the two view are drawing
        // the same visual, the user will not be able to perceive the switch
        co_await ApplicationViewSwitcher::SwitchAsync(
            ApplicationView::GetForCurrentView().Id(),
            fromViewId,
            ApplicationViewSwitchingOptions::SkipAnimation);

        // Now that this window is on screen, animate in its contents
        m_enterAnimation.Begin();
        m_thisViewControl.StopViewInUse();
    }

    void SecondaryViewPage::SetTitle_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Set a title for the window. This title is visible
         // in system switchers
        SetTitle(TitleBox().Text());
    }

    void SecondaryViewPage::ClearTitle_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Clear the title by setting it to blank
        SetTitle(L"");
        TitleBox().Text(L"");
    }

    fire_and_forget SecondaryViewPage::SetTitle(hstring newTitle)
    {
        auto lifetime = get_strong();

        auto thisViewId = ApplicationView::GetForCurrentView().Id();
        ApplicationView::GetForCurrentView().Title(newTitle);
        m_thisViewControl.StartViewInUse();

        // The title contained in the ViewLifetimeControl object is bound to
        // UI elements on the main thread. So, updating the title
        // in this object must be done on the main thread
        co_await resume_foreground(SampleState::MainDispatcher);

        // We are now in the main window.

        // Setting the title on ApplicationView to blank will clear the title in
        // the system switcher. It would be good to still have a title in the app's UI.
        if (newTitle.empty())
        {
            newTitle = EMPTY_TITLE;
        }

        SampleState::UpdateTitle(newTitle, thisViewId);
        m_thisViewControl.StopViewInUse();
    }

    fire_and_forget SecondaryViewPage::View_Released(SDKTemplate::ViewLifetimeControl const& sender, IInspectable const&)
    {
        auto lifetime = get_strong();

        sender.Released(m_releasedToken);

        auto thisDispatcher = Window::Current().Dispatcher();

        // The ViewLifetimeControl object is bound to U.I elements on the main thread
        // So, the object must be removed from that thread
        co_await resume_foreground(SampleState::MainDispatcher);
        
        // We are now in the main window.

        for (unsigned int i = 0; i < SampleState::SecondaryViews.Size(); i++)
        {
            auto viewData = SampleState::SecondaryViews.GetAt(i).as<ViewLifetimeControl>();
            if (m_thisViewControl.Id() == viewData.Id())
            {
                SampleState::SecondaryViews.RemoveAt(i);

                co_await resume_foreground(thisDispatcher);

                // We are now in the secondary window.

                // It's important to make sure no work is scheduled on this thread
                // after it starts to close (no data binding changes, no changes to
                // XAML, creating new objects in destructors, etc.) since
                // that will throw exceptions
                Window::Current().Close();
                co_return;
            }
        }

        throw hresult_error(HRESULT_FROM_WIN32(ERROR_NOT_FOUND), L"ViewID not found in collection");
    }

    void SecondaryViewPage::HandleProtocolLaunch(Uri const& uri)
    {
        // This code should only get executed if DisableSystemActivationPolicy
        // has not been called.
        ProtocolText().Visibility(Visibility::Visible);
        ProtocolText().Text(uri.AbsoluteUri());
    }

    fire_and_forget SecondaryViewPage::ProtocolLaunch_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Used with Scenario 2
        m_thisViewControl.StartViewInUse();
        co_await Launcher::LaunchUriAsync(Uri(L"multiple-view-sample://basiclaunch/"));
        m_thisViewControl.StopViewInUse();
    }

    fire_and_forget SecondaryViewPage::GoToMain_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Switch to the main view without explicitly requesting
        // that this view be hidden
        m_thisViewControl.StartViewInUse();
        co_await ApplicationViewSwitcher::SwitchAsync(SampleState::MainViewId);
        m_thisViewControl.StopViewInUse();
    }

    fire_and_forget SecondaryViewPage::HideView_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        m_thisViewControl.StartViewInUse();
        co_await ApplicationViewSwitcher::SwitchAsync(
            SampleState::MainViewId,
            ApplicationView::GetForCurrentView().Id(),
            ApplicationViewSwitchingOptions::ConsolidateViews);

        m_thisViewControl.StopViewInUse();
    }


    Storyboard SecondaryViewPage::CreateEnterAnimation()
    {
        Storyboard enterAnimation;
        Storyboard::SetTarget(enterAnimation, LayoutRoot());

        AnimationDescription ad(AnimationEffect::EnterPage, AnimationEffectTarget::Primary);
        for (unsigned int i = 0; i < LayoutRoot().Children().Size(); i++)
        {
            // Add a render transform to the existing one just for animations
            auto element = LayoutRoot().Children().GetAt(i);
            TransformGroup tg;
            tg.Children().Append(TranslateTransform());
            tg.Children().Append(element.RenderTransform());
            element.RenderTransform(tg);

            // Calculate the stagger for each animation. Note that this has a max
            TimeSpan delay = std::min(std::chrono::duration_cast<TimeSpan>(ad.StaggerDelay() * i * ad.StaggerDelayFactor()), ad.DelayLimit());

            for (auto description : ad.Animations())
            {
                DoubleAnimationUsingKeyFrames animation;

                // Start the animation at the right offset
                SplineDoubleKeyFrame startSpline;
                startSpline.KeyTime(KeyTimeHelper::FromTimeSpan(TimeSpan::zero()));
                Storyboard::SetTarget(animation, element);

                // Hold at that offset until the stagger delay is hit
                SplineDoubleKeyFrame middleSpline;
                middleSpline.KeyTime(KeyTimeHelper::FromTimeSpan(delay));

                // Animation from delayed time to last time
                SplineDoubleKeyFrame endSpline;
                KeySpline endSplineKey;
                endSplineKey.ControlPoint1(description.Control1());
                endSplineKey.ControlPoint2(description.Control2());
                endSpline.KeySpline(endSplineKey);
                TimeSpan endKeyTime = description.Duration() + delay;
                endSpline.KeyTime(KeyTimeHelper::FromTimeSpan(endKeyTime));

                // Do the translation
                if (description.Type() == PropertyAnimationType::Translation)
                {
                    startSpline.Value(ANIMATION_TRANSLATION_START);
                    middleSpline.Value(ANIMATION_TRANSLATION_START);
                    endSpline.Value(ANIMATION_TRANSLATION_END);

                    Storyboard::SetTargetProperty(animation, L"(UIElement.RenderTransform).(TransformGroup.Children)[0].X");
                }
                // Opacity
                else if (description.Type() == PropertyAnimationType::Opacity)
                {
                    startSpline.Value(ANIMATION_OPACITY_START);
                    middleSpline.Value(ANIMATION_OPACITY_START);
                    endSpline.Value(ANIMATION_OPACITY_END);

                    Storyboard::SetTargetProperty(animation, L"Opacity");
                }
                else
                {
                    throw hresult_invalid_argument(L"Encountered an unexpected animation type.");
                }

                // Put the final animation together
                animation.KeyFrames().Append(startSpline);
                animation.KeyFrames().Append(middleSpline);
                animation.KeyFrames().Append(endSpline);
                enterAnimation.Children().Append(animation);
            }
        }

        return enterAnimation;
    }
}
