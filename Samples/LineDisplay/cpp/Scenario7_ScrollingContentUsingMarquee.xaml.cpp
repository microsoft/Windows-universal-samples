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
#include "Scenario7_ScrollingContentUsingMarquee.xaml.h"

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

Scenario7_ScrollingContentUsingMarquee::Scenario7_ScrollingContentUsingMarquee()
{
    InitializeComponent();
    DataContext = this;
}

void Scenario7_ScrollingContentUsingMarquee::OnNavigatedTo(NavigationEventArgs^ e)
{
    InitializeAsync();
}

void Scenario7_ScrollingContentUsingMarquee::OnNavigatedFrom(NavigationEventArgs^ e)
{
    delete horizontalScrollableWindow;
    horizontalScrollableWindow = nullptr;
    delete verticalScrollableWindow;
    verticalScrollableWindow = nullptr;
    delete lineDisplay;
    lineDisplay = nullptr;
}

Concurrency::task<void> Scenario7_ScrollingContentUsingMarquee::InitializeAsync()
{
    lineDisplay = co_await rootPage->ClaimScenarioLineDisplayAsync();

    if (lineDisplay != nullptr)
    {
        Size screenSize = lineDisplay->GetAttributes()->ScreenSizeInCharacters;
        unsigned int windowCount = 1;

        if (lineDisplay->Capabilities->IsHorizontalMarqueeSupported && (windowCount < lineDisplay->Capabilities->SupportedWindows))
        {
            // Create a horizontal scrollable window by creating a window
            // whose width is wider than the viewport.
            horizontalScrollableWindow = co_await lineDisplay->TryCreateWindowAsync(
                Rect
                {
                    0,
                    0,
                    screenSize.Width,
                    screenSize.Height
                },
                Size
                {
                    screenSize.Width * 2,
                    screenSize.Height
                });
            if (horizontalScrollableWindow != nullptr)
            {
                windowCount++;
                Helpers::AddItem(ScrollDirectionComboBox, LineDisplayScrollDirection::Left);
                Helpers::AddItem(ScrollDirectionComboBox, LineDisplayScrollDirection::Right);
            }
        }

        if (lineDisplay->Capabilities->IsVerticalMarqueeSupported && (windowCount < lineDisplay->Capabilities->SupportedWindows))
        {
            // Create a vertical scrollable window by creating a window
            // whose height is taller than the viewport.
            verticalScrollableWindow = co_await lineDisplay->TryCreateWindowAsync(
                Rect
                {
                    screenSize.Width,
                    screenSize.Height,
                    0,
                    0
                },
                Size
                {
                    screenSize.Width,
                    screenSize.Height * 2
                });
            if (verticalScrollableWindow != nullptr)
            {
                windowCount++;
                Helpers::AddItem(ScrollDirectionComboBox, LineDisplayScrollDirection::Up);
                Helpers::AddItem(ScrollDirectionComboBox, LineDisplayScrollDirection::Down);
            }
        }
    }

    if (ScrollDirectionComboBox->Items->Size > 0)
    {
        ScrollDirectionComboBox->SelectedIndex = 0;
        StartScrollingButton->IsEnabled = true;
        StopScrollingButton->IsEnabled = true;
    }
}

void Scenario7_ScrollingContentUsingMarquee::StartScrollingButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    invoke_async_lambda([=]() -> task<void>
    {
        LineDisplayScrollDirection scrollDirection = Helpers::GetSelectedItemTag<LineDisplayScrollDirection>(ScrollDirectionComboBox);
        LineDisplayWindow^ selectedWindow = nullptr;

        if (scrollDirection == LineDisplayScrollDirection::Left || scrollDirection == LineDisplayScrollDirection::Right)
        {
            selectedWindow = horizontalScrollableWindow;
        }
        else
        {
            selectedWindow = verticalScrollableWindow;
        }

        // Cannot make changes while scrolling is active.
        co_await StopAllScrollingAsync();

        // Bring the window to the front, clear its old contents,
        // and display some text that will require scrolling.
        co_await selectedWindow->TryRefreshAsync();
        co_await selectedWindow->TryClearTextAsync();
        co_await selectedWindow->TryDisplayTextAsync(
            "An incredibly lengthy string of arbitrary text that cannot be displayed on most line displays " +
            "without being scrolled into view. It will need to be scrolled into view left, right, up or down.");

        // Configure the scrolling behavior and start scrolling.
        selectedWindow->Marquee->Format = Helpers::GetSelectedItemTag<LineDisplayMarqueeFormat>(MarqueeFormatComboBox);
        selectedWindow->Marquee->RepeatWaitInterval = Helpers::TimeSpanFromMilliseconds(RepeatWaitIntervalSlider->Value);
        selectedWindow->Marquee->ScrollWaitInterval = Helpers::TimeSpanFromMilliseconds(ScrollWaitIntervalSlider->Value);
        co_await selectedWindow->Marquee->TryStartScrollingAsync(scrollDirection);
    });
}

task<void> Scenario7_ScrollingContentUsingMarquee::StopAllScrollingAsync()
{
    if (horizontalScrollableWindow != nullptr)
    {
        co_await horizontalScrollableWindow->Marquee->TryStopScrollingAsync();
    }

    if (verticalScrollableWindow != nullptr)
    {
        co_await verticalScrollableWindow->Marquee->TryStopScrollingAsync();
    }
}

void Scenario7_ScrollingContentUsingMarquee::StopScrollingButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopAllScrollingAsync();
}
