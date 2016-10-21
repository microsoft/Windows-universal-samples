// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_Conversion.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_Conversion::Scenario1_Conversion() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// This is the click handler for the 'Create Generator' button.
/// When this button is activated, this method will create a text conversion
/// generator with the language tag provided by the user. The generator will 
/// try to resolve the language tag and check if the language is supported
/// and installed.
/// </summary>
/// <param name="sender">The object that raised the event.</param>
/// <param name="e">Event data that describes the click action on the button.</param>
void Scenario1_Conversion::CreateGeneratorButton_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    String^ notifyText = ref new String();

    // Try to parse the language and create generator accordingly.
    generator = ref new Windows::Data::Text::TextConversionGenerator(langTag->Text);

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
        if (generatorOperationArea != nullptr)
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
/// Selecting the generator method to use.
/// User can either specify the max candidate number to get, or use the default value.
/// </summary>
/// <param name="sender">The object that raised the event.</param>
/// <param name="e">Event data that describes the click action on the button.</param>
void Scenario1_Conversion::Methods_SelectionChanged(Platform::Object^ sender, SelectionChangedEventArgs^ e)
{
    ComboBoxItem^ selectedItem = dynamic_cast<ComboBoxItem^>(methods->SelectedItem);

    if (maxCandidatesArea != nullptr)
    {
        // Only show the candidate number input control when user chooses to specify the number.
        if ((selectedItem->Tag != nullptr) && selectedItem->Tag->ToString()->Equals("needMaxCount"))
        {
            maxCandidatesArea->Visibility = Windows::UI::Xaml::Visibility::Visible;
        }
        else
        {
            maxCandidatesArea->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        }
    }
}

/// <summary>
/// This is the click handler for the 'Execute' button.
/// When this button is activated, the text conversion generator will get
/// the conversion candidates for given input string and language tag, then 
/// print them in result column.
/// Using 'concurrency::create_task' to create an asynchronous task and
/// handle the result when it completes.
/// </summary>
/// <param name="sender">The object that raised the event.</param>
/// <param name="e">Event data that describes the click action on the button.</param>
void Scenario1_Conversion::Execute_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    // Clean up result column.
    resultView->ItemsSource = nullptr;

    // Clean notification area.
    String^ notifyText = ref new String();
    MainPage::Current->NotifyUser(notifyText, NotifyType::ErrorMessage);

    auto input = Input->Text;

    ComboBoxItem^ selectedItem = dynamic_cast<ComboBoxItem^>(methods->SelectedItem);

    if ((generator != nullptr) && (selectedItem != nullptr) && !input->IsEmpty())
    {
        // Specify the candidate number to get.
        if ((selectedItem->Tag != nullptr) && selectedItem->Tag->ToString()->Equals("needMaxCount"))
        {
            int maxCandidateCount = _wtoi(maxCandidates->Text->Data());

            if (maxCandidateCount > 0)
            {
                // Call the API with max candidate number we expect, and list the result when there are any candidates.
                concurrency::create_task(generator->GetCandidatesAsync(input, static_cast<UINT>(maxCandidateCount))).then([this](IVectorView<String^>^ candidates)
                {
                    DisplayMultipleCandidates(candidates);
                });
            }
            else
            {
                MainPage::Current->NotifyUser("Not a valid candidate number. Expecting positive integer.", NotifyType::ErrorMessage);
            }
        }
        else
        {
            // Call the API with default candidate number, and list the result when there are any candidates. 
            concurrency::create_task(generator->GetCandidatesAsync(input)).then([this](IVectorView<String^>^ candidates)
            {
                DisplayMultipleCandidates(candidates);
            });
        }
    }
}

/// <summary>
/// Helper method for displaying conversion candidates.
/// </summary>
/// <param name="result">The converison candidates we got from text conversion generator.</param>
void Scenario1_Conversion::DisplayMultipleCandidates(IVectorView<String^>^ candidates)
{
    Vector<String^>^ itemsSource = ref new Vector<String^>();
    for (unsigned int i = 0; i < candidates->Size; i++)
    {
        itemsSource->Append(candidates->GetAt(i));
    }

    resultView->ItemsSource = itemsSource;

    if (resultView->Items->Size == 0)
    {
        MainPage::Current->NotifyUser("No candidates.", NotifyType::StatusMessage);
    }
}

