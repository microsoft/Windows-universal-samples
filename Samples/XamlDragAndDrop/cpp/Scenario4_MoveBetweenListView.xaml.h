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

#include "Scenario4_MoveBetweenListView.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4_MoveBetweenListView sealed
    {
    public:
        Scenario4_MoveBetweenListView();
    private:
        MainPage^ rootPage;

        Platform::Collections::Vector<Platform::String^>^ GetSampleData();
        Platform::Collections::Vector<Platform::String^>^ _source;
        Platform::Collections::Vector<Platform::String^>^ _target;

        void ListView_DragItemsStarting(Platform::Object^ sender, Windows::UI::Xaml::Controls::DragItemsStartingEventArgs^ e);
        void ListView_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
        void ListView_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
    };
}
