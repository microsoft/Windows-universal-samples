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
// Scenario0_Choose.xaml.cpp
// Implementation of the Scenario0_Choose class
//

#include "pch.h"
#include "Scenario0_Choose.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

void Scenario0_Choose::AddReadingType(AccelerometerReadingType readingType)
{
    auto item = ref new ComboBoxItem();
    item->Content = readingType.ToString();
    item->Tag = readingType;
    ReadingTypeComboBox->Items->Append(item);
    if (readingType == rootPage->AccelerometerReadingType)
    {
        ReadingTypeComboBox->SelectedItem = item;
    }
}

Scenario0_Choose::Scenario0_Choose()
{
    InitializeComponent();

    AddReadingType(AccelerometerReadingType::Standard);
    AddReadingType(AccelerometerReadingType::Linear);
    AddReadingType(AccelerometerReadingType::Gravity);
}

void Scenario0_Choose::OnNavigatedFrom(NavigationEventArgs^ e)
{
    auto item = safe_cast<ComboBoxItem^>(ReadingTypeComboBox->SelectedItem);
    rootPage->AccelerometerReadingType = safe_cast<AccelerometerReadingType>(item->Tag);
}
