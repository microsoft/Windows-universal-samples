// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario3_CurrentStepCount.g.h"

namespace PedometerCPP
{
    /// <summary>
    /// 'Current step count' illustration page
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_CurrentStepCount sealed
    {
    public:
        Scenario3_CurrentStepCount();
    private:
        void GetCurrentButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
