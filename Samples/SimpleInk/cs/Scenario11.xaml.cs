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
using Windows.UI.Input.Inking;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.ViewManagement;

namespace SDKTemplate
{
    /// <summary>
    /// 
    /// </summary>
    public sealed partial class Scenario11 : Page
    {
        AccessibilitySettings accessibility = new AccessibilitySettings();

        public Scenario11()
        {
            this.InitializeComponent();

            // Initialize the InkCanvas
            inkCanvas.InkPresenter.InputDeviceTypes =
                Windows.UI.Core.CoreInputDeviceTypes.Mouse |
                Windows.UI.Core.CoreInputDeviceTypes.Pen;
            
            ContrastingColorButton.IsChecked = true;
            ContrastingColorButton.Visibility = accessibility.HighContrast ? Visibility.Visible : Visibility.Collapsed;
            accessibility.HighContrastChanged += Accessibility_HighContrastChanged;
        }

        private void Accessibility_HighContrastChanged(AccessibilitySettings sender, object args)
        {
            ContrastingColorButton.Visibility = accessibility.HighContrast ? Visibility.Visible : Visibility.Collapsed;
        }

        private void ContrastingColorButtonCheckedChanged(object sender, RoutedEventArgs e)
        {
            if (ContrastingColorButton.IsChecked.Value)
            {
                inkCanvas.InkPresenter.HighContrastAdjustment = InkHighContrastAdjustment.UseSystemColorsWhenNecessary;
            }
            else
            {
                inkCanvas.InkPresenter.HighContrastAdjustment = InkHighContrastAdjustment.UseOriginalColors;
            }
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            HelperFunctions.UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
        }

        
    }
}