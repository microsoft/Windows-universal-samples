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
// S7_RetrieveWithAppContent.xaml.h
// Declaration of the S7_RetrieveWithAppContent class
//

#pragma once
#include "S7_RetrieveWithAppContent.g.h"

namespace SDKTemplate
{
    public ref class S7_RetrieveWithAppContent sealed
    {
    public:
        S7_RetrieveWithAppContent();

    private:
        void AddToIndex_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void RetrieveAllItems_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void RetrieveMatchedItems_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
