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
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.Devices.PointOfService;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario6_ManipulatingCursorAttributes : Page
    {
        private MainPage rootPage = MainPage.Current;
        private ClaimedLineDisplay lineDisplay;

        public Scenario6_ManipulatingCursorAttributes()
        {
            this.InitializeComponent();
            this.DataContext = this;
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await InitializeAsync();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            lineDisplay?.Dispose();
            lineDisplay = null;
        }

        private async Task InitializeAsync()
        {
            UpdateButton.IsEnabled = false;
            DisplayTextButton.IsEnabled = false;
            ResetButton.IsEnabled = false;
            CursorTypeComboBox.Items.Clear();

            lineDisplay = await rootPage.ClaimScenarioLineDisplayAsync();

            CursorTypeComboBox.IsEnabled = (lineDisplay != null) && lineDisplay.DefaultWindow.Cursor.CanCustomize;
            if (CursorTypeComboBox.IsEnabled)
            {
                if (lineDisplay.DefaultWindow.Cursor.IsBlockSupported)
                {
                    CursorTypeComboBox.Items.Add(LineDisplayCursorType.Block);
                }

                if (lineDisplay.DefaultWindow.Cursor.IsHalfBlockSupported)
                {
                    CursorTypeComboBox.Items.Add(LineDisplayCursorType.HalfBlock);
                }

                if (lineDisplay.DefaultWindow.Cursor.IsOtherSupported)
                {
                    CursorTypeComboBox.Items.Add(LineDisplayCursorType.Other);
                }

                if (lineDisplay.DefaultWindow.Cursor.IsReverseSupported)
                {
                    CursorTypeComboBox.Items.Add(LineDisplayCursorType.Reverse);
                }

                if (lineDisplay.DefaultWindow.Cursor.IsUnderlineSupported)
                {
                    CursorTypeComboBox.Items.Add(LineDisplayCursorType.Underline);
                }

                if (CursorTypeComboBox.Items.Count > 0)
                {
                    CursorTypeComboBox.SelectedIndex = 0;
                }
            }

            IsAutoAdvanceEnabledCheckbox.IsEnabled = (lineDisplay != null);

            IsBlinkEnabledCheckbox.IsEnabled = (lineDisplay != null) && lineDisplay.DefaultWindow.Cursor.IsBlinkSupported;
            SetCursorPositionCheckbox.IsEnabled = (lineDisplay != null);
            UpdateButton.IsEnabled = (lineDisplay != null);
            DisplayTextButton.IsEnabled = (lineDisplay != null);

            ResetButton.IsEnabled = true;
        }

        private async void UpdateButton_Click(object sender, RoutedEventArgs e)
        {
            LineDisplayCursorAttributes cursorAttributes = lineDisplay.DefaultWindow.Cursor.GetAttributes();
            cursorAttributes.IsAutoAdvanceEnabled = IsAutoAdvanceEnabledCheckbox.IsChecked.Value;

            if (lineDisplay.DefaultWindow.Cursor.CanCustomize && (CursorTypeComboBox.SelectedItem != null))
            {
                cursorAttributes.CursorType = (LineDisplayCursorType)CursorTypeComboBox.SelectedItem;
            }

            if (lineDisplay.DefaultWindow.Cursor.IsBlinkSupported)
            {
                cursorAttributes.IsBlinkEnabled = IsBlinkEnabledCheckbox.IsChecked.Value;
            }

            if (SetCursorPositionCheckbox.IsChecked.Value)
            {
                Point position = new Point()
                {
                    X = Helpers.ParseUIntWithFallback(CursorPositionXTextBox, -1.0),
                    Y = Helpers.ParseUIntWithFallback(CursorPositionYTextBox, -1.0),
                };

                Size windowSizeInCharacters = lineDisplay.DefaultWindow.SizeInCharacters;
                if ((position.X < 0) || (position.X >= windowSizeInCharacters.Width) ||
                    (position.Y < 0) || (position.Y >= windowSizeInCharacters.Height))
                {
                    rootPage.NotifyUser("Cursor position must be within the window.", NotifyType.ErrorMessage);
                    return;
                }

                cursorAttributes.Position = position;
            }

            if (await lineDisplay.DefaultWindow.Cursor.TryUpdateAttributesAsync(cursorAttributes))
            {
                rootPage.NotifyUser("Cursor updated.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Failed to update the cursor.", NotifyType.ErrorMessage);
            }
        }

        private async void DisplayTextButton_Click(object sender, RoutedEventArgs e)
        {
            if (await lineDisplay.DefaultWindow.TryDisplayTextAsync("Text"))
            {
                rootPage.NotifyUser("Text displayed.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Failed to display text.", NotifyType.ErrorMessage);
            }
        }

        private async void ResetButton_Click(object sender, RoutedEventArgs e)
        {
            lineDisplay?.Dispose();
            await InitializeAsync();
        }
    }
}
