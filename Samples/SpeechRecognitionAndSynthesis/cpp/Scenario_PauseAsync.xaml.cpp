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
#include "Scenario_PauseAsync.xaml.h"
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

Scenario_PauseAsync::Scenario_PauseAsync() : rootPage(MainPage::Current)
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
void Scenario_PauseAsync::OnNavigatedTo(NavigationEventArgs^ e)
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
            this->btnRecognize->IsEnabled = true;
			
			InitializeRecognizer();
        }
        else
        {
            this->resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            this->resultTextBlock->Text = L"Permission to access capture resources was not given by the user; please set the application setting in Settings->Privacy->Microphone.";
        }
    });
}

/// <summary>
/// Creates a SpeechRecognizer instance and initializes the grammar.
/// </summary>
void Scenario_PauseAsync::InitializeRecognizer()
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
        this->speechRecognizer = ref new SpeechRecognizer();

        // Provide feedback to the user about the state of the recognizer. This can be used to provide visual feedback in the form
        // of an audio indicator to help the user understand whether they're being heard.
        stateChangedToken = speechRecognizer->StateChanged +=
            ref new TypedEventHandler<
            SpeechRecognizer ^,
            SpeechRecognizerStateChangedEventArgs ^>(
                this,
                &Scenario_PauseAsync::SpeechRecognizer_StateChanged);

		// It's not valid to pause a list grammar recognizer and recompile the constraints without at least one
		// constraint in place, so create a permanent constraint.
		auto goHomeConstraint = ref new SpeechRecognitionListConstraint(ref new Vector<String^>( { L"Go Home" }), L"gohome");

        // These speech recognition constraints will be added and removed from the recognizer.
        emailConstraint = ref new SpeechRecognitionListConstraint(ref new Vector<String^>({ L"Send email" }), L"email");
        phoneConstraint = ref new SpeechRecognitionListConstraint(ref new Vector<String^>({ L"Call phone" }), L"phone");

        // Build a command-list grammar. Commands should ideally be drawn from a resource file for localization, and 
        // be grouped into tags for alternate forms of the same command.
		speechRecognizer->Constraints->Append(goHomeConstraint);
        speechRecognizer->Constraints->Append(emailConstraint);

        create_task(speechRecognizer->CompileConstraintsAsync(), task_continuation_context::use_current())
            .then([this](task<SpeechRecognitionCompilationResult^> previousTask)
        {
            SpeechRecognitionCompilationResult^ compilationResult = previousTask.get();

            if (compilationResult->Status != SpeechRecognitionResultStatus::Success)
            {
                // Disable the recognition buttons.
                btnRecognize->IsEnabled = false;

                // Let the user know that the grammar didn't compile properly.
                rootPage->NotifyUser(L"Grammar Compilation Failed: " + compilationResult->Status.ToString(), NotifyType::ErrorMessage);
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
                        &Scenario_PauseAsync::ContinuousRecognitionSession_Completed);
                continuousRecognitionResultGeneratedToken = speechRecognizer->ContinuousRecognitionSession->ResultGenerated +=
                    ref new TypedEventHandler<
                    SpeechContinuousRecognitionSession ^,
                    SpeechContinuousRecognitionResultGeneratedEventArgs ^>(
                        this,
                        &Scenario_PauseAsync::ContinuousRecognitionSession_ResultGenerated);

                btnRecognize->IsEnabled = true;

                resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            }
        }, task_continuation_context::use_current());


    }
    catch (Exception^ ex)
    {
        auto messageDialog = ref new Windows::UI::Popups::MessageDialog(ex->Message, L"Exception");
        create_task(messageDialog->ShowAsync());
    }
}

