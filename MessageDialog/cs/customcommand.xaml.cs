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
    public sealed partial class CustomCommand : Page
    {
        MainPage rootPage = MainPage.Current;

        public CustomCommand()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Click handler for the 'CustomCommandButton' button.
        /// Demonstrates the use of a message dialog with custom commands, command callbacks, and a default command.
        /// A message will be displayed indicating which command was invoked on the dialog.
        /// In this scenario, 'Install updates' is selected as the default choice.
        /// </summary>
        /// <param name="sender">The Object that caused this event to be fired.</param>
        /// <param name="e">State information and event data associated with the routed event.</param>
        private async void CustomCommandButton_Click(object sender, RoutedEventArgs e)
        {
            // Create the message dialog and set its content and title
            var messageDialog = new MessageDialog("New updates have been found for this program. Would you like to install the new updates?", "Updates available");

            // Add commands and set their callbacks
            messageDialog.Commands.Add(new UICommand("Don't install", (command) =>
            {
                rootPage.NotifyUser("The 'Don't install' command has been selected.", NotifyType.StatusMessage);
            }));

            messageDialog.Commands.Add(new UICommand("Install updates", (command) =>
            {
                rootPage.NotifyUser("The 'Install updates' command has been selected.", NotifyType.StatusMessage);
            }));

            // Set the command that will be invoked by default
            messageDialog.DefaultCommandIndex = 1;

            // Show the message dialog
            await messageDialog.ShowAsync();
        }
    }
}
