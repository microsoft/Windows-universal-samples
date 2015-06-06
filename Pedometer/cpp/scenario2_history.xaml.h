// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario2_History.g.h"
#include <collection.h>
#include "MainPage.xaml.h"

namespace PedometerCPP
{
    /// <summary>
    /// Represents a history record object that gets binded to the ListView 
    /// controller in the XAML
    /// </summary>
    [Windows::UI::Xaml::Data::Bindable]
    public ref class HistoryRecord sealed
    {
    public:
        property Platform::String^ TimeStamp
        {
            Platform::String^ get()
            {
                Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ timestampFormatter = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("shortdate longtime");
                return timestampFormatter->Format(recordTimestamp);
            }
        }

        property Platform::String^ StepKind
        {
            Platform::String^ get()
            {
                Platform::String^ stepString = nullptr;
                switch (stepKind)
                {
                case Windows::Devices::Sensors::PedometerStepKind::Unknown:
                    stepString = ref new Platform::String(L"Unknown");
                    break;
                case Windows::Devices::Sensors::PedometerStepKind::Walking:
                    stepString = ref new Platform::String(L"Walking");
                    break;
                case Windows::Devices::Sensors::PedometerStepKind::Running:
                    stepString = ref new Platform::String(L"Running");
                    break;
                default:
                    stepString = ref new Platform::String(L"Invalid Stepy Kind");
                    break;
                }
                return stepString;
            }
        }

        property INT32 StepsCount
        {
            INT32 get()
            {
                return stepsCount;
            }
        }

        property long long StepsDuration
        {
            long long get()
            {
                // return Duration in ms
                return (stepsDuration.Duration / 10000);
            }
        }

        HistoryRecord(Windows::Devices::Sensors::PedometerReading^ reading)
        {
            stepKind = reading->StepKind;
            stepsCount = reading->CumulativeSteps;
            recordTimestamp.UniversalTime = reading->Timestamp.UniversalTime;
        }
    private:

        Windows::Foundation::DateTime recordTimestamp;
        Windows::Devices::Sensors::PedometerStepKind stepKind;
        INT32 stepsCount;
        Windows::Foundation::TimeSpan stepsDuration;
    };

    /// <summary>
    /// History Page that gets loaded when 'History' scenario is selected.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_History sealed
    {
    public:
        Scenario2_History();
    private:
        SDKTemplate::MainPage^ rootPage;
        void AllHistory_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SpecificHistory_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        Platform::Collections::Vector<HistoryRecord>^ recordCollection;
        void GetHistory_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        bool getAllHistory;
        Platform::Collections::Vector<HistoryRecord^> ^historyRecords;
    };
}
