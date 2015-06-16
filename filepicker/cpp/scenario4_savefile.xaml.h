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
// Scenario4.xaml.h
// Declaration of the Scenario4 class
//

#pragma once

#include "pch.h"
#include "Scenario4_SaveFile.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    public ref class Scenario4 sealed
    {
    public:
        Scenario4();

    private:
        MainPage^ rootPage;

        void SaveFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
