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

#include "pch.h"
#include "Scenario1_MeteringData.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls::Primitives;

MeteringData::MeteringData()
{
    InitializeComponent();
    ViewModel = ref new ServiceViewModel();
}

void SDKTemplate::MeteringData::button_Click_StartMetering(Object^ sender, RoutedEventArgs^ e)
{
    ViewModel->StartMetering((int)SamplePeriodSlider->Value);
}

void SDKTemplate::MeteringData::button_Click_StopMetering(Object^ sender, RoutedEventArgs^ e)
{
    ViewModel->StopMetering();
}

void SDKTemplate::MeteringData::slider_ValueChanged(Object^ sender, RangeBaseValueChangedEventArgs^ e)
{
    // Ignore slider value changes prior to ViewModel initialization.
    if (ViewModel != nullptr)
    {
        // Update the sample period in response to slider change.
        ViewModel->SetSamplePeriod((int)SamplePeriodSlider->Value);
    }
}
