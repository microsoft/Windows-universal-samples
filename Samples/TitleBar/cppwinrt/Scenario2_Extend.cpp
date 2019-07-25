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
#include "Scenario2_Extend.h"
#include "Scenario2_Extend.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Extend::Scenario2_Extend()
    {
        InitializeComponent();

        bool isExtendedIntoTitleBar = CoreApplication::GetCurrentView().TitleBar().ExtendViewIntoTitleBar();
        ExtendView().IsChecked(isExtendedIntoTitleBar);
        EnableControls().Visibility(isExtendedIntoTitleBar ? Visibility::Visible : Visibility::Collapsed);
        EnableControls().IsChecked(SampleState::AreControlsInTitleBar());
    }

    void Scenario2_Extend::OnNavigatedTo(NavigationEventArgs const&)
    {
        windowSizeChangedToken = Window::Current().SizeChanged([this](auto&&, auto&&)
        {
            UpdateFullScreenModeStatus();
        });
        UpdateFullScreenModeStatus();
    }

    void Scenario2_Extend::OnNavigatedFrom(NavigationEventArgs const&)
    {
        Window::Current().SizeChanged(windowSizeChangedToken);
    }

    void Scenario2_Extend::ExtendView_Click(IInspectable const&, RoutedEventArgs const&)
    {
        bool extend = ExtendView().IsChecked().Value();
        CoreApplication::GetCurrentView().TitleBar().ExtendViewIntoTitleBar(extend);
        EnableControls().Visibility(extend ? Visibility::Visible : Visibility::Collapsed);
        if (extend)
        {
            SampleState::AddCustomTitleBar();
        }
        else
        {
            SampleState::RemoveCustomTitleBar();
        }
    }

    void Scenario2_Extend::ToggleFullScreenModeButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ApplicationView view = ApplicationView::GetForCurrentView();
        if (view.IsFullScreenMode())
        {
            view.ExitFullScreenMode();
            rootPage.NotifyUser(L"Exited full screen mode", NotifyType::StatusMessage);
        }
        else
        {
            if (view.TryEnterFullScreenMode())
            {
                rootPage.NotifyUser(L"Entered full screen mode", NotifyType::StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(L"Failed to enter full screen mode", NotifyType::ErrorMessage);
            }
        }
    }

    void Scenario2_Extend::EnableControls_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SampleState::AreControlsInTitleBar(EnableControls().IsChecked().Value());
    }

    void Scenario2_Extend::UpdateFullScreenModeStatus()
    {
        bool isFullScreenMode = ApplicationView::GetForCurrentView().IsFullScreenMode();
        ToggleFullScreenModeSymbol().Symbol(isFullScreenMode ? Symbol::BackToWindow : Symbol::FullScreen);
        ReportFullScreenMode().Text(isFullScreenMode ? L"is in" : L"is not in");
    }
}
