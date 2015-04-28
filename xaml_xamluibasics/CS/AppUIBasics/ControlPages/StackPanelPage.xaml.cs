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

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234237

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public sealed partial class StackPanelPage : Page
    {
        public StackPanelPage()
        {
            this.InitializeComponent();
        }

        private void RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            RadioButton rb = sender as RadioButton;
            if (rb != null && Control1 != null)
            {
                string orientationName = rb.Tag.ToString();
                OrientationLabel.Text = orientationName;
                switch (orientationName)
                {
                    case "Horizontal":
                        Control1.Orientation = Orientation.Horizontal;
                        break;
                    case "Vertical":
                        Control1.Orientation = Orientation.Vertical;
                        break;
                }
            }
        }
    }
}
