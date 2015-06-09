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
#include "DefaultCloseCommand.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

DefaultCloseCommand::DefaultCloseCommand()
{
    InitializeComponent();
}

void DefaultCloseCommand::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage = MainPage::Current;
}

/// <summary>
/// Click handler for the 'DefaultCloseCommandButton' button.
/// Demonstrates showing a message dialog with a default close command and content.
/// A message will be displayed indicating that the dialog has been closed.
/// In this scenario, the only command is the default 'Close' command that is used if no other commands are specified.
/// </summary>
/// <param name="sender">The Object that caused this event to be fired.</param>
/// <param name="e">State information and event data associated with the routed event.</param>
void DefaultCloseCommand::DefaultCloseCommandButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Create the message dialog and set its content; it will get a default "Close" command since there aren't any other commands being added
    auto messageDialog = ref new MessageDialog("You've exceeded your trial period.");

    // Show the message dialog
    create_task(messageDialog->ShowAsync()).then([this](IUICommand^ command)
    {
        rootPage->NotifyUser("The dialog has now been closed", NotifyType::StatusMessage);
    });
}
