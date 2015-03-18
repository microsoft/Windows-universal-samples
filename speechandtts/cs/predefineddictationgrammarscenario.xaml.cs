//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate;
using System;
using Windows.Media.SpeechRecognition;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SpeechAndTTS
{
    public sealed partial class PredefinedDictationGrammarScenario : Page
    {
        /// <summary>
        /// This HResult represents the scenario where a user is prompted to allow in-app speech, but 
        /// declines. This should only happen on a Phone device, where speech is enabled for the entire device,
        /// not per-app.
        /// </summary>
        private static uint HResultPrivacyStatementDeclined = 0x80045509;

        private SpeechRecognizer speechRecognizer;
        private CoreDispatcher dispatcher;

        public PredefinedDictationGrammarScenario()
        {
            this.InitializeComponent();

            // Save the UI thread dispatcher to allow speech status messages to be shown on the UI.
            dispatcher = CoreWindow.GetForCurrentThread().Dispatcher;
        }

        /// <summary>
        /// When activating the scenario, ensure we have permission from the user to access their microphone, and
        /// provide an appropriate path for the user to enable access to the microphone if they haven't
        /// given explicit permission for it.
        /// Construct a speech recognizer using the default dictation grammar.
        /// </summary>
        /// <param name="e">The navigation event details</param>
        protected async override void OnNavigatedTo(NavigationEventArgs e)
        {
            bool permissionGained = await AudioCapturePermissions.RequestMicrophonePermission();
            if(permissionGained)
            {
                // enable the recognition buttons
                btnRecognizeWithUI.IsEnabled = true;
                btnRecognizeWithoutUI.IsEnabled = true;
            }
            else
            {
                this.resultTextBlock.Visibility = Visibility.Visible;
                this.resultTextBlock.Text = "Permission to access capture resources was not given by the user, reset the application setting in Settings->Privacy->Microphone.";
            }

            // Create an instance of SpeechRecognizer.
            this.speechRecognizer = new SpeechRecognizer();

            // Provide feedback to the user about the state of the recognizer.
            speechRecognizer.StateChanged += SpeechRecognizer_StateChanged;

            // Compile the dictation grammar that is loaded by default.
            await speechRecognizer.CompileConstraintsAsync();
        }

        /// <summary>
        /// Ensure that we clean up any state tracking event handlers created in OnNavigatedTo to prevent leaks.
        /// </summary>
        /// <param name="e">Details about the navigation event</param>
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            speechRecognizer.StateChanged -= SpeechRecognizer_StateChanged;
        }

        /// <summary>
        /// Handle SpeechRecognizer state changed events by updating a UI component.
        /// </summary>
        /// <param name="sender">Speech recognizer that generated this status event.</param>
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
        /// <param name="sender">The page containing the button this handler is attached to</param>
        /// <param name="e">State information about the routed event</param>
        private async void RecognizeWithUIDictationGrammar_Click(object sender, RoutedEventArgs e)
        {
            this.heardYouSayTextBlock.Visibility = this.resultTextBlock.Visibility = Visibility.Collapsed;

            // Start recognition.
            try
            {
                SpeechRecognitionResult speechRecognitionResult = await speechRecognizer.RecognizeWithUIAsync();
                // If successful, display the recognition result.
                if (speechRecognitionResult.Status == SpeechRecognitionResultStatus.Success)
                {
                    this.heardYouSayTextBlock.Visibility = this.resultTextBlock.Visibility = Visibility.Visible;
                    this.heardYouSayTextBlock.Text = "Heard you say";
                    this.resultTextBlock.Text = speechRecognitionResult.Text;
                }
            }
            catch(NotImplementedException)
            {
                this.resultTextBlock.Visibility = Visibility.Visible;
                this.resultTextBlock.Text = "RecognizeWithUIAsync() is currently not implemented on desktop.";
            }
            catch (Exception exception)
            {
                if ((uint)exception.HResult == HResultPrivacyStatementDeclined)
                {
                    this.resultTextBlock.Visibility = Visibility.Visible;
                    this.resultTextBlock.Text = "The privacy statement was declined.";
                }
                else
                {
                    var messageDialog = new Windows.UI.Popups.MessageDialog(exception.Message, "Exception");
                    await messageDialog.ShowAsync();
                }
            }
            btnRecognizeWithoutUI.IsEnabled = true;
            btnRecognizeWithUI.IsEnabled = true;
        }

        /// <summary>
        /// Uses the recognizer constructed earlier to listen for speech from the user before displaying 
        /// it back on the screen. Uses developer-provided UI for user feedback.
        /// </summary>
        /// <param name="sender">The page containing the button this handler is attached to</param>
        /// <param name="e">State information about the routed event</param>
        private async void RecognizeWithoutUIDictationGrammar_Click(object sender, RoutedEventArgs e)
        {
            this.heardYouSayTextBlock.Visibility = this.resultTextBlock.Visibility = Visibility.Collapsed;

            // Disable the UI while recognition is occurring, and provide feedback to the user about current state.
            btnRecognizeWithUI.IsEnabled = false;
            btnRecognizeWithoutUI.IsEnabled = false;
            listenWithoutUIButtonText.Text = " Listening for speech...";

            // Start recognition.
            try
            {
                SpeechRecognitionResult speechRecognitionResult = await speechRecognizer.RecognizeAsync();
                // If successful, display the recognition result.
                if (speechRecognitionResult.Status == SpeechRecognitionResultStatus.Success)
                {
                    this.heardYouSayTextBlock.Visibility = this.resultTextBlock.Visibility = Visibility.Visible;
                    this.resultTextBlock.Text = speechRecognitionResult.Text;
                }
            }
            catch (Exception exception)
            {
                if ((uint)exception.HResult == HResultPrivacyStatementDeclined)
                {
                    this.resultTextBlock.Visibility = Visibility.Visible;
                    this.resultTextBlock.Text = "The privacy statement was declined.";
                }
                else
                {
                    var messageDialog = new Windows.UI.Popups.MessageDialog(exception.Message, "Exception");
                    await messageDialog.ShowAsync();
                }
            }

            // Reset UI state.
            btnRecognizeWithUI.IsEnabled = true;
            btnRecognizeWithoutUI.IsEnabled = true;
            listenWithoutUIButtonText.Text = " without UI";
        }
    }
}
