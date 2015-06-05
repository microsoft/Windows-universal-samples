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
// Scenario5.xaml.h
// Declaration of the Scenario5 class
//

#pragma once

#include "pch.h"
#include "Scenario5_OpenCachedFile.g.h"
#include "MainPage.xaml.h"

namespace FilePicker
{
    public ref class Scenario5 sealed
    {
    public:
        Scenario5();

    private:
        SDKTemplate::MainPage^ rootPage;
        Platform::String^ fileToken;

        void PickFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OutputFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OutputFileAsync(Windows::Storage::StorageFile^ file);
    };
}
