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
#include "Scenario_ListConstraint.xaml.h"
#include "AudioCapturePermissions.h"

using namespace SDKTemplate;
using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::SpeechRecognition;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario_ListConstraint::Scenario_ListConstraint() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// When activating the scenario, ensure we have permission from the user to access their microphone, and
/// provide an appropriate path for the user to enable access to the microphone if they haven't
/// given explicit permission for it. 
/// </summary>
/// <param name="e">The navigation event details</param>
void Scenario_ListConstraint::OnNavigatedTo(NavigationEventArgs^ e)
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
void Scenario_ListConstraint::InitializeRecognizer(Windows::Globalization::Language^ recognizerLanguage)
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
                &Scenario_ListConstraint::SpeechRecognizer_StateChanged);

        // Build a command-list grammar. Commands should ideally be drawn from a resource file for localization, and 
        // be grouped into tags for alternate forms of the same command.
        speechRecognizer->Constraints->Append(
            ref new SpeechRecognitionListConstraint(
                ref new Vector<String^>({
            speechResourceMap->GetValue(L"ListGrammarGoHome", speechContext)->ValueAsString
        }), "Home"));
        speechRecognizer->Constraints->Append(
            ref new SpeechRecognitionListConstraint(
                ref new Vector<String^>({
            speechResourceMap->GetValue(L"ListGrammarGoToContosoStudio", speechContext)->ValueAsString
        }), "GoToContosoStudio"));
        speechRecognizer->Constraints->Append(
            ref new SpeechRecognitionListConstraint(
                ref new Vector<String^>({
            speechResourceMap->GetValue(L"ListGrammarShowMessage", speechContext)->ValueAsString,
            speechResourceMap->GetValue(L"ListGrammarOpenMessage", speechContext)->ValueAsString
        }), "Message"));
        speechRecognizer->Constraints->Append(
            ref new SpeechRecognitionListConstraint(
                ref new Vector<String^>({
            speechResourceMap->GetValue(L"ListGrammarSendEmail", speechContext)->ValueAsString,
            speechResourceMap->GetValue(L"ListGrammarCreateEmail", speechContext)->ValueAsString
        }), "Email"));
        speechRecognizer->Constraints->Append(
            ref new SpeechRecognitionListConstraint(
                ref new Vector<String^>({
            speechResourceMap->GetValue(L"ListGrammarCallNitaFarley", speechContext)->ValueAsString,
            speechResourceMap->GetValue(L"ListGrammarCallNita", speechContext)->ValueAsString
        }), "CallNita"));
        speechRecognizer->Constraints->Append(
            ref new SpeechRecognitionListConstraint(
                ref new Vector<String^>({
            speechResourceMap->GetValue(L"ListGrammarCallWayneSigmon", speechContext)->ValueAsString,
            speechResourceMap->GetValue(L"ListGrammarCallWayne", speechContext)->ValueAsString
        }), "CallWayne"));

        // RecognizeWithUIAsync allows developers to customize the prompts.
        String^ uiOptionsText = ref new String(L"Try saying ") +
            speechResourceMap->GetValue("ListGrammarGoHome", speechContext)->ValueAsString + L"', '" +
            speechResourceMap->GetValue("ListGrammarGoToContosoStudio", speechContext)->ValueAsString + L"' or '" +
            speechResourceMap->GetValue("ListGrammarShowMessage", speechContext)->ValueAsString + L"'";
            
        speechRecognizer->UIOptions->ExampleText = uiOptionsText;
        helpTextBlock->Text = speechResourceMap->GetValue("ListGrammarHelpText", speechContext)->ValueAsString + L"\n" +
            uiOptionsText;

        // Compile the constraint.
        create_task(speechRecognizer->CompileConstraintsAsync(), task_continuation_context::use_current())
            .then([this](task<SpeechRecognitionCompilationResult^> previousTask)
        {
            SpeechRecognitionCompilationResult^ compilationResult = previousTask.get();

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

                resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
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
/// Ensure that we clean up any state tracking event handlers created in OnNavigatedTo to prevent leaks,
/// dipose the speech recognizer, and clean up to ensure the scenario is not still attempting to recognize
/// speech while not in view.
/// </summary>
/// <param name="e">Details about the navigation event</param>
void Scenario_ListConstraint::OnNavigatedFrom(NavigationEventArgs^ e)
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
/// Uses the recognizer constructed earlier to listen for speech from the user before displaying 
/// it back on the screen. Uses the built-in speech recognition UI.
/// </summary>
/// <param name="sender">Button that triggered this event</param>
/// <param name="e">State information about the routed event</param>
void Scenario_ListConstraint::RecognizeWithUIListConstraint_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

    // Start recognition.
    create_task(speechRecognizer->RecognizeWithUIAsync(), task_continuation_context::use_current())
        .then([this](task<SpeechRecognitionResult^> recognitionTask)
    {
        try
        {
            SpeechRecognitionResult^ speechRecognitionResult = recognitionTask.get();

            String^ tag = L"unknown";
            if (speechRecognitionResult->Constraint != nullptr)
            {
                // Only attempt to retreive a constraint tag if the result has one. If 
                // the garbage rule is hit, this may be null.
                tag = speechRecognitionResult->Constraint->Tag;
            }

            // If successful, display the recognition result.
            if (speechRecognitionResult->Status == SpeechRecognitionResultStatus::Success)
            {
                heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                resultTextBlock->Text = L"Heard: '" + speechRecognitionResult->Text + L"', Tag( '" + tag + L"', Confidence: " + speechRecognitionResult->Confidence.ToString() + ")";
            }
            else
            {
                heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
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
            if ((unsigned int)exception->HResult == Scenario_ListConstraint::HResultPrivacyStatementDeclined)
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
void Scenario_ListConstraint::RecognizeWithoutUIListConstraint_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

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

            String^ tag = L"unknown";
            if (speechRecognitionResult->Constraint != nullptr)
            {
                // Only attempt to retreive a constraint tag if the result has one. If 
                // the garbage rule is hit, this may be null.
                tag = speechRecognitionResult->Constraint->Tag;
            }

            // If successful, display the recognition result.
            if (speechRecognitionResult->Status == SpeechRecognitionResultStatus::Success)
            {
                heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                resultTextBlock->Text = L"Heard: '" + speechRecognitionResult->Text + L"', Tag( '" + tag + L"', Confidence: " + speechRecognitionResult->Confidence.ToString() + ")";
            }
            else
            {
                heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
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
            if ((unsigned int)exception->HResult == Scenario_ListConstraint::HResultPrivacyStatementDeclined)
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
void Scenario_ListConstraint::SpeechRecognizer_StateChanged(SpeechRecognizer ^sender, SpeechRecognizerStateChangedEventArgs ^args)
{
    dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        rootPage->NotifyUser("Speech recognizer state: " + args->State.ToString(), NotifyType::StatusMessage);
    }));

}

/// <summary>
/// Populate language dropdown with supported Dictation languages.
/// </summary>
void Scenario_ListConstraint::PopulateLanguageDropdown()
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
        ref new SelectionChangedEventHandler(this, &Scenario_ListConstraint::cbLanguageSelection_SelectionChanged);
}


/// <summary>
/// Re-initialize the recognizer based on selections from the language combobox.
/// </summary>
void Scenario_ListConstraint::cbLanguageSelection_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
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