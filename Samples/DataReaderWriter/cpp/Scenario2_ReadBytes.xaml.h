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
// ReadBytes.xaml.h
// Declaration of the ReadBytes class
//

#pragma once

#include <cstring>
#include "pch.h"
#include "Scenario2_ReadBytes.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ReadBytes sealed
    {
    public:
        ReadBytes();
    
    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        MainPage^ rootPage;
        void HexDump(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ReadLoop(Windows::Storage::Streams::DataReader^ dataReader);
        void PrintRow(Platform::Array<byte>^ bytes, int i);
        Platform::String^ GetAddress(int i);
        Platform::String^ ToHex(byte value);
        int static const bytesPerRow = 16;
        int static const bytesPerSegment = 2;
        int static const chunkSize = 4096;
        int currChunk;
    };
}
