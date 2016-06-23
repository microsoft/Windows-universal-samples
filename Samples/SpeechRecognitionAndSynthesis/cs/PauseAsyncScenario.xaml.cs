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

using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Media.SpeechRecognition;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SpeechAndTTS
{
    public sealed partial class PauseAsyncScenario : Page
    {
        // Reference to the main sample page in order to post status messages.
        private MainPage rootPage;

        // Speech events may come in on a thread other than the UI thread, keep track of the UI thread's
        // dispatcher, so we can update the UI in a thread-safe manner.
        private CoreDispatcher dispatcher;

        // The speech recognizer used throughout this sample.
        private SpeechRecognizer speechRecognizer;

        // Keep track of whether the continuous recognizer is currently running, so it can be cleaned up appropriately.
        private bool isListening;

        // These speech recognition constraints will be added and removed from the recognizer.
        private SpeechRecognitionListConstraint emailConstraint;
        private SpeechRecognitionListConstraint phoneConstraint;

        public PauseAsyncScenario()
        {
            InitializeComponent();
            isListening = false;
        }

        /// <summary>
        /// Upon entering the scenario, ensure that we have permissions to use the Microphone. This may entail popping up
        /// a dialog to the user on Desktop systems. Only enable functionality once we've gained that permission in order to 
        /// prevent errors from occurring when using the SpeechRecognizer. If speech is not a primary input mechanism, developers
        /// should consider disabling appropriate parts of the UI if the user does not have a recording device, or does not allow 
        /// audio input.
        /// </summary>
        /// <param name="e">Unused navigation parameters</param>
        protected async override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // Keep track of the UI thread dispatcher, as speech events will come in on a separate thread.
            dispatcher = CoreWindow.GetForCurrentThread().Dispatcher;

            // Prompt the user for permission to access the microphone. This request will only happen
            // once, it will not re-prompt if the user rejects the permission.
            bool permissionGained = await AudioCapturePermissions.RequestMicrophonePermission();
            if (permissionGained)
            {
                btnRecognize.IsEnabled = true;
                
                await InitializeRecognizer();
            }
            else
            {
                resultTextBlock.Text = "Permission to access capture resources was not given by the user, reset the application setting in Settings->Privacy->Microphone.";
                btnRecognize.IsEnabled = false;
            }
        }

        /// <summary>
        /// Initialize Speech Recognizer and compile constraints.
        /// </summary>
        /// <param name="recognizerLanguage">Language to use for the speech recognizer</param>
        /// <returns>Awaitable task.</returns>
        private async Task InitializeRecognizer()
        {
            if (speechRecognizer != null)
            {
                // cleanup prior to re-initializing this scenario.
                speechRecognizer.ContinuousRecognitionSession.Completed -= ContinuousRecognitionSession_Completed;
                speechRecognizer.ContinuousRecognitionSession.ResultGenerated -= ContinuousRecognitionSession_ResultGenerated;
                speechRecognizer.StateChanged -= SpeechRecognizer_StateChanged;

                speechRecognizer.Dispose();
                speechRecognizer = null;
            }

            speechRecognizer = new SpeechRecognizer();

            // Provide feedback to the user about the state of the recognizer. This can be used to provide visual feedback in the form
            // of an audio indicator to help the user understand whether they're being heard.
            speechRecognizer.StateChanged += SpeechRecognizer_StateChanged;

            // It's not valid to pause a list grammar recognizer and recompile the constraints without at least one
            // constraint in place, so create a permanent constraint.
            var goHomeConstraint = new SpeechRecognitionListConstraint(new List<string>() { "Go Home" }, "gohome");

            // These speech recognition constraints will be added and removed from the recognizer.
            emailConstraint = new SpeechRecognitionListConstraint(new List<string>() { "Send email" }, "email");
            phoneConstraint = new SpeechRecognitionListConstraint(new List<string>() { "Call phone" }, "phone");

            // Add some of the constraints initially, so we don't start with an empty list of constraints.
            speechRecognizer.Constraints.Add(goHomeConstraint);
            speechRecognizer.Constraints.Add(emailConstraint);

            SpeechRecognitionCompilationResult result = await speechRecognizer.CompileConstraintsAsync();
            if (result.Status != SpeechRecognitionResultStatus.Success)
            {
                // Disable the recognition buttons.
                btnRecognize.IsEnabled = false;

                // Let the user know that the grammar didn't compile properly.
                resultTextBlock.Text = "Unable to compile grammar.";
            }

            // Handle continuous recognition events. Completed fires when various error states occur. ResultGenerated fires when
            // some recognized phrases occur, or the garbage rule is hit.
            speechRecognizer.ContinuousRecognitionSession.Completed += ContinuousRecognitionSession_Completed;
            speechRecognizer.ContinuousRecognitionSession.ResultGenerated += ContinuousRecognitionSession_ResultGenerated;
        }

        /// <summary>
        /// Upon leaving, clean up the speech recognizer. Ensure we aren't still listening, and disable the event 
        /// handlers to prevent leaks.
        /// </summary>
        /// <param name="e">Unused navigation parameters.</param>
        protected async override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (speechRecognizer != null)
            {
                if (isListening)
                {
                    await speechRecognizer.ContinuousRecognitionSession.CancelAsync();
                    isListening = false;
                }

                speechRecognizer.ContinuousRecognitionSession.Completed -= ContinuousRecognitionSession_Completed;
                speechRecognizer.ContinuousRecognitionSession.ResultGenerated -= ContinuousRecognitionSession_ResultGenerated;
                speechRecognizer.StateChanged -= SpeechRecognizer_StateChanged;

                speechRecognizer.Dispose();
                speechRecognizer = null;
            }
        }

        /// <summary>
        /// Handle events fired when error conditions occur, such as the microphone becoming unavailable, or if
        /// some transient issues occur.
        /// </summary>
        /// <param name="sender">The continuous recognition session</param>
        /// <param name="args">The state of the recognizer</param>
        private async void ContinuousRecognitionSession_Completed(SpeechContinuousRecognitionSession sender, SpeechContinuousRecognitionCompletedEventArgs args)
        {
            if (args.Status != SpeechRecognitionResultStatus.Success)
            {
                await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    rootPage.NotifyUser("Continuous Recognition Completed: " + args.Status.ToString(), NotifyType.StatusMessage);
                    recognizeButtonText.Text = " Continuous Recognition";
                    btnEmailGrammar.IsEnabled = false;
                    btnPhoneGrammar.IsEnabled = false;
                    isListening = false;
                });
            }
        }

        /// <summary>
        /// Handle events fired when a result is generated. This may include a garbage rule that fires when general room noise
        /// or side-talk is captured (this will have a confidence of Rejected typically, but may occasionally match a rule with
        /// low confidence).
        /// </summary>
        /// <param name="sender">The Recognition session that generated this result</param>
        /// <param name="args">Details about the recognized speech</param>
        private async void ContinuousRecognitionSession_ResultGenerated(SpeechContinuousRecognitionSession sender, SpeechContinuousRecognitionResultGeneratedEventArgs args)
        {
            // The garbage rule will not have a tag associated with it, the other rules will return a string matching the tag provided
            // when generating the grammar.
            string tag = "unknown";
            if (args.Result.Constraint != null)
            {
                tag = args.Result.Constraint.Tag;
            }

            // Developers may decide to use per-phrase confidence levels in order to tune the behavior of their 
            // grammar based on testing.
            if (args.Result.Confidence == SpeechRecognitionConfidence.Medium ||
                args.Result.Confidence == SpeechRecognitionConfidence.High)
            {
                await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    resultTextBlock.Text = string.Format("Heard: '{0}', (Tag: '{1}', Confidence: {2})", args.Result.Text, tag, args.Result.Confidence.ToString());
                });
            }
            else
            {
                // In some scenarios, a developer may choose to ignore giving the user feedback in this case, if speech
                // is not the primary input mechanism for the application.
                await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    resultTextBlock.Text = string.Format("Sorry, I didn't catch that. (Heard: '{0}', Tag: {1}, Confidence: {2})", args.Result.Text, tag, args.Result.Confidence.ToString());
                });
            }
        }

        /// <summary>
        /// Provide feedback to the user based on whether the recognizer is receiving their voice input.
        /// </summary>
        /// <param name="sender">The recognizer that is currently running.</param>
        /// <param name="args">The current state of the recognizer.</param>
        private async void SpeechRecognizer_StateChanged(SpeechRecognizer sender, SpeechRecognizerStateChangedEventArgs args)
        {
            await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser(args.State.ToString(), NotifyType.StatusMessage);
            });
        }

        /// <summary>
        /// Begin recognition or finish the recognition session. 
        /// </summary>
        /// <param name="sender">The button that generated this event</param>
        /// <param name="e">Unused event details</param>
        public async void Recognize_Click(object sender, RoutedEventArgs e)
        {
            UpdateUI(false);
            if (isListening == false)
            {
                // The recognizer can only start listening in a continuous fashion if the recognizer is currently idle.
                // This prevents an exception from occurring.
                if (speechRecognizer.State == SpeechRecognizerState.Idle)
                {
                    // Reset the text to prompt the user.
                    try
                    {
                        await speechRecognizer.ContinuousRecognitionSession.StartAsync();

                        recognizeButtonText.Text = " Stop Continuous Recognition";
                        UpdateUI(true);
                        infoBoxes.Visibility = Visibility.Visible;
                        isListening = true;
                    }
                    catch (Exception ex)
                    {
                        var messageDialog = new Windows.UI.Popups.MessageDialog(ex.Message, "Exception");
                        await messageDialog.ShowAsync();
                    }
                }
            }
            else
            {
                recognizeButtonText.Text = " Continuous Recognition";
                infoBoxes.Visibility = Visibility.Collapsed;
                isListening = false;

                if (speechRecognizer.State != SpeechRecognizerState.Idle)
                {
                    // Cancelling recognition prevents any currently recognized speech from
                    // generating a ResultGenerated event. StopAsync() will allow the final session to 
                    // complete.
                    await speechRecognizer.ContinuousRecognitionSession.CancelAsync();
                }

                btnEmailGrammar.IsEnabled = false;
                btnPhoneGrammar.IsEnabled = false;
                btnRecognize.IsEnabled = true;
            }
            
        }

        /// <summary>
        /// Toggle button state while operations are in progress.
        /// </summary>
        /// <param name="state">State to set the buttons to</param>
        private void ToggleButtons(bool state)
        {
            btnRecognize.IsEnabled = state;
            btnEmailGrammar.IsEnabled = state;
            btnPhoneGrammar.IsEnabled = state;
        }

        /// <summary>
        /// Adds or removes the 'email' constraint from the recognizer.
        /// </summary>
        /// <param name="sender">The button that generated this event</param>
        /// <param name="e">Unused event details</param>
        public async void EmailButton_Click(object sender, RoutedEventArgs e)
        {
            // Update UI, disabling buttons so the user can't interrupt.
            UpdateUI(false);
            string newButtonText = "";

            long start = DateTime.Now.Ticks;

            try
            {
                // Pause the session. This allows you to update the grammars without stopping the
                // session and potentially losing audio and a recognition.
                await speechRecognizer.ContinuousRecognitionSession.PauseAsync();

                // Update the grammar appropriately.
                if (emailButtonText.Text.Equals("Remove 'email' grammar"))
                {
                    emailButtonText.Text = "Updating...";
                    speechRecognizer.Constraints.Remove(emailConstraint);
                    newButtonText = "Add 'email' grammar";
                    emailInfoTextBlock.Visibility = Visibility.Collapsed;
                }
                else
                {
                    emailButtonText.Text = "Updating...";
                    speechRecognizer.Constraints.Add(emailConstraint);
                    newButtonText = "Remove 'email' grammar";
                    emailInfoTextBlock.Visibility = Visibility.Visible;
                }
                
                // Recompile with the new constraints and resume the session again.
                SpeechRecognitionCompilationResult result = await speechRecognizer.CompileConstraintsAsync();
                if (result.Status != SpeechRecognitionResultStatus.Success)
                {
                    // Disable the recognition buttons.
                    UpdateUI(false);

                    // Let the user know that the grammar didn't compile properly.
                    resultTextBlock.Text = "Unable to compile grammar.";

                    await speechRecognizer.ContinuousRecognitionSession.CancelAsync();
                    return;
                }

                speechRecognizer.ContinuousRecognitionSession.Resume();

                // For diagnostic purposes, display the amount of time it took to update the grammar.
                long elapsed = (DateTime.Now.Ticks - start) / TimeSpan.TicksPerMillisecond;
                msTextBlock.Text = "Grammar update took " + elapsed + " ms";
            }
            catch (Exception ex)
            {
                var messageDialog = new Windows.UI.Popups.MessageDialog(ex.Message, "Exception");
                await messageDialog.ShowAsync();
            }

            // Restore the original UI state.
            UpdateUI(true);
            emailButtonText.Text = newButtonText;
        }

        /// <summary>
        /// Adds or removes the 'phone' constraint from the recognizer.
        /// </summary>
        /// <param name="sender">The button that generated this event</param>
        /// <param name="e">Unused event details</param>
        public async void PhoneButton_Click(object sender, RoutedEventArgs e)
        {
            // Update UI, disabling buttons so the user can't interrupt.
            UpdateUI(false);
            string newButtonText = "";

            long start = DateTime.Now.Ticks;

            try
            {
                // Pause the session. This allows you to update the grammars without stopping the
                // session and potentially losing audio and a recognition.
                await speechRecognizer.ContinuousRecognitionSession.PauseAsync();

                // Update the grammar appropriately.
                if (phoneButtonText.Text.Equals("Remove 'phone' grammar"))
                {
                    phoneButtonText.Text = "Updating...";
                    speechRecognizer.Constraints.Remove(phoneConstraint);
                    newButtonText = "Add 'phone' grammar";
                    phoneInfoTextBlock.Visibility = Visibility.Collapsed;
                }
                else
                {
                    phoneButtonText.Text = "Updating...";
                    speechRecognizer.Constraints.Add(phoneConstraint);
                    newButtonText = "Remove 'phone' grammar";
                    phoneInfoTextBlock.Visibility = Visibility.Visible;
                }

                // Recompile with the new constraints and resume the session again.
                SpeechRecognitionCompilationResult result = await speechRecognizer.CompileConstraintsAsync();
                if (result.Status != SpeechRecognitionResultStatus.Success)
                {
                    // Disable the recognition buttons.
                    UpdateUI(false);

                    // Let the user know that the grammar didn't compile properly.
                    resultTextBlock.Text = "Unable to compile grammar.";

                    await speechRecognizer.ContinuousRecognitionSession.CancelAsync();
                    return;
                }

                speechRecognizer.ContinuousRecognitionSession.Resume();

                // For diagnostic purposes, display the amount of time it took to update the grammar.
                long elapsed = (DateTime.Now.Ticks - start) / TimeSpan.TicksPerMillisecond;
                msTextBlock.Text = "Grammar update took " + elapsed + " ms";
            }
            catch (Exception ex)
            {
                var messageDialog = new Windows.UI.Popups.MessageDialog(ex.Message, "Exception");
                await messageDialog.ShowAsync();
            }

            // Restore the original UI state.
            UpdateUI(true);
            phoneButtonText.Text = newButtonText;
        }

        private void UpdateUI(bool newState)
        {
            btnRecognize.IsEnabled = newState;
            btnEmailGrammar.IsEnabled = newState;
            btnPhoneGrammar.IsEnabled = newState;
        }
    }
}