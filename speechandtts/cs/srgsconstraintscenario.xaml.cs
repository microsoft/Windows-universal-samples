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
    public sealed partial class SRGSConstraintScenario : Page
    {
        /// <summary>
        /// This HResult represents the scenario where a user is prompted to allow in-app speech, but
        /// declines. This should only happen on a Phone device, where speech is enabled for the entire device,
        /// not per-app.
        /// </summary>
        private static uint HResultPrivacyStatementDeclined = 0x80045509;

        private SpeechRecognizer speechRecognizer;
        private CoreDispatcher dispatcher;

        private Dictionary<string, Color> colorLookup = new Dictionary<string, Color>
        {
            { "red", Colors.Red }, {"blue", Colors.Blue }, {"black",Colors.Black},
            { "brown",Colors.Brown}, {"purple",Colors.Purple}, {"green",Colors.Green},
            { "yellow",Colors.Yellow}, {"cyan",Colors.Cyan}, {"magenta",Colors.Magenta},
            { "orange",Colors.Orange}, {"gray",Colors.Gray}, {"white",Colors.White}
        };

        public SRGSConstraintScenario()
        {
            InitializeComponent();
        }

        /// <summary>
        /// When activating the scenario, ensure we have permission from the user to access their microphone, and
        /// provide an appropriate path for the user to enable access to the microphone if they haven't
        /// given explicit permission for it.
        /// </summary>
        /// <param name="e">The navigation event details</param>
        protected async override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Save the UI thread dispatcher to allow speech status messages to be shown on the UI.
            dispatcher = CoreWindow.GetForCurrentThread().Dispatcher;

            bool permissionGained = await AudioCapturePermissions.RequestMicrophonePermission();
            if (permissionGained)
            {
                // Enable the recognition buttons.
                btnRecognizeWithUI.IsEnabled = true;
                btnRecognizeWithoutUI.IsEnabled = true;
            }
            else
            {
                resultTextBlock.Visibility = Visibility.Visible;
                resultTextBlock.Text = "Permission to access capture resources was not given by the user; please set the application setting in Settings->Privacy->Microphone.";
            }

            await InitializeRecognizer();
        }


        /// <summary>
        /// Ensure that we clean up any state tracking event handlers created in OnNavigatedTo to prevent leaks.
        /// </summary>
        /// <param name="e">Details about the navigation event</param>
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            if (speechRecognizer != null)
            {
                speechRecognizer.StateChanged -= SpeechRecognizer_StateChanged;

                this.speechRecognizer.Dispose();
                this.speechRecognizer = null;
            }
        }

        /// <summary>
        /// Creates a SpeechRecognizer instance and initializes the grammar.
        /// </summary>
        private async Task InitializeRecognizer()
        {
            // Initialize the SRGS-compliant XML file.
            // For more information about grammars for Windows apps and how to
            // define and use SRGS-compliant grammars in your app, see
            // https://msdn.microsoft.com/en-us/library/dn596121.aspx

            StorageFile grammarContentFile = await Package.Current.InstalledLocation.GetFileAsync(@"SRGSColors.xml");

            // Initialize the SpeechRecognizer and add the grammar.
            speechRecognizer = new SpeechRecognizer();

            // Provide feedback to the user about the state of the recognizer.
            speechRecognizer.StateChanged += SpeechRecognizer_StateChanged;

            // RecognizeWithUIAsync allows developers to customize the prompts.
            speechRecognizer.UIOptions.ExampleText = @"Try ""blue background, red border, green circle"".";

            SpeechRecognitionGrammarFileConstraint grammarConstraint = new SpeechRecognitionGrammarFileConstraint(grammarContentFile);
            speechRecognizer.Constraints.Add(grammarConstraint);
            SpeechRecognitionCompilationResult compilationResult = await speechRecognizer.CompileConstraintsAsync();

            // Check to make sure that the constraints were in a proper format and the recognizer was able to compile it.
            if (compilationResult.Status != SpeechRecognitionResultStatus.Success)
            {
                // Disable the recognition buttons.
                btnRecognizeWithUI.IsEnabled = false;
                btnRecognizeWithoutUI.IsEnabled = false;

                // Let the user know that the grammar didn't compile properly.
                resultTextBlock.Visibility = Visibility.Visible;
                resultTextBlock.Text = "Unable to compile grammar.";
            }

            // Set EndSilenceTimeout to give users more time to complete speaking a phrase.
            speechRecognizer.Timeouts.EndSilenceTimeout = TimeSpan.FromSeconds(1.2);
        }

        /// <summary>
        /// Handle SpeechRecognizer state changed events by updating a UI component.
        /// </summary>
        /// <param name="sender">Speech recognizer that generated this status event</param>
        /// <param name="args">The recognizer's status</param>
        private async void SpeechRecognizer_StateChanged(SpeechRecognizer sender, SpeechRecognizerStateChangedEventArgs args)
        {
            await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                MainPage.Current.NotifyUser("Speech recognizer state: " + args.State.ToString(), NotifyType.StatusMessage);
            });
        }

        /// <summary>
        /// Uses the recognizer constructed earlier to listen for speech from the user before displaying
        /// it back on the screen. Uses the built-in speech recognition UI.
        /// </summary>
        /// <param name="sender">Button that triggered this event</param>
        /// <param name="e">State information about the routed event</param>
        private async void RecognizeWithUI_Click(object sender, RoutedEventArgs e)
        {
            // Reset the text to prompt the user.
            resultTextBlock.Text = "Speak to choose colors for the circle, background, and border. Try saying 'blue background, red border, green circle'.";

            // Start recognition.
            try
            {
                SpeechRecognitionResult speechRecognitionResult = await speechRecognizer.RecognizeWithUIAsync();
                HandleRecognitionResult(speechRecognitionResult);
            }
            catch (ObjectDisposedException exception)
            {
                // ObjectDisposedException will be thrown if you exit the scenario while the recogizer is actively
                // processing speech. Since this happens here when we navigate out of the scenario, don't try to 
                // show a message dialog for this exception.
                System.Diagnostics.Debug.WriteLine("ObjectDisposedException caught while recognition in progress (can be ignored):");
                System.Diagnostics.Debug.WriteLine(exception.ToString());
            }
            catch (Exception exception)
            {
                // Handle the speech privacy policy error.
                if ((uint)exception.HResult == HResultPrivacyStatementDeclined)
                {
                    resultTextBlock.Visibility = Visibility.Visible;
                    resultTextBlock.Text = "The privacy statement was declined.";
                }
                else
                {
                    var messageDialog = new Windows.UI.Popups.MessageDialog(exception.Message, "Exception");
                    await messageDialog.ShowAsync();
                }
            }
        }

        /// <summary>
        /// Uses the recognizer constructed earlier to listen for speech from the user before displaying
        /// it back on the screen. Uses developer-provided UI for user feedback.
        /// </summary>
        /// <param name="sender">Button that triggered this event</param>
        /// <param name="e">State information about the routed event</param>
        private async void RecognizeWithoutUI_Click(object sender, RoutedEventArgs e)
        {
            // Reset the text to prompt the user.
            resultTextBlock.Text = "Speak to choose colors for the circle, background, and border. Try saying 'blue background, red border, green circle'.";

            // Disable the UI while recognition is occurring, and provide feedback to the user about current state.
            btnRecognizeWithUI.IsEnabled = false;
            btnRecognizeWithoutUI.IsEnabled = false;
            listenWithoutUIButtonText.Text = " listening for speech...";

            // Start recognition.
            try
            {
                SpeechRecognitionResult speechRecognitionResult = await speechRecognizer.RecognizeAsync();
                HandleRecognitionResult(speechRecognitionResult);
            }
            catch (ObjectDisposedException exception)
            {
                // ObjectDisposedException will be thrown if you exit the scenario while the recogizer is actively
                // processing speech. Since this happens here when we navigate out of the scenario, don't try to 
                // show a message dialog for this exception.
                System.Diagnostics.Debug.WriteLine("ObjectDisposedException caught while recognition in progress (can be ignored):");
                System.Diagnostics.Debug.WriteLine(exception.ToString());
            }
            catch (Exception exception)
            {
                // Handle the speech privacy policy error.
                if ((uint)exception.HResult == HResultPrivacyStatementDeclined)
                {
                    resultTextBlock.Visibility = Visibility.Visible;
                    resultTextBlock.Text = "The privacy statement was declined.";
                }
                else
                {
                    var messageDialog = new Windows.UI.Popups.MessageDialog(exception.Message, "Exception");
                    await messageDialog.ShowAsync();
                }
            }

            // Reset UI state.
            listenWithoutUIButtonText.Text = " without UI";
            btnRecognizeWithUI.IsEnabled = true;
            btnRecognizeWithoutUI.IsEnabled = true;
        }

        /// <summary>
        /// Uses the result from the speech recognizer to change the colors of the shapes.
        /// </summary>
        /// <param name="recoResult">The result from the recognition event</param>
        private void HandleRecognitionResult(SpeechRecognitionResult recoResult)
        {
            // Check the confidence level of the recognition result.
            if (recoResult.Confidence == SpeechRecognitionConfidence.High ||
            recoResult.Confidence == SpeechRecognitionConfidence.Medium)
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

            // Prompt the user if recognition failed or recognition confidence is low.
            else if (recoResult.Confidence == SpeechRecognitionConfidence.Rejected ||
            recoResult.Confidence == SpeechRecognitionConfidence.Low)
            {
                resultTextBlock.Text = "Sorry, didn't get that \n\nTry saying ->\nblue background\nred border\ngreen circle";
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
