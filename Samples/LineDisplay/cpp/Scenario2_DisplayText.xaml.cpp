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
    InitializeAsync();
}

void Scenario2_DisplayText::OnNavigatingFrom(NavigatingCancelEventArgs^ e)
{
    delete lineDisplay;
    lineDisplay = nullptr;
}

task<void> Scenario2_DisplayText::InitializeAsync()
{
    lineDisplay = co_await rootPage->ClaimScenarioLineDisplayAsync();
    BlinkCheckBox->IsEnabled = (lineDisplay != nullptr) && (lineDisplay->Capabilities->CanBlink != LineDisplayTextAttributeGranularity::NotSupported);
    DisplayTextButton->IsEnabled = (lineDisplay != nullptr);
}

void Scenario2_DisplayText::DisplayTextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    invoke_async_lambda([=]() -> task<void>
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

        if (BlinkCheckBox->IsChecked->Value)
        {
            attribute = LineDisplayTextAttribute::Blink;
        }

        if (co_await lineDisplay->DefaultWindow->TryClearTextAsync() &&
            co_await lineDisplay->DefaultWindow->TryDisplayTextAsync(text, attribute, position))
        {
            rootPage->NotifyUser("Text displayed sucessfully", NotifyType::StatusMessage);
        }
        else
        {
            // We probably lost our claim.
            rootPage->NotifyUser("Unable to display text", NotifyType::ErrorMessage);
        }
    });
}
