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
#include "Scenario_SRGSConstraint.xaml.h"
#include "AudioCapturePermissions.h"

using namespace SDKTemplate;
using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::SpeechRecognition;
using namespace Windows::Storage;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario_SRGSConstraint::Scenario_SRGSConstraint() : rootPage(MainPage::Current)
{
    InitializeComponent();

    colorLookup = ref new Map<String^, Color>(
    {
        { "COLOR_RED",   Colors::Red },    { "COLOR_BLUE",  Colors::Blue },   { "COLOR_BLACK",  Colors::Black },
        { "COLOR_BROWN", Colors::Brown },  { "COLOR_PURPLE",Colors::Purple }, { "COLOR_GREEN",  Colors::Green },
        { "COLOR_YELLOW",Colors::Yellow }, { "COLOR_CYAN",  Colors::Cyan },   { "COLOR_MAGENTA",Colors::Magenta },
        { "COLOR_ORANGE",Colors::Orange }, { "COLOR_GRAY",  Colors::Gray },   { "COLOR_WHITE",  Colors::White }
    });
}

/// <summary>
/// When activating the scenario, ensure we have permission from the user to access their microphone, and
/// provide an appropriate path for the user to enable access to the microphone if they haven't
/// given explicit permission for it.
/// </summary>
/// <param name="e">The navigation event details</param>
void Scenario_SRGSConstraint::OnNavigatedTo(NavigationEventArgs^ e)
{
    Page::OnNavigatedTo(e);

    // Save the UI thread dispatcher to allow speech status messages to be shown on the UI.
    dispatcher = CoreWindow::GetForCurrentThread()->Dispatcher;

    create_task(AudioCapturePermissions::RequestMicrophonePermissionAsync(), task_continuation_context::use_current())
        .then([this](bool permissionGained)
    {
        if (permissionGained)
        {
            // Enable the recognition buttons.
            this->btnRecognizeWithUI->IsEnabled = true;
            this->btnRecognizeWithoutUI->IsEnabled = true;

			Windows::Globalization::Language^ speechLanguage = SpeechRecognizer::SystemSpeechLanguage;
			speechContext = ResourceContext::GetForCurrentView();
			speechContext->Languages = ref new VectorView<String^>(1, speechLanguage->LanguageTag);

			speechResourceMap = ResourceManager::Current->MainResourceMap->GetSubtree(L"LocalizationSpeechResources");

			PopulateLanguageDropdown();
			InitializeRecognizer(SpeechRecognizer::SystemSpeechLanguage);
        }
        else
        {
            this->resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            this->resultTextBlock->Text = "Permission to access capture resources was not given by the user; please set the application setting in Settings->Privacy->Microphone.";
			this->cbLanguageSelection->IsEnabled = false;
        }
	});
}

