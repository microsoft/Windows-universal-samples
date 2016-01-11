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
#include "Scenario_ContinuousRecognitionListGrammar.xaml.h"
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

Scenario_ContinuousRecognitionListGrammar::Scenario_ContinuousRecognitionListGrammar() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// Upon entering the scenario, ensure that we have permissions to use the Microphone. This may entail popping up
/// a dialog to the user on Desktop systems. Only enable functionality once we've gained that permission in order to 
/// prevent errors from occurring when using the SpeechRecognizer. If speech is not a primary input mechanism, developers
/// should consider disabling appropriate parts of the UI if the user does not have a recording device, or does not allow 
/// audio input.
/// </summary>
/// <param name="e">Unused navigation parameters</param>
void Scenario_ContinuousRecognitionListGrammar::OnNavigatedTo(NavigationEventArgs^ e)
{
    Page::OnNavigatedTo(e);

    // Keep track of the UI thread dispatcher, as speech events will come in on a separate thread.
    dispatcher = CoreWindow::GetForCurrentThread()->Dispatcher;

    // Prompt the user for permission to access the microphone. This request will only happen
    // once, it will not re-prompt if the user rejects the permission.
    create_task(AudioCapturePermissions::RequestMicrophonePermissionAsync(), task_continuation_context::use_current())
        .then([this](bool permissionGained)
    {
        if (permissionGained)
        {
            this->btnContinuousRecognize->IsEnabled = true;

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
            this->resultTextBlock->Text = L"Permission to access capture resources was not given by the user; please set the application setting in Settings->Privacy->Microphone.";
			this->cbLanguageSelection->IsEnabled = false;
        }
    });
}

