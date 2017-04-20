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
#include "Scenario2_DisplayText.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_DisplayText::Scenario2_DisplayText()
{
    InitializeComponent();
}

void Scenario2_DisplayText::OnNavigatedTo(NavigationEventArgs^ e)
{
    if (rootPage->LineDisplayId->IsEmpty())
    {
        rootPage->NotifyUser("You must use scenario 1 to select a line display", NotifyType::ErrorMessage);
        DisplayTextButton->IsEnabled = false;
    }
}

void Scenario2_DisplayText::DisplayTextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    create_task(ClaimedLineDisplay::FromIdAsync(rootPage->LineDisplayId))
        .then([this](ClaimedLineDisplay^ lineDisplay)
    {
        if (lineDisplay != nullptr)
        {
            String^ text = "Hello from UWP";

            Point position = {};
            if (CenterCheckBox->IsChecked->Value)
            {
                uint32 length = text->Length();
                if (length < lineDisplay->DefaultWindow->SizeInCharacters.Width)
                {
                    position.X = (lineDisplay->DefaultWindow->SizeInCharacters.Width - length) / 2;
                }
            }

            LineDisplayTextAttribute attribute = LineDisplayTextAttribute::Normal;

            // If blinking is requested, verify that the device supports blinking.
            if (BlinkCheckBox->IsChecked->Value)
            {
                if (lineDisplay->Capabilities->CanBlink == LineDisplayTextAttributeGranularity::NotSupported)
                {
                    BlinkNotSupportedText->Visibility = Windows::UI::Xaml::Visibility::Visible;
                }
                else
                {
                    // Device supports blinking.
                    attribute = LineDisplayTextAttribute::Blink;
                }
            }

            create_task(lineDisplay->DefaultWindow->TryClearTextAsync())
                .then([this, text, attribute, position, lineDisplay](bool result)
            {
                if (result)
                {
                    return create_task(lineDisplay->DefaultWindow->TryDisplayTextAsync(text, attribute, position));
                }
                else
                {
                    return task_from_result(result);
                }
            }).then([this, lineDisplay](bool result)
            {
                if (result)
                {
                    rootPage->NotifyUser("Text displayed sucessfully", NotifyType::StatusMessage);
                }
                else
                {
                    // We probably lost our claim.
                    rootPage->NotifyUser("Unable to display text", NotifyType::ErrorMessage);
                }

                // Close the claimed line display.
                delete lineDisplay;
            }, task_continuation_context::get_current_winrt_context());
        }
        else
        {
            rootPage->NotifyUser("Unable to connect to the selected Line Display", NotifyType::ErrorMessage);
        }
    }, task_continuation_context::get_current_winrt_context());
}
