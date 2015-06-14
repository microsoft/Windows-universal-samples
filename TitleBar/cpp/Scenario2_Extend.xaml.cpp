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
#include "Scenario2_Extend.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_Extend::Scenario2_Extend()
{
    InitializeComponent();
    ExtendView->IsChecked = CoreApplication::GetCurrentView()->TitleBar->ExtendViewIntoTitleBar;
    EnableControls->Visibility = ExtendView->IsChecked->Value ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
    EnableControls->IsChecked = rootPage->AreControlsInTitleBar;
}

void Scenario2_Extend::OnNavigatedTo(NavigationEventArgs^ args)
{
    // The SizeChanged event is raised when full screen mode changes.
    windowSizeChangedToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &Scenario2_Extend::OnWindowSizeChanged);
    UpdateFullScreenModeStatus();
}

void Scenario2_Extend::OnNavigatedFrom(NavigationEventArgs^ e)
{
    Window::Current->SizeChanged -= windowSizeChangedToken;
}

void Scenario2_Extend::ExtendView_Click(Object^ sender, RoutedEventArgs^ e)
{
    bool extend = ExtendView->IsChecked->Value;
    CoreApplication::GetCurrentView()->TitleBar->ExtendViewIntoTitleBar = extend;
    EnableControls->Visibility = extend ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
    if (extend)
    {
        rootPage->AddCustomTitleBar();
    }
    else
    {
        rootPage->RemoveCustomTitleBar();
    }
}

void Scenario2_Extend::EnableControls_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->AreControlsInTitleBar = EnableControls->IsChecked->Value;
}

void Scenario2_Extend::ToggleFullScreenModeButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    ApplicationView^ view = ApplicationView::GetForCurrentView();
    if (view->IsFullScreenMode)
    {
        view->ExitFullScreenMode();
        rootPage->NotifyUser("Exited full screen mode", NotifyType::StatusMessage);
    }
    else
    {
        if (view->TryEnterFullScreenMode())
        {
            rootPage->NotifyUser("Entered full screen mode", NotifyType::StatusMessage);
        }
        else
        {
            rootPage->NotifyUser("Failed to enter full screen mode", NotifyType::ErrorMessage);
        }
    }
}

void Scenario2_Extend::OnWindowSizeChanged(Object^ sender, WindowSizeChangedEventArgs^ args)
{
    UpdateFullScreenModeStatus();
}

void Scenario2_Extend::UpdateFullScreenModeStatus()
{
    ApplicationView^ view = ApplicationView::GetForCurrentView();
    bool isFullScreenMode = view->IsFullScreenMode;
    ToggleFullScreenModeSymbol->Symbol = isFullScreenMode ? Symbol::BackToWindow : Symbol::FullScreen;
    ReportFullScreenMode->Text = isFullScreenMode ? "is in" : "is not in";
}