/// <summary>
/// Creates a SpeechRecognizer instance and initializes the grammar.
/// </summary>
void Scenario_ContinuousRecognitionListGrammar::InitializeRecognizer(Windows::Globalization::Language^ recognizerLanguage)
{
    // If reinitializing the recognizer (ie, changing the speech language), clean up the old recognizer first.
    // Avoid doing this while the recognizer is active by disabling the ability to change languages while listening.
    if (this->speechRecognizer != nullptr)
    {
        speechRecognizer->StateChanged -= stateChangedToken;
        speechRecognizer->ContinuousRecognitionSession->Completed -= continuousRecognitionCompletedToken;
        speechRecognizer->ContinuousRecognitionSession->ResultGenerated -= continuousRecognitionResultGeneratedToken;

        delete this->speechRecognizer;
        this->speechRecognizer = nullptr;
    }

    try
    {
        this->speechRecognizer = ref new SpeechRecognizer(recognizerLanguage);

        // Provide feedback to the user about the state of the recognizer. This can be used to provide visual feedback in the form
        // of an audio indicator to help the user understand whether they're being heard.
        stateChangedToken = speechRecognizer->StateChanged +=
            ref new TypedEventHandler<
            SpeechRecognizer ^,
            SpeechRecognizerStateChangedEventArgs ^>(
                this,
                &Scenario_ContinuousRecognitionListGrammar::SpeechRecognizer_StateChanged);

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

        // Update the help text in the UI to show localized examples
        String^ uiOptionsText = ref new String(L"Try saying ") +
            speechResourceMap->GetValue("ListGrammarGoHome", speechContext)->ValueAsString + L"', '" +
            speechResourceMap->GetValue("ListGrammarGoToContosoStudio", speechContext)->ValueAsString + L"' or '" +
            speechResourceMap->GetValue("ListGrammarShowMessage", speechContext)->ValueAsString + L"'";
        listGrammarHelpText->Text = 
            speechResourceMap->GetValue("ListGrammarHelpText", speechContext)->ValueAsString + L"\n" +
            uiOptionsText;

        create_task(speechRecognizer->CompileConstraintsAsync(), task_continuation_context::use_current())
            .then([this](task<SpeechRecognitionCompilationResult^> previousTask)
        {
            SpeechRecognitionCompilationResult^ compilationResult = previousTask.get();

            if (compilationResult->Status != SpeechRecognitionResultStatus::Success)
            {
                // Disable the recognition buttons.
                btnContinuousRecognize->IsEnabled = false;

                // Let the user know that the grammar didn't compile properly.
                rootPage->NotifyUser("Grammar Compilation Failed: " + compilationResult->Status.ToString(), NotifyType::ErrorMessage);
            }
            else
            {
                // Handle continuous recognition events. Completed fires when various error states occur. ResultGenerated fires when
                // some recognized phrases occur, or the garbage rule is hit.
                continuousRecognitionCompletedToken = speechRecognizer->ContinuousRecognitionSession->Completed +=
                    ref new TypedEventHandler<
                    SpeechContinuousRecognitionSession ^,
                    SpeechContinuousRecognitionCompletedEventArgs ^>(
                        this,
                        &Scenario_ContinuousRecognitionListGrammar::ContinuousRecognitionSession_Completed);
                continuousRecognitionResultGeneratedToken = speechRecognizer->ContinuousRecognitionSession->ResultGenerated +=
                    ref new TypedEventHandler<
                    SpeechContinuousRecognitionSession ^,
                    SpeechContinuousRecognitionResultGeneratedEventArgs ^>(
                        this,
                        &Scenario_ContinuousRecognitionListGrammar::ContinuousRecognitionSession_ResultGenerated);

                btnContinuousRecognize->IsEnabled = true;

                resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            }
        }, task_continuation_context::use_current());


    }
    catch (Platform::Exception^ ex)
    {
        if ((unsigned int)ex->HResult == HResultRecognizerNotFound)
        {
            btnContinuousRecognize->IsEnabled = false;

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
/// Upon leaving, clean up the speech recognizer. Ensure we aren't still listening, and disable the event 
/// handlers to prevent leaks.
/// </summary>
/// <param name="e">Unused navigation parameters.</param>
void Scenario_ContinuousRecognitionListGrammar::OnNavigatedFrom(NavigationEventArgs^ e)
{
    Page::OnNavigatedFrom(e);

    this->heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    this->resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

    if (speechRecognizer != nullptr)
    {
        // If we're currently active, start a cancellation task, and wait for it to finish before shutting down
        // the recognizer.
        Concurrency::task<void> cleanupTask;
        if (speechRecognizer->State != SpeechRecognizerState::Idle)
        {
            cleanupTask = create_task(speechRecognizer->ContinuousRecognitionSession->CancelAsync(), task_continuation_context::use_current());
        }
        else
        {
            cleanupTask = create_task([]() {}, task_continuation_context::use_current());
        }

        cleanupTask.then([this]()
        {
            ContinuousRecoButtonText->Text = " Continuous Recognition";

            speechRecognizer->StateChanged -= stateChangedToken;
            speechRecognizer->ContinuousRecognitionSession->Completed -= continuousRecognitionCompletedToken;
            speechRecognizer->ContinuousRecognitionSession->ResultGenerated -= continuousRecognitionResultGeneratedToken;

            delete speechRecognizer;
            speechRecognizer = nullptr;
        }, task_continuation_context::use_current());
    }
}

/// <summary>
/// Begin recognition, or finish the recognition session. 
/// </summary>
/// <param name="sender">The button that generated this event</param>
/// <param name="e">Unused event details</param>
void Scenario_ContinuousRecognitionListGrammar::ContinuousRecognize_Click(Object^ sender, RoutedEventArgs^ e)
{
	btnContinuousRecognize->IsEnabled = false;

    // The recognizer can only start listening in a continuous fashion if the recognizer is currently idle.
    // This prevents an exception from occurring.
    if (speechRecognizer->State == SpeechRecognizerState::Idle)
    {
        ContinuousRecoButtonText->Text = " Stop Continuous Recognition";
        cbLanguageSelection->IsEnabled = false;

        create_task(speechRecognizer->ContinuousRecognitionSession->StartAsync(), task_continuation_context::use_current())
            .then([this](task<void> startAsyncTask)
        {
            try
            {
                // Retreive any exceptions that may have been generated by StartAsync.
                startAsyncTask.get();
            }
            catch (Exception^ exception)
            {
                ContinuousRecoButtonText->Text = " Continuous Recognition";
                cbLanguageSelection->IsEnabled = true;

                auto messageDialog = ref new Windows::UI::Popups::MessageDialog(exception->Message, "Exception");
                create_task(messageDialog->ShowAsync());
            }
        }).then([this]() {
			btnContinuousRecognize->IsEnabled = true;
		});
    }
    else
    {
        // Cancelling recognition prevents any currently recognized speech from
        // generating a ResultGenerated event. StopAsync() will allow the final session to 
        // complete.
        ContinuousRecoButtonText->Text = " Continuous Recognition";
        cbLanguageSelection->IsEnabled = true;

        create_task(speechRecognizer->ContinuousRecognitionSession->CancelAsync()).then([this]() {
			btnContinuousRecognize->IsEnabled = true;
		});
    }
}

/// <summary>
/// Provide feedback to the user based on whether the recognizer is receiving their voice input.
/// </summary>
/// <param name="sender">The recognizer that is currently running.</param>
/// <param name="args">The current state of the recognizer.</param>
void Scenario_ContinuousRecognitionListGrammar::SpeechRecognizer_StateChanged(SpeechRecognizer ^sender, SpeechRecognizerStateChangedEventArgs ^args)
{
    dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        rootPage->NotifyUser("Speech recognizer state: " + args->State.ToString(), NotifyType::StatusMessage);
    }));

}

