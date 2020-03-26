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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Controls;
using PhoneCall.ViewModels;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PhoneCall.Controls
{
    public sealed partial class DialerPanel : UserControl
    {
        /// <summary>
        /// A user control that hosts the dial panel within the main pivot/tab.
        /// </summary>
        public DialerPanel()
        {
            this.InitializeComponent();
            DataContext = ViewModelDispatcher.DialerViewModel;
            setDialPadHeight();
        }

        /// <summary>
        /// sets the dialpad control height based on screen resolution.
        /// </summary>
        private void setDialPadHeight()
        {
            var bounds = Window.Current.Bounds;
            int windowHeight = (int)bounds.Height;
            Dialpad.Height = windowHeight - 350;
        }

        /// <summary>
        /// Auto scrolls the dialer number heap to the end for long phone numbers.
        /// </summary>
        private void OnDialerNumberControlSizeChanged(object sender, SizeChangedEventArgs e)
        {
            double scrollToEndOffset = dialerNumberControlScrollviewer.ScrollableWidth;
            dialerNumberControlScrollviewer.ChangeView(scrollToEndOffset,null,null);
        }

        /// <summary>
        /// Processes press and hold for the back button
        /// This clears the dialer number heap all at once.
        /// </summary>
        private void OnBackspaceHolding(object sender, HoldingRoutedEventArgs e)
        {
            DialerViewModel vm = (DialerViewModel)DataContext;
            if ((vm != null) && (e.HoldingState == Windows.UI.Input.HoldingState.Started))
            {
                vm.ProcessBackSpaceHolding.Execute(null);
            }
        }
    }
}
