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

void Scenario0_Choose::AddReadingType(SensorReadingType readingType)
{
    auto item = ref new ComboBoxItem();
    item->Content = readingType.ToString();
    item->Tag = readingType;
    ReadingTypeComboBox->Items->Append(item);
    if (readingType == rootPage->SensorReadingType)
    {
        ReadingTypeComboBox->SelectedItem = item;
    }
}

void Scenario0_Choose::AddOptimizationGoal(SensorOptimizationGoal optimizationGoal)
{
    auto item = ref new ComboBoxItem();
    item->Content = optimizationGoal.ToString();
    item->Tag = optimizationGoal;
    OptimizationGoalComboBox->Items->Append(item);
    if (optimizationGoal == rootPage->SensorOptimizationGoal)
    {
        OptimizationGoalComboBox->SelectedItem = item;
    }
}

Scenario0_Choose::Scenario0_Choose()
{
    InitializeComponent();

    AddReadingType(SensorReadingType::Absolute);
    AddReadingType(SensorReadingType::Relative);

    AddOptimizationGoal(SensorOptimizationGoal::Precision);
    AddOptimizationGoal(SensorOptimizationGoal::PowerEfficiency);
}

void Scenario0_Choose::OnNavigatedFrom(NavigationEventArgs^ e)
{
    auto item = safe_cast<ComboBoxItem^>(ReadingTypeComboBox->SelectedItem);
    rootPage->SensorReadingType = safe_cast<SensorReadingType>(item->Tag);

    item = safe_cast<ComboBoxItem^>(OptimizationGoalComboBox->SelectedItem);
    rootPage->SensorOptimizationGoal = safe_cast<SensorOptimizationGoal>(item->Tag);
}
