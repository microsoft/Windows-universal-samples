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
using Windows.ApplicationModel.Resources.Core;
using Windows.Globalization;
using Windows.Media.SpeechRecognition;
using Windows.Storage;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Foundation;

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

        /// <summary>
        /// the HResult 0x8004503a typically represents the case where a recognizer for a particular language cannot
        /// be found. This may occur if the language is installed, but the speech pack for that language is not.
        /// See Settings -> Time & Language -> Region & Language -> *Language* -> Options -> Speech Language Options.
        /// </summary>
        private static uint HResultRecognizerNotFound = 0x8004503a;

        private SpeechRecognizer speechRecognizer;
        private IAsyncOperation<SpeechRecognitionResult> recognitionOperation;
        private CoreDispatcher dispatcher;
        private ResourceContext speechContext;
        private ResourceMap speechResourceMap;

        private Dictionary<string, Color> colorLookup = new Dictionary<string, Color>
        {
            { "COLOR_RED",   Colors.Red },   {"COLOR_BLUE",  Colors.Blue },  {"COLOR_BLACK",  Colors.Black},
            { "COLOR_BROWN", Colors.Brown},  {"COLOR_PURPLE",Colors.Purple}, {"COLOR_GREEN",  Colors.Green},
            { "COLOR_YELLOW",Colors.Yellow}, {"COLOR_CYAN",  Colors.Cyan},   {"COLOR_MAGENTA",Colors.Magenta},
            { "COLOR_ORANGE",Colors.Orange}, {"COLOR_GRAY",  Colors.Gray},   {"COLOR_WHITE",  Colors.White}
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
                
                Language speechLanguage = SpeechRecognizer.SystemSpeechLanguage;
                string langTag = speechLanguage.LanguageTag;
                speechContext = ResourceContext.GetForCurrentView();
                speechContext.Languages = new string[] { langTag };

                speechResourceMap = ResourceManager.Current.MainResourceMap.GetSubtree("LocalizationSpeechResources");

                PopulateLanguageDropdown();
                await InitializeRecognizer(SpeechRecognizer.SystemSpeechLanguage);
            }
            else
            {
                resultTextBlock.Visibility = Visibility.Visible;
                resultTextBlock.Text = "Permission to access capture resources was not given by the user; please set the application setting in Settings->Privacy->Microphone.";

                btnRecognizeWithUI.IsEnabled = false;
                btnRecognizeWithoutUI.IsEnabled = false;
                cbLanguageSelection.IsEnabled = false;
            }
        }

        /// <summary>
        /// Look up the supported languages for this speech recognition scenario, 
        /// that are installed on this machine, and populate a dropdown with a list.
        /// </summary>
        private void PopulateLanguageDropdown()
        {

            // disable the callback so we don't accidentally trigger initialization of the recognizer
            // while initialization is already in progress.
            cbLanguageSelection.SelectionChanged -= cbLanguageSelection_SelectionChanged;
            Language defaultLanguage = SpeechRecognizer.SystemSpeechLanguage;
            IEnumerable<Language> supportedLanguages = SpeechRecognizer.SupportedGrammarLanguages;
            foreach (Language lang in supportedLanguages)
            {
                ComboBoxItem item = new ComboBoxItem();
                item.Tag = lang;
                item.Content = lang.DisplayName;

                cbLanguageSelection.Items.Add(item);
                if (lang.LanguageTag == defaultLanguage.LanguageTag)
                {
                    item.IsSelected = true;
                    cbLanguageSelection.SelectedItem = item;
                }
            }
            
            cbLanguageSelection.SelectionChanged += cbLanguageSelection_SelectionChanged;
        }

        /// <summary>
        /// When a user changes the speech recognition language, trigger re-initialization of the 
        /// speech engine with that language, and change any speech-specific UI assets.
        /// </summary>
        /// <param name="sender">Ignored</param>
        /// <param name="e">Ignored</param>
        private async void cbLanguageSelection_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBoxItem item = (ComboBoxItem)(cbLanguageSelection.SelectedItem);
            Language newLanguage = (Language)item.Tag;

            if (speechRecognizer != null)
            {
                if (speechRecognizer.CurrentLanguage == newLanguage)
                {
                    return;
                }
            }

            // trigger cleanup and re-initialization of speech.
            try
            {
                speechContext.Languages = new string[] { newLanguage.LanguageTag };

                await InitializeRecognizer(newLanguage);
            }
            catch (Exception exception)
            {
                var messageDialog = new Windows.UI.Popups.MessageDialog(exception.Message, "Exception");
                await messageDialog.ShowAsync();
            }
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
                if (speechRecognizer.State != SpeechRecognizerState.Idle)
                {
                    if (recognitionOperation != null)
                    {
                        recognitionOperation.Cancel();
                        recognitionOperation = null;
                    }
                }

                speechRecognizer.StateChanged -= SpeechRecognizer_StateChanged;

                this.speechRecognizer.Dispose();
                this.speechRecognizer = null;
            }
        }

        /// <summary>
        /// Initialize Speech Recognizer and compile constraints.
        /// </summary>
        /// <param name="recognizerLanguage">Language to use for the speech recognizer</param>
        /// <returns>Awaitable task.</returns>
        private async Task InitializeRecognizer(Language recognizerLanguage)
        {
            if (speechRecognizer != null)
            {
                // cleanup prior to re-initializing this scenario.
                speechRecognizer.StateChanged -= SpeechRecognizer_StateChanged;

                this.speechRecognizer.Dispose();
                this.speechRecognizer = null;
            }

            try
            {
                // Initialize the SRGS-compliant XML file.
                // For more information about grammars for Windows apps and how to
                // define and use SRGS-compliant grammars in your app, see
                // https://msdn.microsoft.com/en-us/library/dn596121.aspx

                // determine the language code being used.
                string languageTag = recognizerLanguage.LanguageTag;
                string fileName = String.Format("SRGS\\{0}\\SRGSColors.xml", languageTag);
                StorageFile grammarContentFile = await Package.Current.InstalledLocation.GetFileAsync(fileName);

                // Initialize the SpeechRecognizer and add the grammar.
                speechRecognizer = new SpeechRecognizer(recognizerLanguage);

                // Provide feedback to the user about the state of the recognizer.
                speechRecognizer.StateChanged += SpeechRecognizer_StateChanged;

                // RecognizeWithUIAsync allows developers to customize the prompts.
                speechRecognizer.UIOptions.ExampleText = speechResourceMap.GetValue("SRGSUIOptionsExampleText", speechContext).ValueAsString;

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
                else
                {
                    btnRecognizeWithUI.IsEnabled = true;
                    btnRecognizeWithoutUI.IsEnabled = true;

                    resultTextBlock.Visibility = Visibility.Visible;
                    resultTextBlock.Text = speechResourceMap.GetValue("SRGSListeningPromptText", speechContext).ValueAsString;

                    // Set EndSilenceTimeout to give users more time to complete speaking a phrase.
                    speechRecognizer.Timeouts.EndSilenceTimeout = TimeSpan.FromSeconds(1.2);
                }
            }
            catch (Exception ex)
            {
                if ((uint)ex.HResult == HResultRecognizerNotFound)
                {
                    btnRecognizeWithUI.IsEnabled = false;
                    btnRecognizeWithoutUI.IsEnabled = false;

                    resultTextBlock.Visibility = Visibility.Visible;
                    resultTextBlock.Text = "Speech Language pack for selected language not installed.";
                }
                else
                {
                    var messageDialog = new Windows.UI.Popups.MessageDialog(ex.Message, "Exception");
                    await messageDialog.ShowAsync();
                }
            }
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
            resultTextBlock.Text = speechResourceMap.GetValue("SRGSListeningPromptText", speechContext).ValueAsString;

            // Start recognition.
            try
            {
                recognitionOperation = speechRecognizer.RecognizeWithUIAsync();
                SpeechRecognitionResult speechRecognitionResult = await recognitionOperation;
                if (speechRecognitionResult.Status == SpeechRecognitionResultStatus.Success)
                {
                    HandleRecognitionResult(speechRecognitionResult);
                }
                else
                {
                    resultTextBlock.Visibility = Visibility.Visible;
                    resultTextBlock.Text = string.Format("Speech Recognition Failed, Status: {0}", speechRecognitionResult.Status.ToString());
                }
            }
            catch (TaskCanceledException exception)
            {
                // TaskCanceledException will be thrown if you exit the scenario while the recognizer is actively
                // processing speech. Since this happens here when we navigate out of the scenario, don't try to 
                // show a message dialog for this exception.
                System.Diagnostics.Debug.WriteLine("TaskCanceledException caught while recognition in progress (can be ignored):");
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
            resultTextBlock.Text = speechResourceMap.GetValue("SRGSListeningPromptText", speechContext).ValueAsString;

            // Disable the UI while recognition is occurring, and provide feedback to the user about current state.
            btnRecognizeWithUI.IsEnabled = false;
            btnRecognizeWithoutUI.IsEnabled = false;
            cbLanguageSelection.IsEnabled = false;
            listenWithoutUIButtonText.Text = " listening for speech...";

            // Start recognition.
            try
            {
                recognitionOperation = speechRecognizer.RecognizeAsync();
                SpeechRecognitionResult speechRecognitionResult = await recognitionOperation;
                if (speechRecognitionResult.Status == SpeechRecognitionResultStatus.Success)
                {
                    HandleRecognitionResult(speechRecognitionResult);
                }
                else
                {
                    resultTextBlock.Visibility = Visibility.Visible;
                    resultTextBlock.Text = string.Format("Speech Recognition Failed, Status: {0}", speechRecognitionResult.Status.ToString());
                }
            }
            catch (TaskCanceledException exception)
            {
                // TaskCanceledException will be thrown if you exit the scenario while the recognizer is actively
                // processing speech. Since this happens here when we navigate out of the scenario, don't try to 
                // show a message dialog for this exception.
                System.Diagnostics.Debug.WriteLine("TaskCanceledException caught while recognition in progress (can be ignored):");
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
            cbLanguageSelection.IsEnabled = true;
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
                if (recoResult.SemanticInterpretation.Properties.ContainsKey("KEY_BACKGROUND") && recoResult.SemanticInterpretation.Properties["KEY_BACKGROUND"][0].ToString() != "...")
                {
                    string backgroundColor = recoResult.SemanticInterpretation.Properties["KEY_BACKGROUND"][0].ToString();
                    colorRectangle.Fill = new SolidColorBrush(getColor(backgroundColor));
                }

                // If "background" was matched, but the color rule matched GARBAGE, prompt the user.
                else if (recoResult.SemanticInterpretation.Properties.ContainsKey("KEY_BACKGROUND") && recoResult.SemanticInterpretation.Properties["KEY_BACKGROUND"][0].ToString() == "...")
                {
                    
                    garbagePrompt += speechResourceMap.GetValue("SRGSBackgroundGarbagePromptText", speechContext).ValueAsString;
                    resultTextBlock.Text = garbagePrompt;
                }

                // BORDER: Check to see if the recognition result contains the semantic key for the border color,
                // and not a match for the GARBAGE rule, and change the color.
                if (recoResult.SemanticInterpretation.Properties.ContainsKey("KEY_BORDER") && recoResult.SemanticInterpretation.Properties["KEY_BORDER"][0].ToString() != "...")
                {
                    string borderColor = recoResult.SemanticInterpretation.Properties["KEY_BORDER"][0].ToString();
                    colorRectangle.Stroke = new SolidColorBrush(getColor(borderColor));
                }

                // If "border" was matched, but the color rule matched GARBAGE, prompt the user.
                else if (recoResult.SemanticInterpretation.Properties.ContainsKey("KEY_BORDER") && recoResult.SemanticInterpretation.Properties["KEY_BORDER"][0].ToString() == "...")
                {
                    garbagePrompt += speechResourceMap.GetValue("SRGSBorderGarbagePromptText", speechContext).ValueAsString;
                    resultTextBlock.Text = garbagePrompt;
                }

                // CIRCLE: Check to see if the recognition result contains the semantic key for the circle color,
                // and not a match for the GARBAGE rule, and change the color.
                if (recoResult.SemanticInterpretation.Properties.ContainsKey("KEY_CIRCLE") && recoResult.SemanticInterpretation.Properties["KEY_CIRCLE"][0].ToString() != "...")
                {
                    string circleColor = recoResult.SemanticInterpretation.Properties["KEY_CIRCLE"][0].ToString();
                    colorCircle.Fill = new SolidColorBrush(getColor(circleColor));
                }

                // If "circle" was matched, but the color rule matched GARBAGE, prompt the user.
                else if (recoResult.SemanticInterpretation.Properties.ContainsKey("KEY_CIRCLE") && recoResult.SemanticInterpretation.Properties["KEY_CIRCLE"][0].ToString() == "...")
                {
                    garbagePrompt += speechResourceMap.GetValue("SRGSCircleGarbagePromptText", speechContext).ValueAsString;
                    resultTextBlock.Text = garbagePrompt;
                }

                // Initialize a string that will describe the user's color choices.
                string textBoxColors = "You selected (Semantic Interpretation)-> \n";

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
                resultTextBlock.Text = speechResourceMap.GetValue("SRGSGarbagePromptText", speechContext).ValueAsString;
            }
        }

        /// <summary>
        /// Creates a color object from the passed in string.
        /// </summary>
        /// <param name="colorString">The name of the color</param>
        private Color getColor(string colorString)
        {
            Color newColor = Colors.Transparent;

            if (colorLookup.ContainsKey(colorString))
            {
                newColor = colorLookup[colorString];
            }

            return newColor;
        }
    }
}
