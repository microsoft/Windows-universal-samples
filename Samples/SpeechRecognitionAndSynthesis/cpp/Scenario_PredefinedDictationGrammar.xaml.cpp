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
#include "Scenario_PredefinedDictationGrammar.xaml.h"
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
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario_PredefinedDictationGrammar::Scenario_PredefinedDictationGrammar() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// When activating the scenario, ensure we have permission from the user to access their microphone, and
/// provide an appropriate path for the user to enable access to the microphone if they haven't
/// given explicit permission for it.
/// </summary>
/// <param name="e">The navigation event details</param>
void Scenario_PredefinedDictationGrammar::OnNavigatedTo(NavigationEventArgs^ e)
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
void Scenario_PredefinedDictationGrammar::InitializeRecognizer(Windows::Globalization::Language^ recognizerLanguage)
{
    // If reinitializing the recognizer (ie, changing the speech language), clean up the old recognizer first.
    // Avoid doing this while the recognizer is active by disabling the ability to change languages while listening.
    if (this->speechRecognizer != nullptr)
    {
        speechRecognizer->StateChanged -= stateChangedToken;

        delete this->speechRecognizer;
        this->speechRecognizer = nullptr;
    }

    // Create an instance of SpeechRecognizer.
    this->speechRecognizer = ref new SpeechRecognizer(recognizerLanguage);

    // Provide feedback to the user about the state of the recognizer.
    stateChangedToken = speechRecognizer->StateChanged +=
        ref new Windows::Foundation::TypedEventHandler<
        SpeechRecognizer ^,
        SpeechRecognizerStateChangedEventArgs ^>(
            this,
            &Scenario_PredefinedDictationGrammar::SpeechRecognizer_StateChanged);

    // Compile the dictation topic constraint, which optimizes for dictated speech.
    auto dictationConstraint = ref new SpeechRecognitionTopicConstraint(SpeechRecognitionScenario::Dictation, "dictation");
    speechRecognizer->Constraints->Append(dictationConstraint);

    // RecognizeWithUIAsync allows developers to customize the prompts.
    speechRecognizer->UIOptions->AudiblePrompt = L"Dictate a phrase or sentence...";
    speechRecognizer->UIOptions->ExampleText = speechResourceMap->GetValue("DictationUIOptionsExampleText", speechContext)->ValueAsString;

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
    }, task_continuation_context::use_current());
}

/// <summary>
/// Ensure that we clean up any state tracking event handlers created in OnNavigatedTo to prevent leaks.
/// </summary>
/// <param name="e">Details about the navigation event</param>
void Scenario_PredefinedDictationGrammar::OnNavigatedFrom(NavigationEventArgs^ e)
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
void Scenario_PredefinedDictationGrammar::RecognizeWithUIDictationGrammar_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;            
    hlOpenPrivacySettings->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

    try
    {
        // Start recognition.
        create_task(speechRecognizer->RecognizeWithUIAsync(), task_continuation_context::use_current())
            .then([this](task<SpeechRecognitionResult^> recognitionTask)
        {
            try
            {
                SpeechRecognitionResult^ speechRecognitionResult = recognitionTask.get();
                // If successful, display the recognition result.
                if (speechRecognitionResult->Status == SpeechRecognitionResultStatus::Success)
                {
                    heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                    resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                    resultTextBlock->Text = speechRecognitionResult->Text;
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
                auto messageDialog = ref new Windows::UI::Popups::MessageDialog(exception->Message, "Exception");
                create_task(messageDialog->ShowAsync());
            }
        });
    }
    catch (COMException^ exception)
    {
        if ((unsigned int)exception->HResult == Scenario_PredefinedDictationGrammar::HResultPrivacyStatementDeclined)
        {
            // If the privacy policy hasn't been accepted, attempting to create a task out of RecognizeAsync fails immediately with a
            // COMException and this specific HResult.
            hlOpenPrivacySettings->Visibility = Windows::UI::Xaml::Visibility::Visible;
        }
        else
        {
            auto messageDialog = ref new Windows::UI::Popups::MessageDialog(exception->Message, "Exception");
            create_task(messageDialog->ShowAsync());
        }
    }
}

