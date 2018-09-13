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
#include "Scenario1_Basic.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

Scenario1_Basic::Scenario1_Basic() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario1_Basic::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    windowResizeToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &Scenario1_Basic::OnWindowResize);
    keyDownToken = rootPage->KeyDown += ref new KeyEventHandler(this, &Scenario1_Basic::OnKeyDown);
    UpdateContent();
}

void Scenario1_Basic::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    Window::Current->SizeChanged -= windowResizeToken;
    rootPage->KeyDown -= keyDownToken;
}

void Scenario1_Basic::ToggleFullScreenModeButton_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    ApplicationView^ view = ApplicationView::GetForCurrentView();
    if (view->IsFullScreenMode)
    {
        view->ExitFullScreenMode();
        rootPage->NotifyUser("Exiting full screen mode", NotifyType::StatusMessage);
        isLastKnownFullScreen = false;
        // The SizeChanged event will be raised when the exit from full screen mode is complete.
    }
    else
    {
        if (view->TryEnterFullScreenMode())
        {
            rootPage->NotifyUser("Entering full screen mode", NotifyType::StatusMessage);
            isLastKnownFullScreen = true;
            // The SizeChanged event will be raised when the entry to full screen mode is complete.
        }
        else
        {
            rootPage->NotifyUser("Failed to enter full screen mode", NotifyType::ErrorMessage);
        }
    }
}

void Scenario1_Basic::ShowStandardSystemOverlaysButton_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    ApplicationView^ view = ApplicationView::GetForCurrentView();
    view->ShowStandardSystemOverlays();
}

void Scenario1_Basic::UseMinimalOverlaysCheckBox_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    ApplicationView^ view = ApplicationView::GetForCurrentView();
    view->FullScreenSystemOverlayMode = UseMinimalOverlaysCheckBox->IsChecked->Value ? FullScreenSystemOverlayMode::Minimal : FullScreenSystemOverlayMode::Standard;
}

void Scenario1_Basic::OnWindowResize(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e)
{
    UpdateContent();
}

void Scenario1_Basic::UpdateContent()
{
    ApplicationView^ view = ApplicationView::GetForCurrentView();
    bool isFullScreenMode = view->IsFullScreenMode;
    ToggleFullScreenModeSymbol->Symbol = isFullScreenMode ? Symbol::BackToWindow : Symbol::FullScreen;
    ReportFullScreenMode->Text = isFullScreenMode ? "is in" : "is not in";
    FullScreenOptionsPanel->Visibility = isFullScreenMode ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;

    // Did the system force a change in full screen mode?
    if (isLastKnownFullScreen != isFullScreenMode)
    {
        isLastKnownFullScreen = isFullScreenMode;
        // Clear any stray messages that talked about the mode we are no longer in.
        rootPage->NotifyUser("", NotifyType::StatusMessage);
    }
}

void Scenario1_Basic::OnKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
    if (e->Key == Windows::System::VirtualKey::Escape)
    {
        ApplicationView^ view = ApplicationView::GetForCurrentView();
        if (view->IsFullScreenMode)
        {
            view->ExitFullScreenMode();
            rootPage->NotifyUser("Exited full screen mode due to keypress", NotifyType::StatusMessage);
            isLastKnownFullScreen = false;
        }
    }
}
