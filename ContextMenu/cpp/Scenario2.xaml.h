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
// Scenario2.xaml.h
// Declaration of the Scenario2 class
//

#pragma once

#include "pch.h"
#include "Scenario2.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public ref class Scenario2 sealed
    {
    public:
        Scenario2();

    private:
        SDKTemplate::MainPage^ rootPage;

        Windows::Foundation::Rect Scenario2::GetTextboxSelectionRect(Windows::UI::Xaml::Controls::TextBox^ textbox);
        void ReadOnlyTextBox_ContextMenuOpening(Platform::Object^ sender, Windows::UI::Xaml::Controls::ContextMenuEventArgs^ e);
    };
}
