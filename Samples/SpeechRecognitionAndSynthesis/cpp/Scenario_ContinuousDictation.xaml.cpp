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
#include "Scenario_ContinuousDictation.xaml.h"
#include "AudioCapturePermissions.h"

using namespace SDKTemplate;
using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
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

Scenario_ContinuousDictation::Scenario_ContinuousDictation() : rootPage(MainPage::Current)
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
void Scenario_ContinuousDictation::OnNavigatedTo(NavigationEventArgs^ e)
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

			PopulateLanguageDropdown();
			InitializeRecognizer(SpeechRecognizer::SystemSpeechLanguage);
        }
        else
        {
            this->dictationTextBox->Text = "Permission to access capture resources was not given by the user; please set the application setting in Settings->Privacy->Microphone.";
			this->cbLanguageSelection->IsEnabled = false;
        }
	});
}

/// <summary>
/// Creates a SpeechRecognizer instance and initializes the grammar.
/// </summary>
void Scenario_ContinuousDictation::InitializeRecognizer(Windows::Globalization::Language^ recognizerLanguage)
{
    // If reinitializing the recognizer (ie, changing the speech language), clean up the old recognizer first.
    // Avoid doing this while the recognizer is active by disabling the ability to change languages while listening.
    if (this->speechRecognizer != nullptr)
    {
        speechRecognizer->StateChanged -= stateChangedToken;
        speechRecognizer->ContinuousRecognitionSession->Completed -= continuousRecognitionCompletedToken;
        speechRecognizer->ContinuousRecognitionSession->ResultGenerated -= continuousRecognitionResultGeneratedToken;
        speechRecognizer->HypothesisGenerated -= hypothesisGeneratedToken;

        delete this->speechRecognizer;
        this->speechRecognizer = nullptr;
    }

    this->speechRecognizer = ref new SpeechRecognizer(recognizerLanguage);

    // Provide feedback to the user about the state of the recognizer. This can be used to provide visual feedback in the form
    // of an audio indicator to help the user understand whether they're being heard.
    stateChangedToken = speechRecognizer->StateChanged +=
        ref new TypedEventHandler<
        SpeechRecognizer ^,
        SpeechRecognizerStateChangedEventArgs ^>(
            this,
            &Scenario_ContinuousDictation::SpeechRecognizer_StateChanged);

    // Apply the dictation topic constraint to optimize for dictated freeform speech.
    auto dictationConstraint = ref new SpeechRecognitionTopicConstraint(SpeechRecognitionScenario::Dictation, "dictation");
    speechRecognizer->Constraints->Append(dictationConstraint);

    create_task(speechRecognizer->CompileConstraintsAsync(), task_continuation_context::use_current())
        .then([this](task<SpeechRecognitionCompilationResult^> previousTask)
    {
        SpeechRecognitionCompilationResult^ compilationResult = previousTask.get();

        if (compilationResult->Status != SpeechRecognitionResultStatus::Success)
        {
            btnContinuousRecognize->IsEnabled = false;

            // Let the user know that the grammar didn't compile properly.
            rootPage->NotifyUser("Grammar Compilation Failed: " + compilationResult->Status.ToString(), NotifyType::ErrorMessage);
        }

        // Handle continuous recognition events. Completed fires when various error states occur. ResultGenerated fires when
        // some recognized phrases occur, or the garbage rule is hit. HypothesisGenerated fires during recognition, and
        // allows us to provide incremental feedback based on what the user's currently saying.
        continuousRecognitionCompletedToken = speechRecognizer->ContinuousRecognitionSession->Completed +=
            ref new TypedEventHandler<
            SpeechContinuousRecognitionSession ^,
            SpeechContinuousRecognitionCompletedEventArgs ^>(
                this,
                &Scenario_ContinuousDictation::ContinuousRecognitionSession_Completed);
        continuousRecognitionResultGeneratedToken = speechRecognizer->ContinuousRecognitionSession->ResultGenerated +=
            ref new TypedEventHandler<
            SpeechContinuousRecognitionSession ^,
            SpeechContinuousRecognitionResultGeneratedEventArgs ^>(
                this,
                &Scenario_ContinuousDictation::ContinuousRecognitionSession_ResultGenerated);
        hypothesisGeneratedToken = speechRecognizer->HypothesisGenerated +=
            ref new TypedEventHandler<
            SpeechRecognizer ^,
            SpeechRecognitionHypothesisGeneratedEventArgs ^>(
                this,
                &Scenario_ContinuousDictation::SpeechRecognizer_HypothesisGenerated);


    }, task_continuation_context::use_current());
}

