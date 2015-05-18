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

using System;
using System.Collections.Generic;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Input.Inking;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Text.Core;
using Windows.Globalization;
using SDKTemplate;

namespace simpleInk
{
    /// <summary>
    /// This page shows the code to do ink recognition
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        const string InstallRecoText = "You can install handwriting recognition engines for other languages by this: go to Settings -> Time and language -> Region and language, choose a language, and click Options, then click Download under Handwriting";

        private MainPage rootPage;
        InkRecognizerContainer inkRecognizerContainer = null;
        private IReadOnlyList<InkRecognizer> recoView = null;
        private Language previousInputLanguage = null;
        private CoreTextServicesManager textServiceManager = null;
        private ToolTip recoTooltip;

        public Scenario2()
        {
            this.InitializeComponent();

            // Initialize drawing attributes. These are used in inking mode.
            InkDrawingAttributes drawingAttributes = new InkDrawingAttributes();
            drawingAttributes.Color = Windows.UI.Colors.Red;
            double penSize = 4;
            drawingAttributes.Size = new Windows.Foundation.Size(penSize, penSize);
            drawingAttributes.IgnorePressure = false;
            drawingAttributes.FitToCurve = true;

            // Show the available recognizers
            inkRecognizerContainer = new InkRecognizerContainer();
            recoView = inkRecognizerContainer.GetRecognizers();
            if (recoView.Count > 0)
            {
                foreach (InkRecognizer recognizer in recoView)
                {
                    RecoName.Items.Add(recognizer.Name);
                }
            }
            else
            {
                RecoName.IsEnabled = false;
                RecoName.Items.Add("No Recognizer Available");
            }
            RecoName.SelectedIndex = 0;

            // Set the text services so we can query when language changes
            textServiceManager = CoreTextServicesManager.GetForCurrentView();
            textServiceManager.InputLanguageChanged += TextServiceManager_InputLanguageChanged;

            SetDefaultRecognizerByCurrentInputMethodLanguageTag();

            // Initialize reco tooltip
            recoTooltip = new ToolTip();
            recoTooltip.Content = InstallRecoText;
            ToolTipService.SetToolTip(InstallReco, recoTooltip);

            // Initialize the InkCanvas
            inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(drawingAttributes);
            inkCanvas.InkPresenter.InputDeviceTypes = Windows.UI.Core.CoreInputDeviceTypes.Mouse | Windows.UI.Core.CoreInputDeviceTypes.Pen | Windows.UI.Core.CoreInputDeviceTypes.Touch;

            this.Unloaded += Scenario2_Unloaded;
            this.SizeChanged += Scenario2_SizeChanged;
        }

        private void Scenario2_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            SetCanvasSize();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            SetCanvasSize();
        }

        private void Scenario2_Unloaded(object sender, RoutedEventArgs e)
        {
            recoTooltip.IsOpen = false;
        }

        private void SetCanvasSize()
        {
            Output.Width = Window.Current.Bounds.Width;
            Output.Height = Window.Current.Bounds.Height / 2;
            inkCanvas.Width = Window.Current.Bounds.Width;
            inkCanvas.Height = Window.Current.Bounds.Height / 2;
        }

        void OnRecognizerChanged(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            string selectedValue = ((String)RecoName.SelectedValue);
            SetRecognizerByName(selectedValue);
        }

        async void OnRecognizeAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            IReadOnlyList<InkStroke> currentStrokes = inkCanvas.InkPresenter.StrokeContainer.GetStrokes();
            if (currentStrokes.Count > 0)
            {
                try
                {
                    var recognitionResults = await inkRecognizerContainer.RecognizeAsync(inkCanvas.InkPresenter.StrokeContainer, InkRecognitionTarget.All);

                    if (recognitionResults.Count > 0)
                    {
                        // Display recognition result
                        String str = "Recognition result:";
                        foreach (var r in recognitionResults)
                        {
                            str += " " + r.GetTextCandidates()[0];
                        }
                        rootPage.NotifyUser(str, SDKTemplate.NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("No text recognized", SDKTemplate.NotifyType.StatusMessage);
                    }
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser("Recognize operation failed: " + ex.Message, NotifyType.ErrorMessage);
                }
            }
        }

        void OnClear(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            inkCanvas.InkPresenter.StrokeContainer.Clear();
            rootPage.NotifyUser("Cleared Canvas", SDKTemplate.NotifyType.StatusMessage);
        }

        bool SetRecognizerByName(String recognizerName)
        {
            bool recognizerFound = false;

            foreach (InkRecognizer reco in recoView)
            {
                if (recognizerName == reco.Name)
                {
                    inkRecognizerContainer.SetDefaultRecognizer(reco);
                    recognizerFound = true;
                    break;
                }
            }

            if (!recognizerFound && rootPage != null)
            {
                rootPage.NotifyUser("Could not find target recognizer", NotifyType.ErrorMessage);
            }

            return recognizerFound;
        }

        private void TextServiceManager_InputLanguageChanged(CoreTextServicesManager sender, object args)
        {
            SetDefaultRecognizerByCurrentInputMethodLanguageTag();
        }

        private void SetDefaultRecognizerByCurrentInputMethodLanguageTag()
        {
            // Query recognizer name based on current input method language tag (bcp47 tag)
            Language currentInputLanguage = textServiceManager.InputLanguage;

            if (currentInputLanguage != previousInputLanguage)
            {
                // try query with the full BCP47 name
                string recognizerName = RecognizerHelper.LanguageTagToRecognizerName(currentInputLanguage.LanguageTag);

                if (recognizerName != string.Empty)
                {
                    for (int index = 0; index < recoView.Count; index++)
                    {
                        if (recoView[index].Name == recognizerName)
                        {
                            inkRecognizerContainer.SetDefaultRecognizer(recoView[index]);
                            RecoName.SelectedIndex = index;
                            previousInputLanguage = currentInputLanguage;
                            break;
                        }
                    }
                }
            }
        }

        private void RecoButton_Click(object sender, RoutedEventArgs e)
        {
            recoTooltip.IsOpen = !recoTooltip.IsOpen;
        }
    }
}
