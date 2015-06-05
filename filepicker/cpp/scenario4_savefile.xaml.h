//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
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

namespace FilePicker
{
    public ref class Scenario4 sealed
    {
    public:
        Scenario4();

    private:
        SDKTemplate::MainPage^ rootPage;

        void SaveFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
