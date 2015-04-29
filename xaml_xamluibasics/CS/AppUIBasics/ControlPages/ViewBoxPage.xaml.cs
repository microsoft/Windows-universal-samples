//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ViewBoxPage : Page
    {
        public ViewBoxPage()
        {
            this.InitializeComponent();
        }

        private void StretchDirectionButton_Checked(object sender, RoutedEventArgs e)
        {
            RadioButton rb = sender as RadioButton;

            if (rb != null && Control1 != null)
            {
                string direction = rb.Tag.ToString();
                switch (direction)
                {
                    case "UpOnly":
                        Control1.StretchDirection = StretchDirection.UpOnly;
                        break;
                    case "DownOnly":
                        Control1.StretchDirection = StretchDirection.DownOnly;
                        break;
                    case "Both":
                        Control1.StretchDirection = StretchDirection.Both;
                        break;
                }
            }
        }

        private void StretchButton_Checked(object sender, RoutedEventArgs e)
        {
            RadioButton rb = sender as RadioButton;

            if (rb != null && Control1 != null)
            {
                string stretch = rb.Tag.ToString();
                switch (stretch)
                {
                    case "None":
                        Control1.Stretch = Stretch.None;
                        break;
                    case "Fill":
                        Control1.Stretch = Stretch.Fill;
                        break;
                    case "Uniform":
                        Control1.Stretch = Stretch.Uniform;
                        break;
                    case "UniformToFill":
                        Control1.Stretch = Stretch.UniformToFill;
                        break;
                }
            }
        }
    }
}
