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

using Windows.Foundation;
using Windows.Graphics.Display;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Orientation : Page
    {
        DisplayInformation displayInfo;

        public Scenario1_Orientation()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            displayInfo = DisplayInformation.GetForCurrentView();
            displayInfo.OrientationChanged += OnOrientationChanged;
            UpdateContent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            displayInfo.OrientationChanged -= OnOrientationChanged;
        }

        private void OnOrientationChanged(DisplayInformation sender, object e)
        {
            UpdateContent();
        }

        private void UpdateContent()
        {
            NativeOrientationTextBlock.Text = displayInfo.NativeOrientation.ToString();
            CurrentOrientationTextBlock.Text = displayInfo.CurrentOrientation.ToString();
            AutoRotationPreferencesTextBlock.Text = DisplayInformation.AutoRotationPreferences.ToString();
        }

        private void ApplyRotationLock()
        {
            DisplayOrientations orientations = DisplayOrientations.None;
            if (PreferLandscape.IsChecked.Value)
            {
                orientations |= DisplayOrientations.Landscape;
            }
            if (PreferPortrait.IsChecked.Value)
            {
                orientations |= DisplayOrientations.Portrait;
            }
            if (PreferLandscapeFlipped.IsChecked.Value)
            {
                orientations |= DisplayOrientations.LandscapeFlipped;
            }
            if (PreferPortraitFlipped.IsChecked.Value)
            {
                orientations |= DisplayOrientations.PortraitFlipped;
            }
            DisplayInformation.AutoRotationPreferences = orientations;
            UpdateContent();
        }
    }
}