/// <summary>
/// Upon leaving, clean up the speech recognizer. Ensure we aren't still listening, and disable the event 
/// handlers to prevent leaks.
/// </summary>
/// <param name="e">Unused navigation parameters.</param>
void Scenario_ContinuousDictation::OnNavigatedFrom(NavigationEventArgs^ e)
{
    Page::OnNavigatedFrom(e);

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

        cleanupTask.then([this]() {
            DictationButtonText->Text = " Dictate";
            dictationTextBox->Text = "";

            speechRecognizer->StateChanged -= stateChangedToken;
            speechRecognizer->ContinuousRecognitionSession->Completed -= continuousRecognitionCompletedToken;
            speechRecognizer->ContinuousRecognitionSession->ResultGenerated -= continuousRecognitionResultGeneratedToken;
            speechRecognizer->HypothesisGenerated -= hypothesisGeneratedToken;

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
void Scenario_ContinuousDictation::ContinuousRecognize_Click(Object^ sender, RoutedEventArgs^ e)
{
	btnContinuousRecognize->IsEnabled = false;
    // The recognizer can only start listening in a continuous fashion if the recognizer is currently idle.
    // This prevents an exception from occurring.
    if (speechRecognizer->State == SpeechRecognizerState::Idle)
    {
        DictationButtonText->Text = " Stop Dictation";
        cbLanguageSelection->IsEnabled = false;
        hlOpenPrivacySettings->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        discardedTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

        try
        {
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
                    DictationButtonText->Text = " Dictate";
                    cbLanguageSelection->IsEnabled = true;

                    auto messageDialog = ref new Windows::UI::Popups::MessageDialog(exception->Message, "Exception");
                    create_task(messageDialog->ShowAsync());
                }
            }).then([this]() {
				btnContinuousRecognize->IsEnabled = true;
			});
        }
        catch (COMException^ exception)
        {
            if ((unsigned int)exception->HResult == HResultPrivacyStatementDeclined)
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
    else
    {
        DictationButtonText->Text = " Dictate";
        cbLanguageSelection->IsEnabled = true;


        // Cancelling recognition prevents any currently recognized speech from
        // generating a ResultGenerated event. StopAsync() will allow the final session to 
        // complete.
        create_task(speechRecognizer->ContinuousRecognitionSession->StopAsync(), task_continuation_context::use_current())
            .then([this]()
        {

            // Ensure we don't leave any hypothesis text behind
            dictationTextBox->Text = ref new Platform::String(this->dictatedTextBuilder.str().c_str());
        }).then([this]() {
			btnContinuousRecognize->IsEnabled = true;
		});
    }
}

/// <summary> 
/// Clear text in the text box on demand.
/// </summary>
void Scenario_ContinuousDictation::btnClearText_Click(Object^ sender, RoutedEventArgs^ e)
{
    btnClearText->IsEnabled = false;
    this->dictatedTextBuilder.str(L"");
    this->dictatedTextBuilder.clear();
    dictationTextBox->Text = "";
    discardedTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

    btnContinuousRecognize->Focus(Windows::UI::Xaml::FocusState::Programmatic);
}

/// <summary>
/// Automatically scroll the textbox down to the bottom whenever new dictated text arrives
/// </summary>
/// <param name="sender">The dictation textbox</param>
/// <param name="e">Unused text changed arguments</param>
void Scenario_ContinuousDictation::dictationTextBox_TextChanged(Object^ sender, TextChangedEventArgs^ e)
{
    auto grid = static_cast<Grid^>(VisualTreeHelper::GetChild(dictationTextBox, 0));
    for (int i = 0; i < VisualTreeHelper::GetChildrenCount(grid) - 1; i++)
    {
        ScrollViewer^ scroller = dynamic_cast<ScrollViewer^>(VisualTreeHelper::GetChild(grid, i));
        if (scroller != nullptr)
        {
            scroller->ChangeView(0.0, scroller->ExtentHeight, 1.0f);
            break;
        }
    }
}


/// <summary>
/// Provide feedback to the user based on whether the recognizer is receiving their voice input.
/// </summary>
/// <param name="sender">The recognizer that is currently running.</param>
/// <param name="args">The current state of the recognizer.</param>
void Scenario_ContinuousDictation::SpeechRecognizer_StateChanged(SpeechRecognizer ^sender, SpeechRecognizerStateChangedEventArgs ^args)
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
void Scenario_ContinuousDictation::ContinuousRecognitionSession_Completed(SpeechContinuousRecognitionSession ^sender, SpeechContinuousRecognitionCompletedEventArgs ^args)
{
    if (args->Status != SpeechRecognitionResultStatus::Success)
    {
        // If TimeoutExceeded occurs, the user has been silent for too long. We can use this to 
        // cancel recognition if the user in dictation mode and walks away from their device, etc.
        // In a global-command type scenario, this timeout won't apply automatically.
        // With dictation (no grammar in place) modes, the default timeout is 20 seconds.
        if (args->Status == SpeechRecognitionResultStatus::TimeoutExceeded)
        {
            dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
            {
                rootPage->NotifyUser("Automatic Time Out of Dictation", NotifyType::StatusMessage);
                DictationButtonText->Text = " Dictate";
                cbLanguageSelection->IsEnabled = true;
                dictationTextBox->Text = ref new Platform::String(this->dictatedTextBuilder.str().c_str());
            }));
        }
        else
        {
            dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
            {
                rootPage->NotifyUser("Continuous Recognition Completed: " + args->Status.ToString(), NotifyType::ErrorMessage);
                DictationButtonText->Text = " Dictate";
                cbLanguageSelection->IsEnabled = true;
            }));
        }
    }
}

