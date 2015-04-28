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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ToggleButtonPage : Page
    {
        public ToggleButtonPage()
        {
            this.InitializeComponent();

            // Set initial outpput value.
            Control1Output.Text = (bool)Toggle1.IsChecked ? "On" : "Off";
        }

        private void ToggleButton_Checked(object sender, RoutedEventArgs e)
        {
            Control1Output.Text = "On";
        }

        private void ToggleButton_Unchecked(object sender, RoutedEventArgs e)
        {
            Control1Output.Text = "Off";
        }
    }
}
