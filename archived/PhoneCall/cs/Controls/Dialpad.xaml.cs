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

using PhoneCall.ViewModels;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PhoneCall.Controls
{
    public sealed partial class Dialpad : UserControl
    {
        /// <summary>
        /// A user control that hosts the dial pad in the dial panel.
        /// </summary>
        public Dialpad()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Processes press and hold for the buttons that supports press and hold. E.g
        /// 1 -> Voicemail
        /// 0 -> +
        /// * -> , (pause)
        /// # -> ; (wait)
        /// </summary>
        private void OnDialPadHolding(object sender, Windows.UI.Xaml.Input.HoldingRoutedEventArgs e)
        {
            Button button = (Button)sender;
            DialerViewModel vm = (DialerViewModel)DataContext;
            if ((vm != null) && (e.HoldingState == Windows.UI.Input.HoldingState.Started))
            {
                vm.ProcessDialPadHolding.Execute(button.Tag);
            }
            
        }
    }
}
