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
// S4_CheckIndexRevision.xaml.h
// Declaration of the S4_CheckIndexRevision class
//

#pragma once
#include "S4_CheckIndexRevision.g.h"

namespace SDKTemplate
{
    public ref class S4_CheckIndexRevision sealed
    {
    public:
        S4_CheckIndexRevision();

    private:
        void CheckIndexRevision_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
