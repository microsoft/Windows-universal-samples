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

using System.ComponentModel;
using Windows.UI;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Colors : Page, INotifyPropertyChanged
    {
        public Scenario1_Colors()
        {
            this.InitializeComponent();
            titleBar = ApplicationView.GetForCurrentView().TitleBar;

            // Infer the radio button selection from the title bar colors.
            if (titleBar.ButtonBackgroundColor == null)
            {
                UseStandardColors.IsChecked = true;
            }
            else if (titleBar.ButtonBackgroundColor.Value.A > 0)
            {
                UseCustomColors.IsChecked = true;
            }
            else
            {
                TransparentWhenExtended.IsChecked = true;
            }
        }

        #region Data binding
        public event PropertyChangedEventHandler PropertyChanged;

        private ApplicationViewTitleBar titleBar;

        public ApplicationViewTitleBar TitleBar
        {
            get { return titleBar; }
        }
        #endregion

        private void UseCustomColors_Click(object sender, RoutedEventArgs e)
        {
            UpdateColors();
        }

        private void UpdateColors()
        {
            if (UseStandardColors.IsChecked.Value)
            {
                // Setting colors to null returns them to system defaults.
                titleBar.BackgroundColor = null;
                titleBar.ForegroundColor = null;
                titleBar.InactiveBackgroundColor = null;
                titleBar.InactiveForegroundColor = null;

                titleBar.ButtonBackgroundColor = null;
                titleBar.ButtonHoverBackgroundColor = null;
                titleBar.ButtonPressedBackgroundColor = null;
                titleBar.ButtonInactiveBackgroundColor = null;

                titleBar.ButtonForegroundColor = null;
                titleBar.ButtonHoverForegroundColor = null;
                titleBar.ButtonPressedForegroundColor = null;
                titleBar.ButtonInactiveForegroundColor = null;
            }
            else
            {
                // Title bar colors. Alpha must be 255.
                titleBar.BackgroundColor = new Color() { A = 255, R = 54, G = 60, B = 116 };
                titleBar.ForegroundColor = new Color() { A = 255, R = 232, G = 211, B = 162 };
                titleBar.InactiveBackgroundColor = new Color() { A = 255, R = 135, G = 141, B = 199 };
                titleBar.InactiveForegroundColor = new Color() { A = 255, R = 232, G = 211, B = 162 };

                // Title bar button background colors. Alpha is respected when the view is extended
                // into the title bar (see scenario 2). Otherwise, Alpha is ignored and treated as if it were 255.
                byte buttonAlpha = (byte)(TransparentWhenExtended.IsChecked.Value ? 0 : 255);
                titleBar.ButtonBackgroundColor = new Color() { A = buttonAlpha, R = 54, G = 60, B = 116 };
                titleBar.ButtonHoverBackgroundColor = new Color() { A = buttonAlpha, R = 19, G = 21, B = 40 };
                titleBar.ButtonPressedBackgroundColor = new Color() { A = buttonAlpha, R = 232, G = 211, B = 162 };
                titleBar.ButtonInactiveBackgroundColor = new Color() { A = buttonAlpha, R = 135, G = 141, B = 199 };

                // Title bar button foreground colors. Alpha must be 255.
                titleBar.ButtonForegroundColor = new Color() { A = 255, R = 232, G = 211, B = 162 };
                titleBar.ButtonHoverForegroundColor = new Color() { A = 255, R = 255, G = 255, B = 255 };
                titleBar.ButtonPressedForegroundColor = new Color() { A = 255, R = 54, G = 60, B = 116 };
                titleBar.ButtonInactiveForegroundColor = new Color() { A = 255, R = 232, G = 211, B = 162 };
            }

            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("TitleBar"));
            }
        }
    }
}
