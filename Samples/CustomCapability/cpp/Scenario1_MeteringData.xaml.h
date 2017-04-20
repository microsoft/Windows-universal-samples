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

#include "Scenario1_MeteringData.g.h"
#include "ServiceViewModel.h"
#include "DeviceList.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class MeteringData sealed
    {
    public:
        MeteringData();
        property ServiceViewModel^ ViewModel;

    private:
        void button_Click_StartMetering(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void button_Click_StopMetering(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void slider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
    };
}
