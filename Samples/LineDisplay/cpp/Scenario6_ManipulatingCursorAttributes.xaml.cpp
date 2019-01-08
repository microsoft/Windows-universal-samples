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
#include "Scenario6_ManipulatingCursorAttributes.xaml.h"

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

Scenario6_ManipulatingCursorAttributes::Scenario6_ManipulatingCursorAttributes()
{
    InitializeComponent();
    DataContext = this;
}

void Scenario6_ManipulatingCursorAttributes::OnNavigatedTo(NavigationEventArgs^ e)
{
    InitializeAsync();
}

void Scenario6_ManipulatingCursorAttributes::OnNavigatedFrom(NavigationEventArgs^ e)
{
    delete lineDisplay;
    lineDisplay = nullptr;
}

task<void> Scenario6_ManipulatingCursorAttributes::InitializeAsync()
{
    UpdateButton->IsEnabled = false;
    DisplayTextButton->IsEnabled = false;
    ResetButton->IsEnabled = false;
    CursorTypeComboBox->Items->Clear();

    lineDisplay = co_await rootPage->ClaimScenarioLineDisplayAsync();

    CursorTypeComboBox->IsEnabled = (lineDisplay != nullptr) && lineDisplay->DefaultWindow->Cursor->CanCustomize;
    if (CursorTypeComboBox->IsEnabled)
    {
        if (lineDisplay->DefaultWindow->Cursor->IsBlockSupported)
        {
            Helpers::AddItem(CursorTypeComboBox, LineDisplayCursorType::Block);
        }

        if (lineDisplay->DefaultWindow->Cursor->IsHalfBlockSupported)
        {
            Helpers::AddItem(CursorTypeComboBox, LineDisplayCursorType::HalfBlock);
        }

        if (lineDisplay->DefaultWindow->Cursor->IsOtherSupported)
        {
            Helpers::AddItem(CursorTypeComboBox, LineDisplayCursorType::Other);
        }

        if (lineDisplay->DefaultWindow->Cursor->IsReverseSupported)
        {
            Helpers::AddItem(CursorTypeComboBox, LineDisplayCursorType::Reverse);
        }

        if (lineDisplay->DefaultWindow->Cursor->IsUnderlineSupported)
        {
            Helpers::AddItem(CursorTypeComboBox, LineDisplayCursorType::Underline);
        }

        if (SupportedCursorTypes->Size > 0)
        {
            CursorTypeComboBox->SelectedIndex = 0;
        }
    }

    CursorTypeComboBox->IsEnabled = true;//rjc
    Helpers::AddItem(CursorTypeComboBox, LineDisplayCursorType::HalfBlock);
    CursorTypeComboBox->SelectedIndex = 0;

    IsAutoAdvanceEnabledCheckbox->IsEnabled = (lineDisplay != nullptr);

    IsBlinkEnabledCheckbox->IsEnabled = (lineDisplay != nullptr) && lineDisplay->DefaultWindow->Cursor->IsBlinkSupported;
    SetCursorPositionCheckbox->IsEnabled = (lineDisplay != nullptr);
    UpdateButton->IsEnabled = (lineDisplay != nullptr);
    DisplayTextButton->IsEnabled = (lineDisplay != nullptr);

    ResetButton->IsEnabled = true;
}

void Scenario6_ManipulatingCursorAttributes::UpdateButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    invoke_async_lambda([=]() -> task<void>
    {
        LineDisplayCursorAttributes^ cursorAttributes = lineDisplay->DefaultWindow->Cursor->GetAttributes();
        cursorAttributes->IsAutoAdvanceEnabled = IsAutoAdvanceEnabledCheckbox->IsChecked->Value;

        if (lineDisplay->DefaultWindow->Cursor->CanCustomize)
        {
            cursorAttributes->CursorType = Helpers::GetSelectedItemTag<LineDisplayCursorType>(CursorTypeComboBox);
        }

        if (lineDisplay->DefaultWindow->Cursor->IsBlinkSupported)
        {
            cursorAttributes->IsBlinkEnabled = IsBlinkEnabledCheckbox->IsChecked->Value;
        }

        if (SetCursorPositionCheckbox->IsChecked->Value)
        {
            Point position = Point
            {
                Helpers::ParseUIntWithFallback(CursorPositionXTextBox, -1.0f),
                Helpers::ParseUIntWithFallback(CursorPositionYTextBox, -1.0f),
            };

            Size windowSizeInCharacters = lineDisplay->DefaultWindow->SizeInCharacters;
            if ((position.X < 0) || (position.X >= windowSizeInCharacters.Width) ||
                (position.Y < 0) || (position.Y >= windowSizeInCharacters.Height))
            {
                rootPage->NotifyUser("Cursor position must be within the window", NotifyType::ErrorMessage);
                return;
            }

            cursorAttributes->Position = position;
        }

        if (co_await lineDisplay->DefaultWindow->Cursor->TryUpdateAttributesAsync(cursorAttributes))
        {
            rootPage->NotifyUser("Cursor updated.", NotifyType::StatusMessage);
        }
        else
        {
            rootPage->NotifyUser("Failed to update the cursor.", NotifyType::ErrorMessage);
        }
    });
}

void Scenario6_ManipulatingCursorAttributes::DisplayTextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    invoke_async_lambda([=]() -> task<void>
    {
        if (co_await lineDisplay->DefaultWindow->TryDisplayTextAsync("Text"))
        {
            rootPage->NotifyUser("Text displayed.", NotifyType::StatusMessage);
        }
        else
        {
            rootPage->NotifyUser("Failed to display text.", NotifyType::ErrorMessage);
        }
    });
}

void Scenario6_ManipulatingCursorAttributes::ResetButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    invoke_async_lambda([=]()->task<void>
    {
        delete lineDisplay;
        lineDisplay = nullptr;
        co_await InitializeAsync();
    });
}
