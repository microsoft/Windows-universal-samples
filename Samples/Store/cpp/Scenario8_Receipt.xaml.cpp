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

//
// Scenario8.xaml.cpp
// Implementation of the Scenario8 class
//

#include "pch.h"
#include "Scenario8_Receipt.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::UI::Xaml::Navigation;

Scenario8_Receipt::Scenario8_Receipt()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario8_Receipt::OnNavigatedTo(NavigationEventArgs^ e)
{
    ConfigureSimulatorAsync("receipt.xml");
}

void Scenario8_Receipt::ShowReceipt()
{
    create_task(CurrentAppSimulator::GetAppReceiptAsync()).then([this](task<String^> currentTask)
    {
        try
        {
            String^ receipt = currentTask.get();
            Output->Text = receipt;
        }
        catch (Exception^ exception)
        {
            rootPage->NotifyUser("Get Receipt failed.", NotifyType::ErrorMessage);
        }
    });
}
