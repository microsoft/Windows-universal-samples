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
// Scenario6.xaml.h
// Declaration of the Scenario6 class
//

#pragma once

#include "pch.h"
#include "Scenario6_SaveCachedFile.g.h"
#include "MainPage.xaml.h"

namespace FilePicker
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public ref class Scenario6 sealed
    {
    public:
        Scenario6();

    private:
        SDKTemplate::MainPage^ rootPage;
        Platform::String^ fileToken;

        void SaveFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void WriteFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OutputFileAsync(Windows::Storage::StorageFile^ file);
    };
}
