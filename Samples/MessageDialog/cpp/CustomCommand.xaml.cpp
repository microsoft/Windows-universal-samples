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
// CustomCommand.xaml.cpp
// Implementation of the CustomCommand class
//

#include "pch.h"
#include "CustomCommand.xaml.h"

using namespace SDKTemplate;

using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

CustomCommand::CustomCommand()
{
    InitializeComponent();
}

void CustomCommand::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage = MainPage::Current;
}

/// <summary>
/// Click handler for the 'CustomCommandButton' button.
/// Demonstrates the use of a message dialog with custom commands, command callbacks, and a default command.
/// A message will be displayed indicating which command was invoked on the dialog.
/// In this scenario, 'Install updates' is selected as the default choice.
/// </summary>
/// <param name="sender">The Object that caused this event to be fired.</param>
/// <param name="e">State information and event data associated with the routed event.</param>
void CustomCommand::CustomCommandButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Create the message dialog and set its content and title
    auto messageDialog = ref new MessageDialog("New updates have been found for this program. Would you like to install the new updates?", "Updates available");

    // Add commands and set their callbacks
    messageDialog->Commands->Append(ref new UICommand("Don't install", ref new UICommandInvokedHandler([this](IUICommand^ command)
    {
        rootPage->NotifyUser("The 'Don't install' command has been selected.", NotifyType::StatusMessage);
    })));
    messageDialog->Commands->Append(ref new UICommand("Install updates", ref new UICommandInvokedHandler([this](IUICommand^ command)
    {
        rootPage->NotifyUser("The 'Install updates' command has been selected.", NotifyType::StatusMessage);
    })));

    // Set the command that will be invoked by default
    messageDialog->DefaultCommandIndex = 1;

    // Show the message dialog
    messageDialog->ShowAsync();
}