/// <summary>
/// Handle events fired when a result is generated. Check for high to medium confidence, and then append the
/// string to the end of the stringbuffer, and replace the content of the textbox with the string buffer, to
/// remove any hypothesis text that may be present.
/// </summary>
/// <param name="sender">The Recognition session that generated this result</param>
/// <param name="args">Details about the recognized speech</param>
void Scenario_ContinuousDictation::ContinuousRecognitionSession_ResultGenerated(SpeechContinuousRecognitionSession ^sender, SpeechContinuousRecognitionResultGeneratedEventArgs ^args)
{
    // We may choose to discard content that has low confidence, as that could indicate that we're picking up
    // noise via the microphone, or someone could be talking out of earshot.
    if (args->Result->Confidence == SpeechRecognitionConfidence::Medium ||
        args->Result->Confidence == SpeechRecognitionConfidence::High)
    {
        this->dictatedTextBuilder << args->Result->Text->Data() << " ";

        dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
        {
            this->discardedTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

            this->dictationTextBox->Text = ref new String(this->dictatedTextBuilder.str().c_str());
            this->btnClearText->IsEnabled = true;
        }));
    }
    else
    {
        dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
        {
            // In some scenarios, a developer may choose to ignore giving the user feedback in this case, if speech
            // is not the primary input mechanism for the application.
            // Here, just remove any hypothesis text by resetting it to the last known good.
            this->dictationTextBox->Text = ref new String(this->dictatedTextBuilder.str().c_str());

            if (!args->Result->Text->IsEmpty())
            {
                std::wstring discardedText = args->Result->Text->Data();

                discardedText = discardedText.size() <= 25 ? discardedText : (discardedText.substr(0, 25) + L"...");

                discardedTextBlock->Text = L"Discarded due to low/rejected Confidence: " + ref new String(discardedText.c_str());
                discardedTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            }

        }));
    }
}

/// <summary>
/// While the user is speaking, update the textbox with the partial sentence of what's being said for user feedback.
/// </summary>
/// <param name="sender">The recognizer that has generated the hypothesis</param>
/// <param name="args">The hypothesis formed</param>
void Scenario_ContinuousDictation::SpeechRecognizer_HypothesisGenerated(SpeechRecognizer ^sender, SpeechRecognitionHypothesisGeneratedEventArgs ^args)
{
    String^ hypothesis = args->Hypothesis->Text;

    std::wstring textBoxContent = dictatedTextBuilder.str() + L" " + hypothesis->Data() + L"...";
    dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, textBoxContent]()
    {
        // Update the textbox with the currently confirmed text, and the hypothesis combined.
        this->dictationTextBox->Text = ref new Platform::String(textBoxContent.c_str());
        this->btnClearText->IsEnabled = true;
    }));
}

/// <summary>
/// Populate language dropdown with supported Dictation languages.
/// </summary>
void Scenario_ContinuousDictation::PopulateLanguageDropdown()
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
void Scenario_ContinuousDictation::cbLanguageSelection_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    if (this->speechRecognizer != nullptr)
    {
        ComboBoxItem^ item = (ComboBoxItem^)(cbLanguageSelection->SelectedItem);
        Windows::Globalization::Language^ newLanguage = (Windows::Globalization::Language^)item->Tag;
        if (speechRecognizer->CurrentLanguage != newLanguage)
        {
            try
            {
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
void Scenario_ContinuousDictation::openPrivacySettings_Click(Hyperlink^ sender, HyperlinkClickEventArgs^ args)
{
    create_task(Windows::System::Launcher::LaunchUriAsync(ref new Uri(L"ms-settings:privacy-speechtyping")));
}
