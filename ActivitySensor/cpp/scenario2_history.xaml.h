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
// Scenario2_History.xaml.h
// Declaration of the Scenario2_History class
//

#pragma once

#include "pch.h"
#include "Scenario2_History.g.h"
#include "MainPage.xaml.h"

namespace ActivitySensorCPP
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_History sealed
    {
    public:
        Scenario2_History();

    private:
        SDKTemplate::MainPage^ rootPage;

        void ScenarioGetActivityHistory(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