/// <summary>
/// Handle events fired when error conditions occur, such as the microphone becoming unavailable, or if
/// some transient issues occur.
/// </summary>
/// <param name="sender">The continuous recognition session</param>
/// <param name="args">The state of the recognizer</param>
void Scenario_ContinuousRecognitionListGrammar::ContinuousRecognitionSession_Completed(SpeechContinuousRecognitionSession ^sender, SpeechContinuousRecognitionCompletedEventArgs ^args)
{
    if (args->Status != SpeechRecognitionResultStatus::Success)
    {
        dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
        {
            rootPage->NotifyUser("Continuous Recognition Completed: " + args->Status.ToString(), NotifyType::ErrorMessage);
            ContinuousRecoButtonText->Text = " Continuous Recognition";
            cbLanguageSelection->IsEnabled = true;
        }));
    }
}

/// <summary>
/// Handle events fired when a result is generated. This may include a garbage rule that fires when general room noise
/// or side-talk is captured (this will have a confidence of Rejected typically, but may occasionally match a rule with
/// low confidence).
/// </summary>
/// <param name="sender">The Recognition session that generated this result</param>
/// <param name="args">Details about the recognized speech</param>
void Scenario_ContinuousRecognitionListGrammar::ContinuousRecognitionSession_ResultGenerated(SpeechContinuousRecognitionSession ^sender, SpeechContinuousRecognitionResultGeneratedEventArgs ^args)
{
    // The garbage rule will not have a tag associated with it, the other rules will return a string matching the tag provided
    // when generating the grammar.
    String^ tag = L"unknown";
    if (args->Result->Constraint != nullptr)
    {
        tag = args->Result->Constraint->Tag;
    }

    // Developers may decide to use per-phrase confidence levels in order to tune the behavior of their 
    // grammar based on testing.
    if (args->Result->Confidence == SpeechRecognitionConfidence::Medium ||
        args->Result->Confidence == SpeechRecognitionConfidence::High)
    {
        dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args, tag]()
        {
            heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            resultTextBlock->Text = L"Heard: '" + args->Result->Text + L"', Tag( '" + tag + L"', Confidence: " + args->Result->Confidence.ToString() + ")";
        }));
    }
    else
    {
        // In some scenarios, a developer may choose to ignore giving the user feedback in this case, if speech
        // is not the primary input mechanism for the application.
        dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args, tag]()
        {
            heardYouSayTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            resultTextBlock->Text = L"Sorry, I didn't catch that. (Heard: '" + args->Result->Text + L"', (Tag: '" + tag + L"', Confidence: " + args->Result->Confidence.ToString() + L")";
        }));
    }
}

/// <summary>
/// Populate language dropdown with supported Dictation languages.
/// </summary>
void Scenario_ContinuousRecognitionListGrammar::PopulateLanguageDropdown()
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
        ref new SelectionChangedEventHandler(this, &Scenario_ContinuousRecognitionListGrammar::cbLanguageSelection_SelectionChanged);
}

/// <summary>
/// Re-initialize the recognizer based on selections from the language combobox.
/// </summary>
void Scenario_ContinuousRecognitionListGrammar::cbLanguageSelection_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
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