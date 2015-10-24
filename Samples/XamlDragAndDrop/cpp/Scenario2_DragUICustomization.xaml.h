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

#pragma once

#include "Scenario2_DragUICustomization.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_DragUICustomization sealed
    {
    public:
        Scenario2_DragUICustomization();
    private:
        MainPage^ rootPage;
        Platform::String^ _insideName;
        Platform::String^ _outsideName;
        Platform::String^ _dropHere;
        bool IsChecked(Windows::UI::Xaml::Controls::Primitives::ToggleButton^ button);

        void SourceGrid_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args);
        void TargetTextBox_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
        void TargetTextBox_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
        void TargetTextBox_DragLeave(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);

    };
}
