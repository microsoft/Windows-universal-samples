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
// Scenario3.xaml.h
// Declaration of the Scenario3 class
//

#pragma once

#include "pch.h"
#include "Scenario3.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3 sealed
    {
    public:
        Scenario3();

    protected:
        void GetFilesButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        Platform::String^ GetPropertyDisplayValue(Platform::Object^ property);
        Windows::UI::Xaml::Controls::TextBlock^ CreateHeaderTextBlock(Platform::String^ contents);
        Windows::UI::Xaml::Controls::TextBlock^ CreateLineItemTextBlock();

    private:
        static Platform::String^ CopyrightProperty;
        static Platform::String^ ColorSpaceProperty;
    };
}