/// <summary>
/// Creates a SpeechRecognizer instance and initializes the grammar.
/// </summary>
void Scenario_SRGSConstraint::InitializeRecognizer(Windows::Globalization::Language^ recognizerLanguage)
{
    // If reinitializing the recognizer (ie, changing the speech language), clean up the old recognizer first.
    // Avoid doing this while the recognizer is active by disabling the ability to change languages while listening.
    if (this->speechRecognizer != nullptr)
    {
        speechRecognizer->StateChanged -= stateChangedToken;

        delete this->speechRecognizer;
        this->speechRecognizer = nullptr;
    }

    try
    {
        // Create an instance of SpeechRecognizer.
        this->speechRecognizer = ref new SpeechRecognizer(recognizerLanguage);

        // Provide feedback to the user about the state of the recognizer.
        stateChangedToken = speechRecognizer->StateChanged +=
            ref new Windows::Foundation::TypedEventHandler<
            SpeechRecognizer ^,
            SpeechRecognizerStateChangedEventArgs ^>(
                this,
                &Scenario_SRGSConstraint::SpeechRecognizer_StateChanged);

        // RecognizeWithUIAsync allows developers to customize the prompts.
        speechRecognizer->UIOptions->ExampleText = speechResourceMap->GetValue("SRGSUIOptionsExampleText", speechContext)->ValueAsString;

        // Initialize the SRGS-compliant XML file.
        // For more information about grammars for Windows apps and how to
        // define and use SRGS-compliant grammars in your app, see
        // https://msdn.microsoft.com/en-us/library/dn596121.aspx
        String^ fileName = L"SRGS\\" + recognizerLanguage->LanguageTag + L"\\SRGSColors.xml";
        resultTextBlock->Text = speechResourceMap->GetValue("SRGSHelpText", speechContext)->ValueAsString;

        create_task(Package::Current->InstalledLocation->GetFileAsync(fileName), task_continuation_context::use_current())
            .then([this](task<StorageFile^> getFileTask)
        {
            StorageFile^ grammarContentFile = getFileTask.get();
            SpeechRecognitionGrammarFileConstraint^ grammarConstraint = ref new SpeechRecognitionGrammarFileConstraint(grammarContentFile);
            speechRecognizer->Constraints->Append(grammarConstraint);

            return create_task(speechRecognizer->CompileConstraintsAsync(), task_continuation_context::use_current());

        }).then([this](task<SpeechRecognitionCompilationResult^> previousTask)
        {
            SpeechRecognitionCompilationResult^ compilationResult = previousTask.get();

            // Set EndSilenceTimeout to give users more time to complete speaking a phrase.
            TimeSpan endSilenceTime;
            endSilenceTime.Duration = 12000000L;
            speechRecognizer->Timeouts->EndSilenceTimeout = endSilenceTime; // (1.2 seconds in nanoseconds)

            // Check to make sure that the constraints were in a proper format and the recognizer was able to compile it.
            if (compilationResult->Status != SpeechRecognitionResultStatus::Success)
            {
                // Disable the recognition buttons.
                btnRecognizeWithUI->IsEnabled = false;
                btnRecognizeWithoutUI->IsEnabled = false;

                // Let the user know that the grammar didn't compile properly.
                resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                resultTextBlock->Text = "Unable to compile grammar.";
            }
            else
            {
                btnRecognizeWithUI->IsEnabled = true;
                btnRecognizeWithoutUI->IsEnabled = true;

                // Let the user know that the grammar didn't compile properly.
                resultTextBlock->Text = speechResourceMap->GetValue("SRGSHelpText", speechContext)->ValueAsString;
                resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            }
        }, task_continuation_context::use_current());
    }
    catch (Platform::Exception^ ex)
    {
        if ((unsigned int)ex->HResult == HResultRecognizerNotFound)
        {
            btnRecognizeWithUI->IsEnabled = false;
            btnRecognizeWithoutUI->IsEnabled = false;

            resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            resultTextBlock->Text = L"Speech Language pack for selected language not installed.";
        }
        else
        {
            auto messageDialog = ref new Windows::UI::Popups::MessageDialog(ex->Message, "Exception");
            create_task(messageDialog->ShowAsync());
        }
    }
}

/// <summary>
/// Ensure that we clean up any state tracking event handlers created in OnNavigatedTo to prevent leaks.
/// </summary>
/// <param name="e">Details about the navigation event</param>
void Scenario_SRGSConstraint::OnNavigatedFrom(NavigationEventArgs^ e)
{
    Page::OnNavigatedFrom(e);

    if (speechRecognizer != nullptr)
    {
        speechRecognizer->StateChanged -= stateChangedToken;

        delete speechRecognizer;
        speechRecognizer = nullptr;
    }
}

