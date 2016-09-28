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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace SDKTemplate
{
    public sealed partial class Scenario3_Legacy : Page
    {
        public Scenario3_Legacy()
        {
            this.InitializeComponent();
        }

        // This method demonstrates how an app can programmatically remove focus engagement.
        private void LegacyControl_KeyUp(object sender, KeyRoutedEventArgs e)
        {
            // If the user presses and releases the "B" button when focus is on the legacy
            // control, then remove focus engagement.
            if (e.OriginalKey == Windows.System.VirtualKey.GamepadB)
            {
                e.Handled = true;
                ((Control)sender).RemoveFocusEngagement();
            }
        }
    }
}
