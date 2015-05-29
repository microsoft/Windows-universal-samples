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
    windowSizeChangedToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &Scenario1_Basic::OnWindowSizeChanged);
    UpdateContent();
}

void Scenario1_Basic::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    Window::Current->SizeChanged -= windowSizeChangedToken;
}

void Scenario1_Basic::ResizeView_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    ApplicationView^ view = ApplicationView::GetForCurrentView();
    if (view->TryResizeView(Size(600, 500)))
    {
        rootPage->NotifyUser("Resizing to 600 \u00D7 500", NotifyType::StatusMessage);
        // The SizeChanged event will be raised when the resize is complete.
    }
    else
    {
        // The system does not support resizing, or the provided size is out of range.
        rootPage->NotifyUser("Failed to resize view", NotifyType::ErrorMessage);
    }
}

void Scenario1_Basic::SetMinimumSize_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    ApplicationView^ view = ApplicationView::GetForCurrentView();
    if (SetMinimumSize->IsChecked->Value)
    {
        // If this size is not permitted by the system, the nearest permitted value is used.
        view->SetPreferredMinSize(Size(300, 200));
    }
    else
    {
        // Passing width = height = 0 returns to the default system behavior.
        view->SetPreferredMinSize(Size());
    }
}

void Scenario1_Basic::OnWindowSizeChanged(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e)
{
    UpdateContent();
}

void Scenario1_Basic::UpdateContent()
{
    Rect bounds = Window::Current->Bounds;
    ReportViewWidth->Text = bounds.Width.ToString();
    ReportViewHeight->Text = bounds.Height.ToString();
}
