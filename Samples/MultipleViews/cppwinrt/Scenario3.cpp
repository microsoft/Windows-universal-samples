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
#include "Scenario3.h"
#include "Scenario3.g.cpp"
#include "SampleConfiguration.h"
#include "SecondaryViewPage.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;
using namespace std::chrono_literals;

namespace
{
    handle duplicate(handle const& other)
    {
        handle result;
        if (other)
        {
            check_bool(DuplicateHandle(GetCurrentProcess(), other.get(), GetCurrentProcess(), result.put(), 0, FALSE, DUPLICATE_SAME_ACCESS));
        }
        return result;
    }
}

namespace winrt::SDKTemplate::implementation
{
    Scenario3::Scenario3()
    {
        InitializeComponent();

        ViewChooser().ItemsSource(SampleState::SecondaryViews);

        FadeOutThemeAnimation fadeOut;
        fadeOutStoryboard.Children().Append(fadeOut);

        // Normally you can point directly to an object named in your XAML. Since
        // the sample hosts multiple pages, it's convenient for this scenario to
        // get the root element
        Storyboard::SetTarget(fadeOut, rootPage.Content().as<FrameworkElement>().FindName(L"Splitter").as<DependencyObject>());
    }

    void Scenario3::CompleteCurrentAnimation()
    {
        // Must be called from the UI thread to avoid race conditions.
        if (animationCompleteEvent)
        {
            SetEvent(animationCompleteEvent.get());
            animationCompleteEvent.close();
        }
    }

    void Scenario3::Fade_Completed(IInspectable const&, IInspectable const&)
    {
        // This event always fires on the UI thread, along with Current_VisibilityChanged.
        CompleteCurrentAnimation();
    }

    void Scenario3::StartFadeOutContents()
    {
        CompleteCurrentAnimation();
        animationCompleteEvent.attach(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE));
        check_bool(static_cast<bool>(animationCompleteEvent));

        fadeOutStoryboard.Begin();
    }

    void Scenario3::OnNavigatedTo(NavigationEventArgs const&)
    {
        visibilityToken = Window::Current().VisibilityChanged({ this, &Scenario3::Current_VisibilityChanged });
        fadeOutToken = fadeOutStoryboard.Completed({ this, &Scenario3::Fade_Completed });
    }

    void Scenario3::OnNavigatedFrom(NavigationEventArgs const&)
    {
        Window::Current().VisibilityChanged(visibilityToken);
        fadeOutStoryboard.Completed(fadeOutToken);
    }

    void Scenario3::Current_VisibilityChanged(IInspectable const&, VisibilityChangedEventArgs const& e)
    {
        auto now = clock::now();
        // Timeout the animation if the secondary window fails to respond in 500
        // ms. Since this animation clears out the main view of the app, it's not desirable
        // to leave it unusable
        if (e.Visible() || (now - lastFadeOutTime) > 500ms)
        {
            // This event always fires on the UI thread, along with Fade_Completed.
            CompleteCurrentAnimation();
            fadeOutStoryboard.Stop();
        }
    }

    fire_and_forget Scenario3::AnimatedSwitch_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // The sample demonstrates a general strategy for doing custom animations when switching view
        // It's technically only possible to animate the contents of a particular view. But, it is possible
        // to animate the outgoing view to some common visual (like a blank background), have the incoming
        // view draw that same visual, switch in the incoming window (which will be imperceptible to the
        // user since both windows will be showing the same thing), then animate the contents of the incoming
        // view in from the common visual.
        auto selectedItem = ViewChooser().SelectedItem().as<ViewLifetimeControl>();
        if (selectedItem == nullptr)
        {
            rootPage.NotifyUser(L"Select a window to see a switch animation. You can create a window in scenario 1", NotifyType::ErrorMessage);
            co_return;
        }

        try
        {
            // Prevent the view from being closed while switching to it
            selectedItem.StartViewInUse();

            // Signal that the window is about to be switched to
            // If the view is already shown to the user, then the app
            // shouldn't run any extra animations
            auto currentId = ApplicationView::GetForCurrentView().Id();

            bool useFallback = false;
            bool isViewVisible = false;
            try
            {
                isViewVisible = co_await ApplicationViewSwitcher::PrepareForCustomAnimatedSwitchAsync(
                    selectedItem.Id(),
                    currentId,
                    ApplicationViewSwitchingOptions::SkipAnimation);
            }
            catch (hresult_not_implemented&)
            {
                // The device family does not support PrepareForCustomAnimatedSwitchAsync.
                // Fall back to switching with standard animation.
                useFallback = true;
            }

            if (useFallback)
            {
                isViewVisible = true;
                co_await ApplicationViewSwitcher::SwitchAsync(selectedItem.Id(), currentId);
            }

            // The view may already be on screen, in which case there's no need to animate its
            // contents (or animate out the contents of the current window).
            if (!isViewVisible)
            {
                // The view isn't visible, so animate it on screen
                lastFadeOutTime = clock::now();

                // Make the current window totally blank. The incoming window is
                // going to be totally blank as well when the two windows switch,
                StartFadeOutContents();

                // Wait for the animation to complete. Make a duplicate of animationCompleteEvent
                // because it will be closed when the animation completes.
                handle localEvent = duplicate(animationCompleteEvent);
                co_await resume_on_signal(localEvent.get());

                // Once the current view is blank, switch in the other window
                co_await resume_foreground(selectedItem.Dispatcher());

                // We are now in the switched-to window.

                // More details are in SecondaryViewPage.cpp
                // This function makes the view totally blank, swaps the two view (which
                // is not apparent to the user since both windows are blank), then animates
                // in the content of newly visible view
                auto currentPage = Window::Current().Content().as<Controls::Frame>().Content().as<SDKTemplate::SecondaryViewPage>();
                currentPage.SwitchAndAnimate(currentId);

                selectedItem.StopViewInUse();
            }
        }
        catch (hresult_error& ex)
        {
            if (ex.to_abi() == RO_E_CLOSED)
            {
                // The view could be in the process of closing, and
                // this thread just hasn't updated. As part of being closed,
                // this thread will be informed to clean up its list of
                // views (see SecondaryViewPage.cpp)
            }
            else
            {
                throw;
            }
        }
    }
}