/// <summary>
/// Uses the recognizer constructed earlier to listen for speech from the user before displaying 
/// it back on the screen. Uses developer-provided UI for user feedback.
/// </summary>
/// <param name="sender">Button that triggered this event</param>
/// <param name="e">State information about the routed event</param>
void Scenario_PredefinedDictationGrammar::RecognizeWithoutUIDictationGrammar_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    hlOpenPrivacySettings->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

    // Disable the UI while recognition is occurring, and provide feedback to the user about current state.
    btnRecognizeWithUI->IsEnabled = false;
    btnRecognizeWithoutUI->IsEnabled = false;
    listenWithoutUIButtonText->Text = " listening for speech...";
    cbLanguageSelection->IsEnabled = false;

    try
    {
        // Start recognition.
        create_task(speechRecognizer->RecognizeAsync(), task_continuation_context::use_current())
            .then([this](task<SpeechRecognitionResult^> recognitionTask)
        {
            try
            {
                SpeechRecognitionResult^ speechRecognitionResult = recognitionTask.get();
                // If successful, display the recognition result.
                if (speechRecognitionResult->Status == SpeechRecognitionResultStatus::Success)
                {
                    heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                    resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                    resultTextBlock->Text = speechRecognitionResult->Text;
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
                auto messageDialog = ref new Windows::UI::Popups::MessageDialog(exception->Message, "Exception");
                create_task(messageDialog->ShowAsync());
            }

            // Reset UI state.
            listenWithoutUIButtonText->Text = " without UI";
            btnRecognizeWithUI->IsEnabled = true;
            btnRecognizeWithoutUI->IsEnabled = true;
            cbLanguageSelection->IsEnabled = true;
        });
    }
    catch (COMException^ exception)
    {
        if ((unsigned int)exception->HResult == Scenario_PredefinedDictationGrammar::HResultPrivacyStatementDeclined)
        {
            // If the privacy policy hasn't been accepted, attempting to create a task out of RecognizeAsync fails immediately with a
            // COMException and this specific HResult.
            hlOpenPrivacySettings->Visibility = Windows::UI::Xaml::Visibility::Visible;
        }
        else
        {
            auto messageDialog = ref new Windows::UI::Popups::MessageDialog(exception->Message, "Exception");
            create_task(messageDialog->ShowAsync());
        }

        listenWithoutUIButtonText->Text = " without UI";
        btnRecognizeWithUI->IsEnabled = true;
        btnRecognizeWithoutUI->IsEnabled = true;
        cbLanguageSelection->IsEnabled = true;
    }
}

/// <summary>
/// Handle SpeechRecognizer state changed events by updating a UI component.
/// </summary>
/// <param name="sender">Speech recognizer that generated this status event</param>
/// <param name="args">The recognizer's status</param>
void Scenario_PredefinedDictationGrammar::SpeechRecognizer_StateChanged(SpeechRecognizer ^sender, SpeechRecognizerStateChangedEventArgs ^args)
{
    dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        rootPage->NotifyUser("Speech recognizer state: " + args->State.ToString(), NotifyType::StatusMessage);
    }));

}

/// <summary>
/// Populate language dropdown with supported Dictation languages.
/// </summary>
void Scenario_PredefinedDictationGrammar::PopulateLanguageDropdown()
{
    Windows::Globalization::Language^ defaultLanguage = SpeechRecognizer::SystemSpeechLanguage;
    auto supportedLanguages = SpeechRecognizer::SupportedTopicLanguages;
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
}

/// <summary>
/// Re-initialize the recognizer based on selections from the language combobox.
/// </summary>
void Scenario_PredefinedDictationGrammar::cbLanguageSelection_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    if (this->speechRecognizer != nullptr)
    {
        ComboBoxItem^ item = (ComboBoxItem^)(cbLanguageSelection->SelectedItem);
        Windows::Globalization::Language^ newLanguage = (Windows::Globalization::Language^)item->Tag;
        if (speechRecognizer->CurrentLanguage != newLanguage)
        {
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
    }
}

/// <summary>
/// Open the Speech, Inking and Typing page under Settings -> Privacy, enabling a user to accept the 
/// Microsoft Privacy Policy, and enable personalization.
/// </summary>
/// <param name="sender">Ignored</param>
/// <param name="args">Ignored</param>
void Scenario_PredefinedDictationGrammar::openPrivacySettings_Click(Hyperlink^ sender, HyperlinkClickEventArgs^ args)
{
    create_task(Windows::System::Launcher::LaunchUriAsync(ref new Uri(L"ms-settings:privacy-speechtyping")));
}
