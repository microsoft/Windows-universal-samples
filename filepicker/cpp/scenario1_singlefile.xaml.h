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
// Scenario1.xaml.h
// Declaration of the Scenario1 class
//

#pragma once

#include "pch.h"
#include "Scenario1_SingleFile.g.h"
#include "MainPage.xaml.h"

namespace FilePicker
{
    public ref class Scenario1 sealed
    {
    public:
        Scenario1();

    private:
        SDKTemplate::MainPage^ rootPage;

        void PickAFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
