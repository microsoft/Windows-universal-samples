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
using Windows.Globalization;
using Windows.UI.Input.Inking;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Text.Core;
using SDKTemplate;

namespace SDKTemplate
{
    /// <summary>
    /// This page shows the code to do ink recognition
    /// </summary>
    public sealed partial class Scenario4 : Page
    {
        private MainPage rootPage = MainPage.Current;
        InkRecognizerContainer inkRecognizerContainer = null;
        private IReadOnlyList<InkRecognizer> recoView = null;
        private Language previousInputLanguage = null;
        private CoreTextServicesManager textServiceManager = null;

        public Scenario4()
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

            // Initialize the InkCanvas
            inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(drawingAttributes);
            inkCanvas.InkPresenter.InputDeviceTypes = Windows.UI.Core.CoreInputDeviceTypes.Mouse | Windows.UI.Core.CoreInputDeviceTypes.Pen | Windows.UI.Core.CoreInputDeviceTypes.Touch;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            InstallReco.IsOpen = false;
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            HelperFunctions.UpdateCanvasSize(RootGrid, Output, inkCanvas);
        }

        void OnRecognizerChanged(object sender, RoutedEventArgs e)
        {
            string selectedValue = (string)RecoName.SelectedValue;
            SetRecognizerByName(selectedValue);
        }

        async void OnRecognizeAsync(object sender, RoutedEventArgs e)
        {
            IReadOnlyList<InkStroke> currentStrokes = inkCanvas.InkPresenter.StrokeContainer.GetStrokes();
            if (currentStrokes.Count > 0)
            {
                RecognizeBtn.IsEnabled = false;
                ClearBtn.IsEnabled = false;
                RecoName.IsEnabled = false;

                var recognitionResults = await inkRecognizerContainer.RecognizeAsync(inkCanvas.InkPresenter.StrokeContainer, InkRecognitionTarget.All);

                if (recognitionResults.Count > 0)
                {
                    // Display recognition result
                    string str = "Recognition result:";
                    foreach (var r in recognitionResults)
                    {
                        str += " " + r.GetTextCandidates()[0];
                    }
                    rootPage.NotifyUser(str, NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("No text recognized.", NotifyType.StatusMessage);
                }

                RecognizeBtn.IsEnabled = true;
                ClearBtn.IsEnabled = true;
                RecoName.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser("Must first write something.", NotifyType.ErrorMessage);
            }
        }

        void OnClear(object sender, RoutedEventArgs e)
        {
            inkCanvas.InkPresenter.StrokeContainer.Clear();
            rootPage.NotifyUser("Cleared Canvas.", NotifyType.StatusMessage);
        }

        bool SetRecognizerByName(string recognizerName)
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
                rootPage.NotifyUser("Could not find target recognizer.", NotifyType.ErrorMessage);
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
            InstallReco.IsOpen = !InstallReco.IsOpen;
        }
    }
}