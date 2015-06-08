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
// CompletedCallback.xaml.cpp
// Implementation of the CompletedCallback class
//

#include "pch.h"
#include "CompletedCallback.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

CompletedCallback::CompletedCallback()
{
    InitializeComponent();
}

void CompletedCallback::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage = MainPage::Current;
}

/// <summary>
/// Click handler for the 'CompletedCallbackButton' button.
/// Demonstrates the use of a message dialog with custom commands and using a completed callback instead of delegates.
/// A message will be displayed indicating which command was invoked on the dialog.
/// In this scenario, 'Install updates' is selected as the default choice.
/// </summary>
/// <param name="sender">The Object that caused this event to be fired.</param>
/// <param name="e">State information and event data associated with the routed event.</param>
void CompletedCallback::CompletedCallbackButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Create the message dialog and set its content and title
    auto messageDialog = ref new MessageDialog("New updates have been found for this program. Would you like to install the new updates?", "Updates available");

    // Add the commands to the dialog, use ids for the commands instead of delegates
    messageDialog->Commands->Append(ref new UICommand("Don't install", nullptr, PropertyValue::CreateInt32(0)));
    messageDialog->Commands->Append(ref new UICommand("Install updates", nullptr, PropertyValue::CreateInt32(1)));

    // Set the command that will be invoked by default
    messageDialog->DefaultCommandIndex = 1;

    // Show the message dialog and retrieve the id of the chosen command
    create_task(messageDialog->ShowAsync()).then([this](IUICommand^ command)
    {
        if (command->Id != nullptr)
        {
            rootPage->NotifyUser("The '" + command->Label + "' (" +  command->Id + ") command has been selected.", NotifyType::StatusMessage);
        }
        else
        {
            rootPage->NotifyUser("The dialog has now been closed", NotifyType::StatusMessage);
        }
    });
}
