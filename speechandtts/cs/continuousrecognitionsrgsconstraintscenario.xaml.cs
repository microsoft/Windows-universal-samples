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
using Windows.ApplicationModel;
using Windows.Media.SpeechRecognition;
using Windows.Storage;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace SpeechAndTTS
{
    public sealed partial class ContinuousRecoSRGSConstraintScenario : Page
    {
        // Reference to the main sample page in order to post status messages.
        private MainPage rootPage;

        // Speech events may come in on a thread other than the UI thread, keep track of the UI thread's
        // dispatcher, so we can update the UI in a thread-safe manner.
        private CoreDispatcher dispatcher;

        // The speech recognizer used throughout this sample.
        private SpeechRecognizer speechRecognizer;

        private Dictionary<string, Color> colorLookup = new Dictionary<string, Color>
        {
            { "red", Colors.Red }, {"blue", Colors.Blue }, {"black",Colors.Black},
            { "brown",Colors.Brown}, {"purple",Colors.Purple}, {"green",Colors.Green},
            { "yellow",Colors.Yellow}, {"cyan",Colors.Cyan}, {"magenta",Colors.Magenta},
            { "orange",Colors.Orange}, {"gray",Colors.Gray}, {"white",Colors.White}
        };

        public ContinuousRecoSRGSConstraintScenario()
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
                btnContinuousRecognize.IsEnabled = true;
            }
            else
            {
                resultTextBlock.Text = "Permission to access capture resources was not given by the user, reset the application setting in Settings->Privacy->Microphone.";
            }

            // Initialize the recognizer. Since the recognizer is disposed on scenario exit, we need to make sure we re-initialize it on scenario
            // entrance, as the xaml page may not get destroyed between openings of the scenario.
            InitializeRecognizer();
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
                if (speechRecognizer.State != SpeechRecognizerState.Idle)
                {
                    await speechRecognizer.ContinuousRecognitionSession.CancelAsync();
                }

                speechRecognizer.ContinuousRecognitionSession.Completed -= ContinuousRecognitionSession_Completed;
                speechRecognizer.ContinuousRecognitionSession.ResultGenerated -= ContinuousRecognitionSession_ResultGenerated;
                speechRecognizer.StateChanged -= SpeechRecognizer_StateChanged;

                this.speechRecognizer.Dispose();
                this.speechRecognizer = null;
            }
        }

        /// <summary>
        /// Creates a SpeechRecognizer instance and initializes the grammar.
        /// </summary>
        private async void InitializeRecognizer()
        {
            // Initialize the SRGS-compliant XML file.
            // For more information about grammars for Windows apps and how to
            // define and use SRGS-compliant grammars in your app, see
            // https://msdn.microsoft.com/en-us/library/dn596121.aspx

            StorageFile grammarContentFile = await Package.Current.InstalledLocation.GetFileAsync(@"SRGSColors.xml");

            // Initialize the SpeechRecognizer and add the grammar.
            speechRecognizer = new SpeechRecognizer();

            // Provide feedback to the user about the state of the recognizer. This can be used to provide
            // visual feedback to help the user understand whether they're being heard.
            speechRecognizer.StateChanged += SpeechRecognizer_StateChanged;

            SpeechRecognitionGrammarFileConstraint grammarConstraint = new SpeechRecognitionGrammarFileConstraint(grammarContentFile);
            speechRecognizer.Constraints.Add(grammarConstraint);
            SpeechRecognitionCompilationResult compilationResult = await speechRecognizer.CompileConstraintsAsync();

            // Check to make sure that the constraints were in a proper format and the recognizer was able to compile them.
            if (compilationResult.Status != SpeechRecognitionResultStatus.Success)
            {
                // Disable the recognition button.
                btnContinuousRecognize.IsEnabled = false;

                // Let the user know that the grammar didn't compile properly.
                resultTextBlock.Text = "Unable to compile grammar.";
            }

            // Set EndSilenceTimeout to give users more time to complete speaking a phrase.
            speechRecognizer.Timeouts.EndSilenceTimeout = TimeSpan.FromSeconds(1.2);

            // Handle continuous recognition events. Completed fires when various error states occur. ResultGenerated fires when
            // some recognized phrases occur, or the garbage rule is hit.
            speechRecognizer.ContinuousRecognitionSession.Completed += ContinuousRecognitionSession_Completed;
            speechRecognizer.ContinuousRecognitionSession.ResultGenerated += ContinuousRecognitionSession_ResultGenerated;
        }

        /// <summary>
        /// Begin recognition or finish the recognition session.
        /// </summary>
        /// <param name="sender">The button that generated this event</param>
        /// <param name="e">Unused event details</param>
        public async void ContinuousRecognize_Click(object sender, RoutedEventArgs e)
        {
            // The recognizer can only start listening in a continuous fashion if the recognizer is currently idle.
            // This prevents an exception from occurring.
            if (speechRecognizer.State == SpeechRecognizerState.Idle)
            {
                // Reset the text to prompt the user.
                resultTextBlock.Text = "Speak to choose colors for the circle, background, and border. Try saying 'blue background, red border, green circle'.";
                ContinuousRecoButtonText.Text = " Stop Continuous Recognition";
                await speechRecognizer.ContinuousRecognitionSession.StartAsync();
            }
            else
            {
                // Reset the text to prompt the user.
                resultTextBlock.Text = "Tap the microphone and speak to choose colors for the circle, background, and border. Try saying 'blue background, red border, green circle'.";
                // Cancelling recognition prevents any currently recognized speech from
                // generating a ResultGenerated event. StopAsync() will allow the final session to 
                // complete.
                await speechRecognizer.ContinuousRecognitionSession.CancelAsync();
            }
        }

        /// <summary>
        /// Handle events fired when the session ends, either from a call to
        /// CancelAsync() or StopAsync(), or an error condition, such as the 
        /// microphone becoming unavailable or some transient issues occuring.
        /// </summary>
        /// <param name="sender">The continuous recognition session</param>
        /// <param name="args">The state of the recognizer</param>
        private async void ContinuousRecognitionSession_Completed(SpeechContinuousRecognitionSession sender, SpeechContinuousRecognitionCompletedEventArgs args)
        {
            await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("Continuous Recognition Completed: " + args.Status.ToString(), NotifyType.StatusMessage);
                ContinuousRecoButtonText.Text = " Continuous Recognition";
            });
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
            // Developers may decide to use per-phrase confidence levels in order to tune the behavior of their 
            // grammar based on testing.
            if (args.Result.Confidence == SpeechRecognitionConfidence.Medium ||
                args.Result.Confidence == SpeechRecognitionConfidence.High)
            {
                await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    HandleRecognitionResult(args.Result);
                });
            }
            // Prompt the user if recognition failed or recognition confidence is low.
            else if (args.Result.Confidence == SpeechRecognitionConfidence.Rejected ||
            args.Result.Confidence == SpeechRecognitionConfidence.Low)
            {
                // In some scenarios, a developer may choose to ignore giving the user feedback in this case, if speech
                // is not the primary input mechanism for the application.
                await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    resultTextBlock.Text = "Sorry, didn't get that \n\nTry saying ->\nblue background\nred border\ngreen circle";
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
        /// Uses the result from the speech recognizer to change the colors of the shapes.
        /// </summary>
        /// <param name="recoResult">The result from the recognition event</param>
        private void HandleRecognitionResult(SpeechRecognitionResult recoResult)
        {
            // Declare a string that will contain messages when the color rule matches GARBAGE.
            string garbagePrompt = "";

            // BACKGROUND: Check to see if the recognition result contains the semantic key for the background color,
            // and not a match for the GARBAGE rule, and change the color.
            if (recoResult.SemanticInterpretation.Properties.ContainsKey("background") && recoResult.SemanticInterpretation.Properties["background"][0].ToString() != "...")
            {
                string backgroundColor = recoResult.SemanticInterpretation.Properties["background"][0].ToString();
                colorRectangle.Fill = new SolidColorBrush(getColor(backgroundColor.ToLower()));
            }

            // If "background" was matched, but the color rule matched GARBAGE, prompt the user.
            else if (recoResult.SemanticInterpretation.Properties.ContainsKey("background") && recoResult.SemanticInterpretation.Properties["background"][0].ToString() == "...")
            {
                garbagePrompt += "Didn't get the background color \n\nTry saying blue background\n";
                resultTextBlock.Text = garbagePrompt;
            }

            // BORDER: Check to see if the recognition result contains the semantic key for the border color,
            // and not a match for the GARBAGE rule, and change the color.
            if (recoResult.SemanticInterpretation.Properties.ContainsKey("border") && recoResult.SemanticInterpretation.Properties["border"][0].ToString() != "...")
            {
                string borderColor = recoResult.SemanticInterpretation.Properties["border"][0].ToString();
                colorRectangle.Stroke = new SolidColorBrush(getColor(borderColor.ToLower()));
            }

            // If "border" was matched, but the color rule matched GARBAGE, prompt the user.
            else if (recoResult.SemanticInterpretation.Properties.ContainsKey("border") && recoResult.SemanticInterpretation.Properties["border"][0].ToString() == "...")
            {
                garbagePrompt += "Didn't get the border color\n\n Try saying red border\n";
                resultTextBlock.Text = garbagePrompt;
            }

            // CIRCLE: Check to see if the recognition result contains the semantic key for the circle color,
            // and not a match for the GARBAGE rule, and change the color.
            if (recoResult.SemanticInterpretation.Properties.ContainsKey("circle") && recoResult.SemanticInterpretation.Properties["circle"][0].ToString() != "...")
            {
                string circleColor = recoResult.SemanticInterpretation.Properties["circle"][0].ToString();
                colorCircle.Fill = new SolidColorBrush(getColor(circleColor.ToLower()));
            }

            // If "circle" was matched, but the color rule matched GARBAGE, prompt the user.
            else if (recoResult.SemanticInterpretation.Properties.ContainsKey("circle") && recoResult.SemanticInterpretation.Properties["circle"][0].ToString() == "...")
            {
                garbagePrompt += "Didn't get the circle color\n\n Try saying green circle\n";
                resultTextBlock.Text = garbagePrompt;
            }

            // Initialize a string that will describe the user's color choices.
            string textBoxColors = "You selected -> \n";

            // Write the color choices contained in the semantics of the recognition result to the text box.
            foreach (KeyValuePair<String, IReadOnlyList<string>> child in recoResult.SemanticInterpretation.Properties)
            {
                // Check to see if any of the semantic values in recognition result contains a match for the GARBAGE rule.
                if (!child.Value.Equals("..."))
                {

                    // Cycle through the semantic keys and values and write them to the text box.
                    textBoxColors += (string.Format(" {0} {1}\n",
                    child.Value[0], child.Key ?? "null"));

                    resultTextBlock.Text = textBoxColors;
                }

                // If there was no match to the colors rule or if it matched GARBAGE, prompt the user.
                else
                {
                    resultTextBlock.Text = garbagePrompt;
                }
            }
        }

        /// <summary>
        /// Creates a color object from the passed in string.
        /// </summary>
        /// <param name="colorString">The name of the color</param>
        private Color getColor(string colorString)
        {
            Color newColor = Colors.Transparent;

            if (colorLookup.ContainsKey(colorString.ToLower()))
            {
                newColor = colorLookup[colorString.ToLower()];
            }

            return newColor;
        }
    }
}
