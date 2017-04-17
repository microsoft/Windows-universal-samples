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
#include "pch.h"

namespace SDKTemplate
{
    value struct Scenario;

    partial ref class MainPage
    {
    internal:
        static property Platform::String^ FEATURE_NAME
        {
            Platform::String^ get()
            {
                return "Orientation Sensor";
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

        static void SetReadingText(Windows::UI::Xaml::Controls::TextBlock^ textBlock, Windows::Devices::Sensors::OrientationSensorReading^ reading);

        property Windows::Devices::Sensors::SensorReadingType SensorReadingType
        {
            Windows::Devices::Sensors::SensorReadingType get()
            {
                return sensorReadingType;
            }
            void set(Windows::Devices::Sensors::SensorReadingType value)
            {
                sensorReadingType = value;
            }
        }

        property Windows::Devices::Sensors::SensorOptimizationGoal SensorOptimizationGoal
        {
            Windows::Devices::Sensors::SensorOptimizationGoal get()
            {
                return sensorOptimizationGoal;
            }
            void set(Windows::Devices::Sensors::SensorOptimizationGoal value)
            {
                sensorOptimizationGoal = value;
            }
        }

        property Platform::String^ SensorDescription
        {
            Platform::String^ get()
            {
                return SensorReadingType.ToString() + " orientation sensor with " + SensorOptimizationGoal.ToString() + " optimization goal";
            }
        }

    private:
        static Platform::Array<Scenario>^ scenariosInner;
        Windows::Devices::Sensors::SensorReadingType sensorReadingType;
        Windows::Devices::Sensors::SensorOptimizationGoal sensorOptimizationGoal;
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };
}
