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
// Scenario2.xaml.h
// Declaration of the Scenario2 class
//

#pragma once

#include "pch.h"
#include "Scenario2_MultiFile.g.h"
#include "MainPage.xaml.h"

namespace FilePicker
{
    public ref class Scenario2 sealed
    {
    public:
        Scenario2();

    private:
        SDKTemplate::MainPage^ rootPage;

        void PickFilesButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
