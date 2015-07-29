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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using PhoneCall.ViewModels;


// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PhoneCall.Controls
{
    public sealed partial class LinePicker : UserControl
    {
        LinePickerViewModel linePickerVM;

        /// <summary>
        /// A user control that hosts the line picker used in all the panels but contact.
        /// </summary>
        public LinePicker()
        {
            this.InitializeComponent();
            linePickerVM = ViewModelDispatcher.LinePickerViewModel;
            DataContext = linePickerVM;
        }

        /// <summary>
        /// Tap event that shows our custom bindable fly out.
        /// </summary>
        private void ShowLinePickerFlyout(object sender, TappedRoutedEventArgs e)
        {
            FrameworkElement senderElement = sender as FrameworkElement;
            FlyoutBase flyoutBase = FlyoutBase.GetAttachedFlyout(senderElement);

            flyoutBase.Placement = FlyoutPlacementMode.Bottom;
            flyoutBase.ShowAt(senderElement);
        }

        /// <summary>
        /// upddates the line in calling info when the user switches the line in the UI.
        /// </summary>
        private void onLinePickerChange(object sender, RoutedEventArgs e)
        {
            MenuFlyoutItem LinePickerItem = (MenuFlyoutItem)sender;
            Guid LineGuid = (Guid)LinePickerItem.CommandParameter;
            linePickerVM.ChangePhoneLine(LineGuid);
            LinePickerFlyout.Hide();
        }
    }
}
