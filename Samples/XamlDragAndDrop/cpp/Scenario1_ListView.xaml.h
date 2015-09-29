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

#include "Scenario1_ListView.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_ListView sealed
    {
    public:
        Scenario1_ListView();
    private:
        MainPage^ rootPage;

        Platform::Collections::Vector<Platform::String^>^ GetSampleData();
        Platform::Collections::Vector<Platform::String^>^ _reference;
        Platform::Collections::Vector<Platform::String^>^ _selection;
        Platform::String^ _deletedItem;

        void SourceListView_DragItemsStarting(Platform::Object^ sender, Windows::UI::Xaml::Controls::DragItemsStartingEventArgs^ e);
        void TargetListView_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
        void TargetListView_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
        void TargetListView_DragItemsStarting(Platform::Object^ sender, Windows::UI::Xaml::Controls::DragItemsStartingEventArgs^ e);
        void TargetListView_DragItemsCompleted(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::DragItemsCompletedEventArgs^ args);
        void TargetTextBlock_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
        void TargetTextBlock_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
    };
}
