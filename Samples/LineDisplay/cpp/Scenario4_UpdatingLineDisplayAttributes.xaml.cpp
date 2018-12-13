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
#include "Scenario4_UpdatingLineDisplayattributes.xaml.h"

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

Scenario4_UpdatingLineDisplayAttributes::Scenario4_UpdatingLineDisplayAttributes()
{
    InitializeComponent();
    DataContext = this;
}

void Scenario4_UpdatingLineDisplayAttributes::OnNavigatedTo(NavigationEventArgs^ e)
{
    InitializeAsync();
}

void Scenario4_UpdatingLineDisplayAttributes::OnNavigatedFrom(NavigationEventArgs^ e)
{
    delete lineDisplay;
    lineDisplay = nullptr;
}

task<void> Scenario4_UpdatingLineDisplayAttributes::InitializeAsync()
{
    lineDisplay = co_await rootPage->ClaimScenarioLineDisplayAsync();

    BlinkRateSlider->IsEnabled = (lineDisplay != nullptr) && lineDisplay->Capabilities->CanChangeBlinkRate;
    BrightnessSlider->IsEnabled = (lineDisplay != nullptr) && lineDisplay->Capabilities->IsBrightnessSupported;

    SupportedScreenSizesComboBox->IsEnabled = (lineDisplay != nullptr) && lineDisplay->Capabilities->CanChangeScreenSize;

    if (SupportedScreenSizesComboBox->IsEnabled)
    {
        for (Size screenSize : lineDisplay->SupportedScreenSizesInCharacters)
        {
            Helpers::AddItem(SupportedScreenSizesComboBox, screenSize, screenSize.Width.ToString() + " x " + screenSize.Height.ToString());
        }

        if (SupportedScreenSizesComboBox->Items->Size > 0)
        {
            SupportedScreenSizesComboBox->SelectedIndex = 0;
        }
    }

    CharacterSetMappingEnabledCheckbox->IsEnabled = (lineDisplay != nullptr) && lineDisplay->Capabilities->CanMapCharacterSets;

    UpdateButton->IsEnabled = (lineDisplay != nullptr);

    if (UpdateButton->IsEnabled)
    {
        SetValuesFromLineDisplay();
        if (lineDisplay->Capabilities->CanBlink == LineDisplayTextAttributeGranularity::NotSupported)
        {
            co_await lineDisplay->DefaultWindow->TryDisplayTextAsync("Regular Text");
        }
        else
        {
            co_await lineDisplay->DefaultWindow->TryDisplayTextAsync("Blinking Text", LineDisplayTextAttribute::Blink);
        }
    }
}

void Scenario4_UpdatingLineDisplayAttributes::SetValuesFromLineDisplay()
{
    LineDisplayAttributes^ attributes = lineDisplay->GetAttributes();
    BlinkRateSlider->Value = Helpers::MillisecondsFromTimeSpan(attributes->BlinkRate);
    BrightnessSlider->Value = attributes->Brightness;
    CharacterSetMappingEnabledCheckbox->IsChecked = attributes->IsCharacterSetMappingEnabled;
}

void Scenario4_UpdatingLineDisplayAttributes::UpdateButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    invoke_async_lambda([=]() -> task<void>
    {
        LineDisplayAttributes^ attributes = lineDisplay->GetAttributes();

        if (lineDisplay->Capabilities->CanChangeBlinkRate)
        {
            attributes->BlinkRate = Helpers::TimeSpanFromMilliseconds(BlinkRateSlider->Value);
        }

        if (lineDisplay->Capabilities->IsBrightnessSupported)
        {
            attributes->Brightness = static_cast<int>(BrightnessSlider->Value);
        }

        if (lineDisplay->Capabilities->CanChangeScreenSize)
        {
            attributes->ScreenSizeInCharacters = Helpers::GetSelectedItemTag<Size>(SupportedScreenSizesComboBox);
        }

        if (lineDisplay->Capabilities->CanMapCharacterSets)
        {
            attributes->IsCharacterSetMappingEnabled = CharacterSetMappingEnabledCheckbox->IsChecked->Value;
        }

        if (co_await lineDisplay->TryUpdateAttributesAsync(attributes))
        {
            rootPage->NotifyUser("Attributes updated successfully.", NotifyType::StatusMessage);

            // The resulting attributes may not match our request.
            // For example, the Line Display will choose the closest available blink rate.
            // Update the controls to show what the Line Display actually used.
            SetValuesFromLineDisplay();
        }
        else
        {
            rootPage->NotifyUser("Failed to update attributes.", NotifyType::ErrorMessage);
        }
    });
}
