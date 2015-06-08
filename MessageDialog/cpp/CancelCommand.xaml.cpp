//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// CancelCommand.xaml.cpp
// Implementation of the CancelCommand class
//

#include "pch.h"
#include "CancelCommand.xaml.h"

using namespace SDKTemplate;

using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

CancelCommand::CancelCommand()
{
    InitializeComponent();
}

void CancelCommand::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage = MainPage::Current;
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
void CancelCommand::CancelCommandButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Create the message dialog and set its content
    MessageDialog^ msg = ref new MessageDialog("No internet connection has been found.");

    // Add commands and set their callbacks; both commands use the same callback function instead of inline event handlers
    UICommand^ continueCommand = ref new UICommand("Try again", ref new UICommandInvokedHandler(this, &CancelCommand::CommandInvokedHandler));
    UICommand^ upgradeCommand = ref new UICommand("Close", ref new UICommandInvokedHandler(this, &CancelCommand::CommandInvokedHandler));

    // Add the commands to the dialog
    msg->Commands->Append(continueCommand);
    msg->Commands->Append(upgradeCommand);

    // Set the command that will be invoked by default
    msg->DefaultCommandIndex = 0;

    // Set the command to be invoked when escape is pressed
    msg->CancelCommandIndex = 1;

    // Show the message dialog
    msg->ShowAsync();
}

/// <summary>
/// Callback function for the invocation of the dialog commands.
/// </summary>
/// <param name="command">The command that was invoked.</param>
void CancelCommand::CommandInvokedHandler(Windows::UI::Popups::IUICommand^ command)
{
    // Display message
    rootPage->NotifyUser("The '" + command->Label + "' command has been selected.", NotifyType::StatusMessage);
}
