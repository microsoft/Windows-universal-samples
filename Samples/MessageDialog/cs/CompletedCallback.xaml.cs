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
    public sealed partial class CompletedCallback : Page
    {
        MainPage rootPage = MainPage.Current;

        public CompletedCallback()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Click handler for the 'CompletedCallbackButton' button.
        /// Demonstrates the use of a message dialog with custom commands and using a completed callback instead of delegates.
        /// A message will be displayed indicating which command was invoked on the dialog.
        /// In this scenario, 'Install updates' is selected as the default choice.
        /// </summary>
        /// <param name="sender">The Object that caused this event to be fired.</param>
        /// <param name="e">State information and event data associated with the routed event.</param>
        private async void CompletedCallbackButton_Click(object sender, RoutedEventArgs e)
        {
            // Create the message dialog and set its content and title
            var messageDialog = new MessageDialog("New updates have been found for this program. Would you like to install the new updates?", "Updates available");

            // Add commands and set their command ids
            messageDialog.Commands.Add(new UICommand("Don't install", null, 0));
            messageDialog.Commands.Add(new UICommand("Install updates", null, 1));

            // Set the command that will be invoked by default
            messageDialog.DefaultCommandIndex = 1;

            // Show the message dialog and get the event that was invoked via the async operator
            var commandChosen = await messageDialog.ShowAsync();

            // Display message showing the label and id of the command that was invoked
            rootPage.NotifyUser("The '" + commandChosen.Label + "' (" + commandChosen.Id + ") command has been selected.", NotifyType.StatusMessage);
        }
    }
}