/// <summary>
/// Upon leaving, clean up the speech recognizer. Ensure we aren't still listening, and disable the event 
/// handlers to prevent leaks.
/// </summary>
/// <param name="e">Unused navigation parameters.</param>
void Scenario_PauseAsync::OnNavigatedFrom(NavigationEventArgs^ e)
{
    Page::OnNavigatedFrom(e);

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
            recognizeButtonText->Text = L" Continuous Recognition";

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
void Scenario_PauseAsync::Recognize_Click(Object^ sender, RoutedEventArgs^ e)
{
	EnableUI(false);
    // The recognizer can only start listening in a continuous fashion if the recognizer is currently idle.
    // This prevents an exception from occurring.
    if (speechRecognizer->State == SpeechRecognizerState::Idle)
    {
        create_task(speechRecognizer->ContinuousRecognitionSession->StartAsync(), task_continuation_context::use_current())
            .then([this](task<void> startAsyncTask)
        {
            try
            {
                // Retreive any exceptions that may have been generated by StartAsync.
                startAsyncTask.get();

                recognizeButtonText->Text = L" Stop Continuous Recognition";
                infoBoxes->Visibility = Windows::UI::Xaml::Visibility::Visible;
            }
            catch (Exception^ exception)
            {
                auto messageDialog = ref new Windows::UI::Popups::MessageDialog(exception->Message, L"Exception");
                create_task(messageDialog->ShowAsync());
            }
		}).then([this]() {
			EnableUI(true);
		});
    }
    else
    {
        // Cancelling recognition prevents any currently recognized speech from
        // generating a ResultGenerated event. StopAsync() will allow the final session to 
        // complete.
        recognizeButtonText->Text = L" Continuous Recognition";
        infoBoxes->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

		create_task(speechRecognizer->ContinuousRecognitionSession->CancelAsync()).then([this]() {
			btnRecognize->IsEnabled = true;
			btnEmailGrammar->IsEnabled = false;
			btnPhoneGrammar->IsEnabled = false;
		});
    }
	
}

/// <summary>
/// Adds or removes the 'email' constraint from the recognizer.
/// </summary>
/// <param name="sender">The button that generated this event</param>
/// <param name="e">Unused event details</param>
void Scenario_PauseAsync::EmailButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Update UI, disabling buttons so the user can't interrupt.
    EnableUI(false);
    ULONGLONG start = GetTickCount64();

    try
    {
        // Pause the session. This allows you to update the grammars without stopping the
        // session and potentially losing audio and a recognition.
        create_task(speechRecognizer->ContinuousRecognitionSession->PauseAsync(), task_continuation_context::use_current())
            .then([this](task<void> startAsyncTask)
        {
            // Update the grammar appropriately.
            if (emailButtonText->Text->Equals(L"Remove 'email' grammar"))
            {
                emailButtonText->Text = L"Updating...";
                unsigned int index;
                if (speechRecognizer->Constraints->IndexOf(emailConstraint, &index))
                {
                    speechRecognizer->Constraints->RemoveAt(index);
                }
                newButtonText = L"Add 'email' grammar";
                emailInfoTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            }
            else
            {
                emailButtonText->Text = L"Updating...";
                speechRecognizer->Constraints->Append(emailConstraint);
                newButtonText = L"Remove 'email' grammar";
                emailInfoTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            }

            // Recompile with the new constraints and resume the session again.
            return create_task(speechRecognizer->CompileConstraintsAsync());
        }).then([start, this](task<Windows::Media::SpeechRecognition::SpeechRecognitionCompilationResult^> result)
        {
            SpeechRecognitionCompilationResult^ compilationResult = result.get();

            if (compilationResult->Status != SpeechRecognitionResultStatus::Success)
            {
                // Disable the recognition buttons.
                EnableUI(false);

                // Let the user know that the grammar didn't compile properly.
                resultTextBlock->Text = L"Unable to compile grammar.";

                create_task(speechRecognizer->ContinuousRecognitionSession->CancelAsync());
            }
            else
            {
                speechRecognizer->ContinuousRecognitionSession->Resume();

                // For diagnostic purposes, display the amount of time it took to update the grammar.
                ULONGLONG end = GetTickCount64();
                msTextBlock->Text = L"Grammar update took " + (end - start) + L" ms";

                // Restore the original UI state.
                EnableUI(true);
                emailButtonText->Text = newButtonText;
            }
        }); 
    }
    catch(Exception^ ex)
    {
        auto messageDialog = ref new Windows::UI::Popups::MessageDialog(ex->Message, L"Exception");
        create_task(messageDialog->ShowAsync());
    }
}

