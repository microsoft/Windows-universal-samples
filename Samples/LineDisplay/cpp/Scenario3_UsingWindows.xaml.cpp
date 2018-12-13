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
#include "Scenario3_UsingWindows.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Navigation;

Scenario3_UsingWindows::Scenario3_UsingWindows()
{
    InitializeComponent();
    DataContext = this;
}

void Scenario3_UsingWindows::OnNavigatedTo(NavigationEventArgs^ e)
{
    InitializeAsync();
}

void Scenario3_UsingWindows::OnNavigatedFrom(NavigationEventArgs^ e)
{
    delete lineDisplay;
    lineDisplay = nullptr;
}

task<void> Scenario3_UsingWindows::InitializeAsync()
{
    lineDisplay = co_await rootPage->ClaimScenarioLineDisplayAsync();

    maxWindows = (lineDisplay == nullptr) ? 0 : lineDisplay->Capabilities->SupportedWindows;
    if (maxWindows >= 1)
    {
        // Create an entry to represent the default window.
        WindowList->Append(ref new WindowInfo(lineDisplay->DefaultWindow, nextWindowId));
        nextWindowId++;
    }
}

void Scenario3_UsingWindows::NewWindowButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    invoke_async_lambda([=]() -> task<void>
    {
        Rect viewportBounds =
        {
            Helpers::ParseUIntWithFallback(NewViewportX, -1.0f),
            Helpers::ParseUIntWithFallback(NewViewportY, -1.0f),
            Helpers::ParseUIntWithFallback(NewViewportWidth, 0.0f),
            Helpers::ParseUIntWithFallback(NewViewportHeight, 0.0f),
        };

        if ((viewportBounds.Width <= 0) || (viewportBounds.Height <= 0))
        {
            rootPage->NotifyUser("Viewport size must be be positive.", NotifyType::ErrorMessage);
            return;
        }

        // Viewport cannot extend beyond the screen.
        Size screenSizeInCharacters = lineDisplay->GetAttributes()->ScreenSizeInCharacters;
        if ((viewportBounds.X < 0) || (viewportBounds.Y < 0) ||
            (viewportBounds.X + viewportBounds.Width > screenSizeInCharacters.Width) ||
            (viewportBounds.Y + viewportBounds.Height > screenSizeInCharacters.Height))
        {
            rootPage->NotifyUser("Viewport cannot extend beyond the screen.", NotifyType::ErrorMessage);
            return;
        }

        Size windowSize =
        {
            Helpers::ParseUIntWithFallback(NewWindowWidth, 0.0f),
            Helpers::ParseUIntWithFallback(NewWindowHeight, 0.0f),
        };

        if ((windowSize.Width <= 0) || (windowSize.Height <= 0))
        {
            rootPage->NotifyUser("Window size must be be positive.", NotifyType::ErrorMessage);
            return;
        }

        // Window taller than viewport requires IsVerticalMarqueeSupported.
        if ((windowSize.Height > viewportBounds.Height) && !lineDisplay->Capabilities->IsVerticalMarqueeSupported)
        {
            rootPage->NotifyUser("Window cannot be taller than viewport on this line display.", NotifyType::ErrorMessage);
            return;
        }

        // Window wider than viewport requires IsHorizontalMarqueeSupported.
        if ((windowSize.Width > viewportBounds.Width) && !lineDisplay->Capabilities->IsHorizontalMarqueeSupported)
        {
            rootPage->NotifyUser("Window cannot be wider than viewport on this line display.", NotifyType::ErrorMessage);
            return;
        }

        // Window cannot be larger than viewport in both directions.
        // (Two-dimensional scrolling is not supported.)
        if ((windowSize.Width > viewportBounds.Width) && (windowSize.Height > viewportBounds.Height))
        {
            rootPage->NotifyUser("Window cannot be larger than viewport in both dimensions.", NotifyType::ErrorMessage);
            return;
        }

        LineDisplayWindow^ newWindow = co_await lineDisplay->TryCreateWindowAsync(viewportBounds, windowSize);

        if (newWindow != nullptr)
        {
            WindowInfo^ newWindowInfo = ref new WindowInfo(newWindow, nextWindowId);
            WindowList->Append(newWindowInfo);
            nextWindowId++;
            rootPage->NotifyUser("Created window " + newWindowInfo->Id + ".", NotifyType::StatusMessage);
        }
        else
        {
            rootPage->NotifyUser("Failed to create a new window.", NotifyType::ErrorMessage);
        }
    });
}

void Scenario3_UsingWindows::RefreshWindowButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    invoke_async_lambda([=]() -> task<void>
    {
        WindowInfo^ targetWindow = safe_cast<WindowInfo^>(WindowsListBox->SelectedItem);
        if (co_await targetWindow->Window->TryRefreshAsync())
        {
            rootPage->NotifyUser("Refreshed window " + targetWindow->Id + ".", NotifyType::StatusMessage);
        }
        else
        {
            // We probably lost our claim.
            rootPage->NotifyUser("Failed to refresh window " + targetWindow->Id + ".", NotifyType::ErrorMessage);
        }
    });
}

void Scenario3_UsingWindows::DestroyWindowButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    WindowInfo^ targetWindow = safe_cast<WindowInfo^>(WindowsListBox->SelectedItem);
    WindowList->RemoveAt(WindowsListBox->SelectedIndex);

    // The LineDisplayWindow class implements IDispose. Disposing of a LineDisplayWindow
    // results in its destruction on the line display device and invalidation of the object.
    delete targetWindow->Window;

    rootPage->NotifyUser("Destroyed window " + targetWindow->Id + ".", NotifyType::StatusMessage);
}

void Scenario3_UsingWindows::DisplayTextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    invoke_async_lambda([=]() -> task<void>
    {
        WindowInfo^ targetWindow = safe_cast<WindowInfo^>(WindowsListBox->SelectedItem);
        if (co_await targetWindow->Window->TryDisplayTextAsync(DisplayTextTextbox->Text))
        {
            rootPage->NotifyUser("Displayed \"" + DisplayTextTextbox->Text + "\" to window " + targetWindow->Id + ".", NotifyType::StatusMessage);
        }
        else
        {
            // We probably lost our claim.
            rootPage->NotifyUser("Failed to display text to window " + targetWindow->Id + ".", NotifyType::ErrorMessage);
        }
    });
}
