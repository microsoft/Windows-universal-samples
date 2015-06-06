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
// Scenario1_CurrentActivity.xaml.h
// Declaration of the Scenario1_CurrentActivity class
//

#pragma once

#include "pch.h"
#include "Scenario1_CurrentActivity.g.h"
#include "MainPage.xaml.h"

namespace ActivitySensorCPP
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_CurrentActivity sealed
    {
    public:
        Scenario1_CurrentActivity();

    private:
        SDKTemplate::MainPage^ rootPage;

        void ScenarioGetCurrentActivity(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}