/// <summary>
/// Adds or removes the 'phone' constraint from the recognizer.
/// </summary>
/// <param name="sender">The button that generated this event</param>
/// <param name="e">Unused event details</param>
void Scenario_PauseAsync::PhoneButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Update UI, disabling buttons so the user can't interrupt.
    EnableUI(false);
    ULONGLONG start = GetTickCount64();

    try
    {        
        // Pause the session. This allows you to update the grammars without stopping the
        // session and potentially losing audio and a recognition.
        create_task(speechRecognizer->ContinuousRecognitionSession->PauseAsync(), task_continuation_context::use_current())
            .then([this](task<void> startAsyncTask)
        {
            // Update the grammar appropriately.
            if (phoneButtonText->Text->Equals(L"Remove 'phone' grammar"))
            {
                phoneButtonText->Text = L"Updating...";
                unsigned int index;
                if (speechRecognizer->Constraints->IndexOf(phoneConstraint, &index))
                {
                    speechRecognizer->Constraints->RemoveAt(index);
                }
                newButtonText = L"Add 'phone' grammar";
                phoneInfoTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            }
            else
            {
                phoneButtonText->Text = L"Updating...";
                speechRecognizer->Constraints->Append(phoneConstraint);
                newButtonText = L"Remove 'phone' grammar";
                phoneInfoTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            }

            // Recompile with the new constraints and resume the session again.
            return create_task(speechRecognizer->CompileConstraintsAsync());
        }).then([start, this](task<Windows::Media::SpeechRecognition::SpeechRecognitionCompilationResult^> result)
        {
            SpeechRecognitionCompilationResult^ compilationResult = result.get();

            if (compilationResult->Status != SpeechRecognitionResultStatus::Success)
            {
                // Disable the recognition buttons.
                EnableUI(false);

                // Let the user know that the grammar didn't compile properly.
                resultTextBlock->Text = L"Unable to compile grammar.";

                create_task(speechRecognizer->ContinuousRecognitionSession->CancelAsync());
            }
            else
            {
                speechRecognizer->ContinuousRecognitionSession->Resume();

                // For diagnostic purposes, display the amount of time it took to update the grammar.
                ULONGLONG end = GetTickCount64();
                msTextBlock->Text = L"Grammar update took " + (end - start) + " ms";

                // Restore the original UI state.
                EnableUI(true);
                phoneButtonText->Text = newButtonText;
            }
        });
    }
    catch (Exception^ ex)
    {
        auto messageDialog = ref new Windows::UI::Popups::MessageDialog(ex->Message, L"Exception");
        create_task(messageDialog->ShowAsync());
    }
}

/// <summary>
/// Provide feedback to the user based on whether the recognizer is receiving their voice input.
/// </summary>
/// <param name="sender">The recognizer that is currently running.</param>
/// <param name="args">The current state of the recognizer.</param>
void Scenario_PauseAsync::SpeechRecognizer_StateChanged(SpeechRecognizer ^sender, SpeechRecognizerStateChangedEventArgs ^args)
{
    dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        rootPage->NotifyUser(L"Speech recognizer state: " + args->State.ToString(), NotifyType::StatusMessage);
    }));
}

/// <summary>
/// Handle events fired when error conditions occur, such as the microphone becoming unavailable, or if
/// some transient issues occur.
/// </summary>
/// <param name="sender">The continuous recognition session</param>
/// <param name="args">The state of the recognizer</param>
void Scenario_PauseAsync::ContinuousRecognitionSession_Completed(SpeechContinuousRecognitionSession ^sender, SpeechContinuousRecognitionCompletedEventArgs ^args)
{
    if (args->Status != SpeechRecognitionResultStatus::Success)
    {
        dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
        {
            rootPage->NotifyUser(L"Continuous Recognition Completed: " + args->Status.ToString(), NotifyType::ErrorMessage);
            recognizeButtonText->Text = L" Continuous Recognition";

            btnEmailGrammar->IsEnabled = false;
            btnPhoneGrammar->IsEnabled = false;
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
void Scenario_PauseAsync::ContinuousRecognitionSession_ResultGenerated(SpeechContinuousRecognitionSession ^sender, SpeechContinuousRecognitionResultGeneratedEventArgs ^args)
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
            resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            resultTextBlock->Text = L"Heard: '" + args->Result->Text + L"', Tag( '" + tag + L"', Confidence: " + args->Result->Confidence.ToString() + L")";
        }));
    }
    else
    {
        // In some scenarios, a developer may choose to ignore giving the user feedback in this case, if speech
        // is not the primary input mechanism for the application.
        dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args, tag]()
        {
            resultTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
            resultTextBlock->Text = L"Sorry, I didn't catch that. (Heard: '" + args->Result->Text + L"', (Tag: '" + tag + L"', Confidence: " + args->Result->Confidence.ToString() + L")";
        }));
    }
}

/// <summary>
/// Handles enabling/disabling all buttons in the scenario.
/// </summary>
/// <param name="newState">The new setting for the buttons.</param>
void Scenario_PauseAsync::EnableUI(bool newState)
{
    btnRecognize->IsEnabled = newState;
    btnEmailGrammar->IsEnabled = newState;
    btnPhoneGrammar->IsEnabled = newState;
}

