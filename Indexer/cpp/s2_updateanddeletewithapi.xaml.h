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
// S2_UpdateAndDeleteWithAPI.xaml.h
// Declaration of the S2_UpdateAndDeleteWithAPI class
//

#pragma once
#include "S2_UpdateAndDeleteWithAPI.g.h"

namespace SDKTemplate
{
    public ref class S2_UpdateAndDeleteWithAPI sealed
    {
    public:
        S2_UpdateAndDeleteWithAPI();

    private:
        void AddToIndex_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UpdatePropertyHelper(Platform::String^ itemKey, Platform::String^ propertyKey, Platform::String^ propertyName, Platform::Object^ newValue);
        void UpdateName_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DeleteKeywords_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DeleteItemsHelper(std::function<Windows::Foundation::IAsyncAction^(Windows::Storage::Search::ContentIndexer^)> deleteFunction);
        void DeleteSingleItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DeleteMultipleItems_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DeleteAllItems_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
