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
    public sealed partial class VariableSizedWrapGridPage : Page
    {
        public VariableSizedWrapGridPage()
        {
            this.InitializeComponent();
        }

        private void RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            RadioButton rb = sender as RadioButton;

            if (rb != null && Control1 != null)
            {
                string orientionName = rb.Tag.ToString();
                OrientationLabel.Text = orientionName;
                switch (orientionName)
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