/// <summary>
/// Begin Recognition with RecognizeWithUI, then update the UI with the results.
/// </summary>
void Scenario_SRGSConstraint::RecognizeWithUI_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Reset the text to prompt the user.
    resultTextBlock->Text = speechResourceMap->GetValue("SRGSListeningPromptText", speechContext)->ValueAsString;


    // Start recognition.
    create_task(speechRecognizer->RecognizeWithUIAsync(), task_continuation_context::use_current())
        .then([this](task<SpeechRecognitionResult^> recognitionTask)
    {
        try
        {
            SpeechRecognitionResult^ speechRecognitionResult = recognitionTask.get();
            if (speechRecognitionResult->Status == SpeechRecognitionResultStatus::Success)
            {
                HandleRecognitionResult(speechRecognitionResult);
            }
            else
            {
                resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                resultTextBlock->Text = L"Speech Recognition Failure, Status: " + speechRecognitionResult->Status.ToString();
            }
        }
        catch (ObjectDisposedException^ exception)
        {
            // ObjectDisposedException will be thrown if you exit the scenario while the recogizer is actively
            // processing speech. Since this happens here when we navigate out of the scenario, don't try to 
            // show a message dialog for this exception.
            OutputDebugString(L"ObjectDisposedException caught while recognition in progress (can be ignored):");
            OutputDebugString(exception->ToString()->Data());
        }
        catch (Exception^ exception)
        {
            // Handle the speech privacy policy error.
            if ((unsigned int)exception->HResult == Scenario_SRGSConstraint::HResultPrivacyStatementDeclined)
            {
                resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                resultTextBlock->Text = "The privacy statement was declined. Go to Settings -> Privacy -> Speech, inking and typing, and ensure you have viewed the privacy policy, and Cortana is set to 'Get To Know You'";
            }
            else
            {
                auto messageDialog = ref new Windows::UI::Popups::MessageDialog(exception->Message, "Exception");
                create_task(messageDialog->ShowAsync());
            }
        }
    });
}

/// <summary>
/// Uses the recognizer constructed earlier to listen for speech from the user before displaying
/// it back on the screen. Uses developer-provided UI for user feedback.
/// </summary>
/// <param name="sender">Button that triggered this event</param>
/// <param name="e">State information about the routed event</param>
void Scenario_SRGSConstraint::RecognizeWithoutUI_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Reset the text to prompt the user.
    resultTextBlock->Text = speechResourceMap->GetValue("SRGSListeningPromptText", speechContext)->ValueAsString;

    // Disable the UI while recognition is occurring, and provide feedback to the user about current state.
    btnRecognizeWithUI->IsEnabled = false;
    btnRecognizeWithoutUI->IsEnabled = false;
    cbLanguageSelection->IsEnabled = false;
    listenWithoutUIButtonText->Text = " listening for speech...";

    // Start recognition.
    create_task(speechRecognizer->RecognizeAsync(), task_continuation_context::use_current())
        .then([this](task<SpeechRecognitionResult^> recognitionTask)
    {
        try
        {
            SpeechRecognitionResult^ speechRecognitionResult = recognitionTask.get();
            if (speechRecognitionResult->Status == SpeechRecognitionResultStatus::Success)
            {
                HandleRecognitionResult(speechRecognitionResult);
            }
            else
            {
                resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                resultTextBlock->Text = L"Speech Recognition Failure, Status: " + speechRecognitionResult->Status.ToString();
            }

            // Reset UI state.
            listenWithoutUIButtonText->Text = " without UI";
            btnRecognizeWithUI->IsEnabled = true;
            btnRecognizeWithoutUI->IsEnabled = true;
            cbLanguageSelection->IsEnabled = true;
        }
        catch (ObjectDisposedException^ exception)
        {
            // ObjectDisposedException will be thrown if you exit the scenario while the recogizer is actively
            // processing speech. Since this happens here when we navigate out of the scenario, don't try to 
            // show a message dialog for this exception.
            OutputDebugString(L"ObjectDisposedException caught while recognition in progress (can be ignored):");
            OutputDebugString(exception->ToString()->Data());
        }
        catch (Exception^ exception)
        {
            // Handle the speech privacy policy error.
            if ((unsigned int)exception->HResult == Scenario_SRGSConstraint::HResultPrivacyStatementDeclined)
            {
                resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                resultTextBlock->Text = "The privacy statement was declined. Go to Settings -> Privacy -> Speech, inking and typing, and ensure you have viewed the privacy policy, and Cortana is set to 'Get To Know You'";
            }
            else
            {
                auto messageDialog = ref new Windows::UI::Popups::MessageDialog(exception->Message, "Exception");
                create_task(messageDialog->ShowAsync());
            }
        }
    });
}

