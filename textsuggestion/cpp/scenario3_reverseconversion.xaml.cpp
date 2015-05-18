// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario3_ReverseConversion.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario3_ReverseConversion::Scenario3_ReverseConversion()
{
    InitializeComponent();
}

/// <summary>
/// This is the click handler for the 'Create Generator' button.
/// When this button is activated, the Text Suggestion API will try
/// to resolve the language tag provided by the user, and create a 
/// Reverse Conversion Generator.
/// </summary>
/// <param name="sender">The object that raised the event.</param>
/// <param name="e">Event data that describes the click action on the button.</param>
void Scenario3_ReverseConversion::CreateGeneratorButton_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    String^ notifyText = ref new String();

    // Try to parse the language and create generator accordingly.
    generator = ref new Windows::Data::Text::TextReverseConversionGenerator(langTag->Text);

    // Only allow generator operation when the language is available and installed.
    if (generator == nullptr)
    {
        if (generatorOperationArea != nullptr)
        {
            generatorOperationArea->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        }

        notifyText = "Successfully instantiated a generator.";
        MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);
    }
    else if (generator->ResolvedLanguage == "und")
    {
        {
            generatorOperationArea->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        }

        notifyText = "Unsupported language.";
        MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);
    }
    else if (generator->LanguageAvailableButNotInstalled)
    {
        if (generatorOperationArea != nullptr)
        {
            generatorOperationArea->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        }

        notifyText = "Language available but not installed.";
        MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);
    }
    else
    {
        if (generatorOperationArea != nullptr)
        {
            generatorOperationArea->Visibility = Windows::UI::Xaml::Visibility::Visible;
        }

        notifyText = "Successfully instantiated a generator.";
        MainPage::Current->NotifyUser(notifyText, NotifyType::StatusMessage);
    }
}

/// <summary>
/// This is the click handler for the 'Execute' button.
/// When this button is activated, the text suggestion generator will get
/// the conversion result for given input string and language tag, then 
/// print them in result column.
/// Using 'concurrency::create_task' to create an asynchronous task and
/// handle the result when it completes.
/// </summary>
/// <param name="sender">The object that raised the event.</param>
/// <param name="e">Event data that describes the click action on the button.</param>
void Scenario3_ReverseConversion::Execute_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    // Clean up result column.
    resultText->Text = "";

    // Clean notification area.
    String^ notifyText = ref new String();
    MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);

    auto input = Input->Text;

    ComboBoxItem^ selectedItem = dynamic_cast<ComboBoxItem^>(methods->SelectedItem);

    if ((generator != nullptr) && (selectedItem != nullptr) && !input->IsEmpty())
    {
        // Get the reverse conversion result through calling the API and print it in result area.
        concurrency::create_task(generator->ConvertBackAsync(input)).then([this](Platform::String^ result)
        {
            resultText->Text = result;

            if (resultText->Text->IsEmpty())
            {
                MainPage::Current->NotifyUser("No candidates.", NotifyType::StatusMessage);
            }
        });
    }
}
