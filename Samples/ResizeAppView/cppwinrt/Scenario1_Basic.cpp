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
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Basic::Scenario1_Basic()
    {
        InitializeComponent();
    }

    void Scenario1_Basic::OnNavigatedTo(NavigationEventArgs const&)
    {
        windowSizeChangedToken = Window::Current().SizeChanged([this](auto&&, auto&&)
        {
            UpdateContent();
        });
        UpdateContent();
    }

    void Scenario1_Basic::OnNavigatedFrom(NavigationEventArgs const&)
    {
        Window::Current().SizeChanged(windowSizeChangedToken);
    }

    void Scenario1_Basic::ResizeView_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto view = ApplicationView::GetForCurrentView();
        if (view.TryResizeView({ 600, 500 }))
        {
            rootPage.NotifyUser(L"Resizing to 600 \u00D7 500", NotifyType::StatusMessage);
            // The SizeChanged event will be raised when the resize is complete.
        }
        else
        {
            // The system does not support resizing, or the provided size is out of range.
            rootPage.NotifyUser(L"Failed to resize view", NotifyType::ErrorMessage);
        }
    }

    void Scenario1_Basic::SetMinimumSize_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto view = ApplicationView::GetForCurrentView();
        if (SetMinimumSize().IsChecked().Value())
        {
            // If this size is not permitted by the system, the nearest permitted value is used.
            view.SetPreferredMinSize({ 300, 200 });
        }
        else
        {
            // Passing width = height = 0 returns to the default system behavior.
            view.SetPreferredMinSize({ 0, 0 });
        }
    }

    void Scenario1_Basic::UpdateContent()
    {
        Rect bounds = Window::Current().Bounds();
        ReportViewWidth().Text(to_hstring(bounds.Width));
        ReportViewHeight().Text(to_hstring(bounds.Height));
    }
}