/// <summary>
/// Handle SpeechRecognizer state changed events by updating a UI component.
/// </summary>
/// <param name="sender">Speech recognizer that generated this status event</param>
/// <param name="args">The recognizer's status</param>
void Scenario_SRGSConstraint::SpeechRecognizer_StateChanged(SpeechRecognizer ^sender, SpeechRecognizerStateChangedEventArgs ^args)
{
    dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        rootPage->NotifyUser("Speech recognizer state: " + args->State.ToString(), NotifyType::StatusMessage);
    }));

}

/// <summary>
/// Uses the result from the speech recognizer to change the colors of the shapes.
/// </summary>
/// <param name="recoResult">The result from the recognition event</param>
void Scenario_SRGSConstraint::HandleRecognitionResult(SpeechRecognitionResult^ recoResult)
{
    if (recoResult->Confidence == SpeechRecognitionConfidence::High ||
        recoResult->Confidence == SpeechRecognitionConfidence::Medium)
    {
        String^ garbagePrompt = "";


        // BACKGROUND: Check to see if the recognition result contains the semantic key for the background color,
        // and not a match for the GARBAGE rule, and change the color.
        if (recoResult->SemanticInterpretation->Properties->HasKey("KEY_BACKGROUND") && recoResult->SemanticInterpretation->Properties->Lookup("KEY_BACKGROUND")->GetAt(0) != "...")
        {
            String^ backgroundColor = recoResult->SemanticInterpretation->Properties->Lookup("KEY_BACKGROUND")->GetAt(0);
            colorRectangle->Fill = ref new SolidColorBrush(getColor(backgroundColor));
        }

        // If "background" was matched, but the color rule matched GARBAGE, prompt the user.
        else if (recoResult->SemanticInterpretation->Properties->HasKey("KEY_BACKGROUND") && recoResult->SemanticInterpretation->Properties->Lookup("KEY_BACKGROUND")->GetAt(0) == "...")
        {
            garbagePrompt += speechResourceMap->GetValue("SRGSBackgroundGarbagePromptText", speechContext)->ValueAsString;
            resultTextBlock->Text = garbagePrompt;
        }

        // BORDER: Check to see if the recognition result contains the semantic key for the border color,
        // and not a match for the GARBAGE rule, and change the color.
        if (recoResult->SemanticInterpretation->Properties->HasKey("KEY_BORDER") && recoResult->SemanticInterpretation->Properties->Lookup("KEY_BORDER")->GetAt(0) != "...")
        {
            String^ borderColor = recoResult->SemanticInterpretation->Properties->Lookup("KEY_BORDER")->GetAt(0);
            colorRectangle->Stroke = ref new SolidColorBrush(getColor(borderColor));
        }

        // If "border" was matched, but the color rule matched GARBAGE, prompt the user.
        else if (recoResult->SemanticInterpretation->Properties->HasKey("KEY_BORDER") && recoResult->SemanticInterpretation->Properties->Lookup("KEY_BORDER")->GetAt(0) == "...")
        {
            garbagePrompt += speechResourceMap->GetValue("SRGSBorderGarbagePromptText", speechContext)->ValueAsString;
            resultTextBlock->Text = garbagePrompt;
        }

        // CIRCLE: Check to see if the recognition result contains the semantic key for the circle color,
        // and not a match for the GARBAGE rule, and change the color.
        if (recoResult->SemanticInterpretation->Properties->HasKey("KEY_CIRCLE") && recoResult->SemanticInterpretation->Properties->Lookup("KEY_CIRCLE")->GetAt(0) != "...")
        {
            String^ circleColor = recoResult->SemanticInterpretation->Properties->Lookup("KEY_CIRCLE")->GetAt(0);
            colorCircle->Fill = ref new SolidColorBrush(getColor(circleColor));
        }

        // If "circle" was matched, but the color rule matched GARBAGE, prompt the user.
        else if (recoResult->SemanticInterpretation->Properties->HasKey("KEY_CIRCLE") && recoResult->SemanticInterpretation->Properties->Lookup("KEY_CIRCLE")->GetAt(0) == "...")
        {
            garbagePrompt += speechResourceMap->GetValue("SRGSCircleGarbagePromptText", speechContext)->ValueAsString;
            resultTextBlock->Text = garbagePrompt;
        }

        // Initialize a string that will describe the user's color choices.
        String^ textBoxColors = "You selected (Semantic Interpretation) -> \n";

        std::for_each(begin(recoResult->SemanticInterpretation->Properties),
            end(recoResult->SemanticInterpretation->Properties),
            [&](IKeyValuePair<String^, IVectorView<String^>^>^ child)
        {
            // Check to see if any of the semantic values in recognition result contains a match for the GARBAGE rule.
            if (!child->Value->Equals("..."))
            {
                textBoxColors += child->Value->GetAt(0) + " ";
                if (child->Key != nullptr)
                {
                    textBoxColors += child->Key;
                }
                else
                {
                    textBoxColors += "null";
                }
                textBoxColors += "\n";

                resultTextBlock->Text = textBoxColors;
            }

            // If there was no match to the colors rule or if it matched GARBAGE, prompt the user.
            else
            {
                resultTextBlock->Text = garbagePrompt;
            }
        });
    }
    else
    {
        resultTextBlock->Text = speechResourceMap->GetValue("SRGSGarbagePromptText", speechContext)->ValueAsString;
    }
}

