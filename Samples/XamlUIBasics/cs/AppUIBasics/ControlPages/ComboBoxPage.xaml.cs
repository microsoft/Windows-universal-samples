//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using System;
using System.Collections.Generic;
using Windows.Foundation.Metadata;
using Windows.UI;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ComboBoxPage : Page
    {
        public List<Tuple<string, FontFamily>> Fonts { get; } = new List<Tuple<string, FontFamily>>()
            {
                new Tuple<string, FontFamily>("Arial", new FontFamily("Arial")),
                new Tuple<string, FontFamily>("Comic Sans MS", new FontFamily("Comic Sans MS")),
                new Tuple<string, FontFamily>("Courier New", new FontFamily("Courier New")),
                new Tuple<string, FontFamily>("Segoe UI", new FontFamily("Segoe UI")),
                new Tuple<string, FontFamily>("Times New Roman", new FontFamily("Times New Roman"))
            };

        public List<double> FontSizes { get; } = new List<double>()
            {
                8,
                9,
                10,
                11,
                12,
                14,
                16,
                18,
                20,
                24,
                28,
                36,
                48,
                72
            };

        public ComboBoxPage()
        {
            this.InitializeComponent();
        }

        private void ColorComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string colorName = e.AddedItems[0].ToString();
            Color color;
            switch (colorName)
            {
                case "Yellow":
                    color = Colors.Yellow;
                    break;
                case "Green":
                    color = Colors.Green;
                    break;
                case "Blue":
                    color = Colors.Blue;
                    break;
                case "Red":
                    color = Colors.Red;
                    break;
            }
            Control1Output.Fill = new SolidColorBrush(color);
        }

        private void Combo2_Loaded(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Combo2.SelectedIndex = 2;
        }

        private void Combo3_Loaded(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Combo3.SelectedIndex = 2;

            if ((ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7)))
            {
                Combo3.TextSubmitted += Combo3_TextSubmitted;
            }
        }

        private void Combo3_TextSubmitted(ComboBox sender, ComboBoxTextSubmittedEventArgs args)
        {
            if (double.TryParse(sender.Text, out double newValue) && newValue < 100 && newValue > 8)
            {
                // Update the SelectedItem to the new value. 
                sender.SelectedItem = newValue;
            }
            else
            {
                // If the item is invalid, reject it and revert the text. 
                sender.Text = sender.SelectedValue.ToString();

                var dialog = new ContentDialog();
                dialog.Content = "The font size must be a number between 8 and 100.";
                dialog.CloseButtonText = "Close";
                dialog.DefaultButton = ContentDialogButton.Close;
                var task = dialog.ShowAsync();
            }

            // Mark the event as handled so the framework doesn’t update the selected item automatically. 
            args.Handled = true;
        }
    }
}
