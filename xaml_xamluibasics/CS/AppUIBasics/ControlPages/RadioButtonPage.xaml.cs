//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class RadioButtonPage : Page
    {
        public RadioButtonPage()
        {
            this.InitializeComponent();
        }

        private void BGRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            RadioButton rb = sender as RadioButton;

            if (rb != null && Control2Output != null)
            {
                string colorName = rb.Tag.ToString();
                switch (colorName)
                {
                    case "Yellow":
                        Control2Output.Background = new SolidColorBrush(Colors.Yellow);
                        break;
                    case "Green":
                        Control2Output.Background = new SolidColorBrush(Colors.Green);
                        break;
                    case "Blue":
                        Control2Output.Background = new SolidColorBrush(Colors.Blue);
                        break;
                    case "White":
                        Control2Output.Background = new SolidColorBrush(Colors.White);
                        break;
                }
            }
        }

        private void BorderRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            RadioButton rb = sender as RadioButton;

            if (rb != null && Control2Output != null)
            {
                string colorName = rb.Tag.ToString();
                switch (colorName)
                {
                    case "Yellow":
                        Control2Output.BorderBrush = new SolidColorBrush(Colors.Gold);
                        break;
                    case "Green":
                        Control2Output.BorderBrush = new SolidColorBrush(Colors.DarkGreen);
                        break;
                    case "Blue":
                        Control2Output.BorderBrush = new SolidColorBrush(Colors.DarkBlue);
                        break;
                    case "White":
                        Control2Output.BorderBrush = new SolidColorBrush(Colors.White);
                        break;
                }
            }
        }

        private void Option1RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            Control1Output.Text = "You selected option 1.";
        }

        private void Option2RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            Control1Output.Text = "You selected option 2.";
        }

        private void Option3RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            Control1Output.Text = "You selected option 3.";
        }
    }
}
