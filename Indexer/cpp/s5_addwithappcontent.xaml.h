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
// S5_AddWithAppContent.xaml.h
// Declaration of the S5_AddWithAppContent class
//

#pragma once
#include "S5_AddWithAppContent.g.h"

namespace SDKTemplate
{
    public ref class S5_AddWithAppContent sealed
    {
    public:
        S5_AddWithAppContent();

    private:
        void AddToIndex_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