/// <summary>
/// Creates a color object from the passed in string.
/// </summary>
/// <param name="colorString">The name of the color</param>
Windows::UI::Color Scenario_SRGSConstraint::getColor(Platform::String^ colorString)
{
    Color newColor = Colors::Transparent;

    if (colorLookup->HasKey(colorString))
    {
        newColor = colorLookup->Lookup(colorString);
    }

    return newColor;
}

/// <summary>
/// Populate language dropdown with supported Dictation languages.
/// </summary>
void Scenario_SRGSConstraint::PopulateLanguageDropdown()
{
    // disable callback temporarily.
    cbLanguageSelection->SelectionChanged -= cbLanguageSelectionSelectionChangedToken;

    Windows::Globalization::Language^ defaultLanguage = SpeechRecognizer::SystemSpeechLanguage;
    auto supportedLanguages = SpeechRecognizer::SupportedGrammarLanguages;
    std::for_each(begin(supportedLanguages), end(supportedLanguages), [&](Windows::Globalization::Language^ lang)
    {
        ComboBoxItem^ item = ref new ComboBoxItem();
        item->Tag = lang;
        item->Content = lang->DisplayName;

        cbLanguageSelection->Items->Append(item);
        if (lang->LanguageTag == defaultLanguage->LanguageTag)
        {
            item->IsSelected = true;
            cbLanguageSelection->SelectedItem = item;
        }
    });
    cbLanguageSelectionSelectionChangedToken = cbLanguageSelection->SelectionChanged +=
        ref new SelectionChangedEventHandler(this, &Scenario_SRGSConstraint::cbLanguageSelection_SelectionChanged);
}


/// <summary>
/// Re-initialize the recognizer based on selections from the language combobox.
/// </summary>
void Scenario_SRGSConstraint::cbLanguageSelection_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    ComboBoxItem^ item = (ComboBoxItem^)(cbLanguageSelection->SelectedItem);
    Windows::Globalization::Language^ newLanguage = (Windows::Globalization::Language^)item->Tag;

    if (this->speechRecognizer != nullptr)
    {
        if (speechRecognizer->CurrentLanguage == newLanguage)
        {
            return;
        }
    }

    try
    {
        speechContext->Languages = ref new VectorView<String^>(1, newLanguage->LanguageTag);

        InitializeRecognizer(newLanguage);
    }
    catch (Exception^ exception)
    {
        auto messageDialog = ref new Windows::UI::Popups::MessageDialog(exception->Message, "Exception");
        create_task(messageDialog->ShowAsync());
    }
}
