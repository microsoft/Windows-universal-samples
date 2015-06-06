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
// S1_AddWithAPI.xaml.h
// Declaration of the S1_AddWithAPI class
//

#pragma once
#include "S1_AddWithAPI.g.h"

namespace SDKTemplate
{
    public ref class S1_AddWithAPI sealed
    {
    public:
        S1_AddWithAPI();

    private:
        void AddToIndex_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
