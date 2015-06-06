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

using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System;

namespace SDKTemplate
{
    public sealed partial class CancelCommand : Page
    {
        MainPage rootPage = MainPage.Current;

        public CancelCommand()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Click handler for the 'CancelCommandButton' button.
        /// Demonstrates setting the command to be invoked when the 'escape' key is pressed.
        /// Also demonstrates retrieval of the label of the chosen command and setting a callback to a function.
        /// A message will be displayed indicating which command was invoked.
        /// In this scenario, 'Try again' is selected as the default choice, and the 'escape' key will invoke the command named 'Close'
        /// </summary>
        /// <param name="sender">The Object that caused this event to be fired.</param>
        /// <param name="e">State information and event data associated with the routed event.</param>
        private async void CancelCommandButton_Click(object sender, RoutedEventArgs e)
        {
            // Create the message dialog and set its content
            var messageDialog = new MessageDialog("No internet connection has been found.");

            // Add commands and set their callbacks; both buttons use the same callback function instead of inline event handlers
            messageDialog.Commands.Add(new UICommand("Try again", new UICommandInvokedHandler(this.CommandInvokedHandler)));
            messageDialog.Commands.Add(new UICommand("Close", new UICommandInvokedHandler(this.CommandInvokedHandler)));

            // Set the command that will be invoked by default
            messageDialog.DefaultCommandIndex = 0;

            // Set the command to be invoked when escape is pressed
            messageDialog.CancelCommandIndex = 1;

            // Show the message dialog
            await messageDialog.ShowAsync();
        }

        #region Commands
        /// <summary>
        /// Callback function for the invocation of the dialog commands.
        /// </summary>
        /// <param name="command">The command that was invoked.</param>
        private void CommandInvokedHandler(IUICommand command)
        {
            // Display message showing the label of the command that was invoked
            rootPage.NotifyUser("The '" + command.Label + "' command has been selected.", NotifyType.StatusMessage);
        }
        #endregion
    }
}
