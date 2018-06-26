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

#include "pch.h"
#include "Scenario2_DoNotAutoInvoke.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::UI::Text::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario2_DoNotAutoInvoke::Scenario2_DoNotAutoInvoke()
{
    InitializeComponent();
}

/// <summary>
/// Update the setting to show touch keyboard on focus change.
/// If check box is checked, touch keyboard will not show on focus change.
/// If check box is not checked, touch keyboard will show on focus change. (This is the default behavior.)
/// </summary>
void Scenario2_DoNotAutoInvoke::UpdateAutoInvokeState()
{
    auto coreTextServicesManager = CoreTextServicesManager::GetForCurrentView();
    auto editContext = coreTextServicesManager->CreateEditContext();

    if (AutoInvokedCheckbox->IsChecked->Value)
    {
        // Prevent touch keyboard to show on focus change.
        editContext->InputPaneDisplayPolicy = CoreTextInputPaneDisplayPolicy::Manual;
    }
    else
    {
        // Touch keyboard will show on focus change.
        editContext->InputPaneDisplayPolicy = CoreTextInputPaneDisplayPolicy::Automatic;
    }
}

/// <summary>
/// Invoked when the checkbox gets clicked.
/// </summary>
void Scenario2_DoNotAutoInvoke::OnAutoInvokedClicked(Object^ /*sender*/, RoutedEventArgs^ /*e*/)
{
    UpdateAutoInvokeState();
}
