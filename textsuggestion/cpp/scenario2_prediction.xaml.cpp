// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_Prediction.xaml.h"

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

Scenario2_Prediction::Scenario2_Prediction()
{
    InitializeComponent();
}

/// <summary>
/// This is the click handler for the 'Create Generator' button.
/// When this button is activated, the Text Suggestion API will try
/// to resolve the language tag provided by the user, and create a 
/// Prediction Generator.
/// </summary>
/// <param name="sender">The object that raised the event.</param>
/// <param name="e">Event data that describes the click action on the button.</param>
void Scenario2_Prediction::CreateGeneratorButton_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    String^ notifyText = ref new String();

    // Try to parse the language and create generator accordingly.
    generator = ref new Windows::Data::Text::TextPredictionGenerator(langTag->Text);

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
/// Selecting the API interface to use.
/// User can either specify the max candidate number to get, or use the default value.
/// </summary>
/// <param name="sender">The object that raised the event.</param>
/// <param name="e">Event data that describes the click action on the button.</param>
void Scenario2_Prediction::Methods_SelectionChanged(Platform::Object^ sender, SelectionChangedEventArgs^ e)
{
    ComboBoxItem^ selectedItem = dynamic_cast<ComboBoxItem^>(methods->SelectedItem);

    if (maxCandidatesArea != nullptr)
    {
        // Only show the candidate number input control when user chooses to specify the number.
        if (selectedItem->Content->ToString()->Equals("GetCandidatesAsync(WithMaxCount)"))
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
/// When this button is activated, the text suggestion generator will get
/// the Prediction result for given input string and language tag, then 
/// print them in result column.
/// Using 'concurrency::create_task' to create an asynchronous task and
/// handle the result when it completes.
/// </summary>
/// <param name="sender">The object that raised the event.</param>
/// <param name="e">Event data that describes the click action on the button.</param>
void Scenario2_Prediction::Execute_Click(Platform::Object^ sender, RoutedEventArgs^ e)
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
        // Specifiy the candidate number to get.
        if (selectedItem->Content->ToString()->Equals("GetCandidatesAsync(WithMaxCount)"))
        {
            int maxCandidateCount = _wtoi(maxCandidates->Text->Data());

            if (maxCandidateCount > 0)
            {
                // Call the API with max candidate number we expect, and list the result when there are any candidates.
                concurrency::create_task(generator->GetCandidatesAsync(input, static_cast<UINT>(maxCandidateCount))).then([this](IVectorView<Platform::String^>^ result)
                {
                    itemsSource = ref new Vector<Platform::String^>();
                    for (unsigned int i = 0; i < result->Size; i++)
                    {
                        itemsSource->Append(result->GetAt(i));
                    }

                    resultView->ItemsSource = itemsSource;

                    if (resultView->Items->Size == 0)
                    {
                        MainPage::Current->NotifyUser("No candidates.", NotifyType::StatusMessage);
                    }
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
            concurrency::create_task(generator->GetCandidatesAsync(input)).then([this](IVectorView<Platform::String^>^ result)
            {
                itemsSource = ref new Vector<Platform::String^>();
                for (unsigned int i = 0; i < result->Size; i++)
                {
                    itemsSource->Append(result->GetAt(i));
                }

                resultView->ItemsSource = itemsSource;

                if (resultView->Items->Size == 0)
                {
                    MainPage::Current->NotifyUser("No candidates.", NotifyType::StatusMessage);
                }
            });
        }
    }
}
