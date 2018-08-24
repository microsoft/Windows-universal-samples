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

#pragma once
#include "Scenario5_DeviceReadWrite.g.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class DeviceReadWrite sealed
    {
    public:
        DeviceReadWrite();

    private:
        MainPage^ rootPage = MainPage::Current;

        unsigned int writeCounter = 0;
        unsigned int readCounter = 0;

        void ReadBlock_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void WriteBlock_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void LogMessage(Platform::String^ message);
    };
}

