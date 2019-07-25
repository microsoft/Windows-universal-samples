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
#include "Scenario1_Basic.h"
#include "Scenario1_Basic.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Basic::Scenario1_Basic()
    {
        InitializeComponent();
    }

    void Scenario1_Basic::OnNavigatedTo(NavigationEventArgs const&)
    {
        windowResizeToken = Window::Current().SizeChanged([this](auto&&, auto&&)
        {
            UpdateContent();
        });
        keyDownToken = rootPage.KeyDown({ this, &Scenario1_Basic::RootPage_OnKeyDown });

        UpdateContent();
    }

    void Scenario1_Basic::OnNavigatedFrom(NavigationEventArgs const&)
    {
        Window::Current().SizeChanged(windowResizeToken);
        rootPage.KeyDown(keyDownToken);
    }

    void Scenario1_Basic::ToggleFullScreenModeButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ApplicationView view = ApplicationView::GetForCurrentView();
        if (view.IsFullScreenMode())
        {
            view.ExitFullScreenMode();
            rootPage.NotifyUser(L"Exiting full screen mode", NotifyType::StatusMessage);
            isLastKnownFullScreen = false;
            // The SizeChanged event will be raised when the exit from full screen mode is complete.
        }
        else
        {
            if (view.TryEnterFullScreenMode())
            {
                rootPage.NotifyUser(L"Entering full screen mode", NotifyType::StatusMessage);
                isLastKnownFullScreen = true;
                // The SizeChanged event will be raised when the entry to full screen mode is complete.
            }
            else
            {
                rootPage.NotifyUser(L"Failed to enter full screen mode", NotifyType::ErrorMessage);
            }
        }
    }

    void Scenario1_Basic::ShowStandardSystemOverlaysButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ApplicationView view = ApplicationView::GetForCurrentView();
        view.ShowStandardSystemOverlays();
    }

    void Scenario1_Basic::UseMinimalOverlaysCheckBox_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ApplicationView view = ApplicationView::GetForCurrentView();
        view.FullScreenSystemOverlayMode(UseMinimalOverlaysCheckBox().IsChecked().Value() ? FullScreenSystemOverlayMode::Minimal : FullScreenSystemOverlayMode::Standard);
    }

    void Scenario1_Basic::UpdateContent()
    {
        ApplicationView view = ApplicationView::GetForCurrentView();
        bool isFullScreenMode = view.IsFullScreenMode();
        ToggleFullScreenModeSymbol().Symbol(isFullScreenMode ? Symbol::BackToWindow : Symbol::FullScreen);
        ReportFullScreenMode().Text(isFullScreenMode ? L"is in" : L"is not in");
        FullScreenOptionsPanel().Visibility(isFullScreenMode ? Visibility::Visible : Visibility::Collapsed);

        // Did the system force a change in full screen mode?
        if (isLastKnownFullScreen != isFullScreenMode)
        {
            isLastKnownFullScreen = isFullScreenMode;
            // Clear any stray messages that talked about the mode we are no longer in.
            rootPage.NotifyUser(L"", NotifyType::StatusMessage);
        }
    }

    void Scenario1_Basic::RootPage_OnKeyDown(IInspectable const&, KeyRoutedEventArgs const& e)
    {
        if (e.Key() == Windows::System::VirtualKey::Escape)
        {
            ApplicationView view = ApplicationView::GetForCurrentView();
            if (view.IsFullScreenMode())
            {
                view.ExitFullScreenMode();
                rootPage.NotifyUser(L"Exited full screen mode due to keypress", NotifyType::StatusMessage);
                isLastKnownFullScreen = false;
            }
        }
    }
}
