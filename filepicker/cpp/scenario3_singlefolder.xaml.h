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
// Scenario3.xaml.h
// Declaration of the Scenario3 class
//

#pragma once

#include "pch.h"
#include "Scenario3_SingleFolder.g.h"
#include "MainPage.xaml.h"

namespace FilePicker
{
    public ref class Scenario3 sealed
    {
    public:
        Scenario3();

    private:
        SDKTemplate::MainPage^ rootPage;

        void PickFolderButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
