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
// FindId.xaml.h
// Declaration of the FindId class
//

#pragma once
#include "Scenario1_FindId.g.h"
#include <vector>
#include <string>

namespace SDKTemplate
{
    public ref class Scenario1_FindId sealed
    {
    public:
        Scenario1_FindId();

    private:
        std::vector<std::wstring> FindIdsInString(Platform::String^ inputString);

        void Default_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
