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

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_DoNotAutoInvoke::Scenario2_DoNotAutoInvoke()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when the button is clicked, and used to direct programmatic focus to the field. We had to work around a
/// problem with focus in order to make this function work. The behavior of the On Screen Keyboard is to appear on
/// programmatic focus depending on the value of the PreventKeyboardDisplayOnProgrammaticFocus property. However, after
/// a manual dismiss, this behavior is overridden, and we will not display unless we perform an explicit invocation of
/// the keyboard, for instance, by tapping on a field.
/// 
/// The only type of dismiss that will allow us to trigger this property is a light dismiss, but we have a problem at
/// the container level, where tapping outside of the field will redirect the focus to the scenario chooser, which is
/// part of a group of controls that do not dismiss the keyboard when focus goes towards it. Therefore, we have had to
/// modify the ScrollViewer container to set its property IsTabStop to true. This makes the container focusable and works
/// around the problem with the focus in the Scenarios.
/// 
/// </summary>
/// <param name="sender">The object that raised the event.</param>
/// <param name="e">Event data that describes the click action on the button.</param>
void Scenario2_DoNotAutoInvoke::OnFocusClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    textBox->Focus(Windows::UI::Xaml::FocusState::Programmatic);
}

/// <summary>
/// Invoked when the checkbox gets checked, and used to disable the IHM invocation when a field gets programmatic focus.
/// </summary>
/// <param name="sender">The object that raised the event.</param>
/// <param name="e">Event data that describes the toggle state of the checkbox.</param>
void Scenario2_DoNotAutoInvoke::OnAutoInvokedChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    textBox->PreventKeyboardDisplayOnProgrammaticFocus = true;
}

/// <summary>
/// Invoked when the checkbox gets unchecked, and used to enable the IHM invocation when a field gets programmatic focus.
/// </summary>
/// <param name="sender">The object that raised the event.</param>
/// <param name="e">Event data that describes the toggle state of the checkbox.</param>
void Scenario2_DoNotAutoInvoke::OnAutoInvokedUnchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    textBox->PreventKeyboardDisplayOnProgrammaticFocus = false;
}
