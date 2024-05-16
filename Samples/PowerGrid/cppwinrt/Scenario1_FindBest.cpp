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
#include "Scenario1_FindBest.h"
#include "Scenario1_FindBest.g.cpp"
#include "SampleConfiguration.h"

namespace winrt
{
    using namespace winrt::Windows::Devices::Power;
    using namespace winrt::Windows::Foundation;
    using namespace winrt::Windows::Foundation::Collections;
    using namespace winrt::Windows::Globalization::DateTimeFormatting;
    using namespace winrt::Windows::Globalization::NumberFormatting;
    using namespace winrt::Windows::UI::Xaml;
    using namespace winrt::Windows::UI::Xaml::Navigation;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario1_FindBest::Scenario1_FindBest()
    {
        InitializeComponent();
    }

    void Scenario1_FindBest::OnNavigatedTo(NavigationEventArgs const&)
    {
        forecastUpdatedToken = PowerGridForecast::ForecastUpdated({ get_weak(), &Scenario1_FindBest::PowerGridForecast_ForecastUpdated });
    }

    void Scenario1_FindBest::OnNavigatedFrom(NavigationEventArgs const&)
    {
        PowerGridForecast::ForecastUpdated(forecastUpdatedToken);
    }

    void Scenario1_FindBest::PowerGridForecast_ForecastUpdated(IInspectable const&, IInspectable const&)
    {
        // Notify the user that a new forecast is available
        rootPage.NotifyUser(L"New forecast is available, click \"Find best time\" to find a new best time", NotifyType::StatusMessage);
    }

    // Calculate the index of the forecast entry that contains the requested time.
    // If the time is before the start of the forecast, then returns 0.
    // If the time is past the end of the forecast, then returns the number of forecasts.
    int Scenario1_FindBest::GetForecastIndexContainingTime(PowerGridForecast const& gridForecast, DateTime time)
    {
        TimeSpan blockDuration = gridForecast.BlockDuration();
        auto startBlock = static_cast<int>((time - gridForecast.StartTime()) / blockDuration);
        return std::clamp(startBlock, 0, static_cast<int>(gridForecast.Forecast().Size()));
    }

    fire_and_forget Scenario1_FindBest::FindBest_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        BestTimeRun().Text(L"");
        LowestSeverityRun().Text(L"");
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        // Collect user inputs.
        wchar_t* end;
        auto lookAheadHours = std::wcstoul(HoursAheadTextBox().Text().data(), &end, 10);
        if (lookAheadHours == 0 || lookAheadHours == ULONG_MAX)
        {
            rootPage.NotifyUser(L"Invalid number of hours to look ahead", NotifyType::ErrorMessage);
            return;
        }
        TimeSpan lookAhead = std::chrono::hours(lookAheadHours);

        bool restrictToLowUXImpact = LowUXImpactCheckBox().IsChecked().Value();

        // Do the calculations on a background thread.
        co_await winrt::resume_background();

        PowerGridForecast gridForecast = PowerGridForecast::GetForecast();

        // Find the first and last blocks that include the time range we are
        // interested in.
        DateTime startTime = clock::now();
        DateTime endTime = startTime + lookAhead;

        int startBlock = GetForecastIndexContainingTime(gridForecast, startTime);
        int endBlock = GetForecastIndexContainingTime(gridForecast, endTime + gridForecast.BlockDuration());

        double lowestSeverity = (std::numeric_limits<double>::max)();
        DateTime timeWithLowestSeverity = (DateTime::max)();

        for (int index = startBlock; index < endBlock; ++index)
        {
            PowerGridData data = gridForecast.Forecast().GetAt(index);

            // If we are restricting to low impact, then use only low impact time periods.
            if (restrictToLowUXImpact && !data.IsLowUserExperienceImpact())
            {
                continue;
            }

            // If the severity is not an improvement, then don't use this one.
            double severity = data.Severity();
            if (severity >= lowestSeverity)
            {
                continue;
            }

            lowestSeverity = severity;
            timeWithLowestSeverity = gridForecast.StartTime() + index * gridForecast.BlockDuration();
        }

        // Report the results on the UI thread.
        co_await winrt::resume_foreground(Dispatcher());

        if (lowestSeverity <= 1.0)
        {
            DateTimeFormatter dateFormatter = GetDateFormatter();
            DecimalFormatter severityFormatter = GetSeverityFormatter();
            BestTimeRun().Text(
                dateFormatter.Format(timeWithLowestSeverity) + L" to " +
                dateFormatter.Format(timeWithLowestSeverity + gridForecast.BlockDuration()));
            LowestSeverityRun().Text(severityFormatter.Format(lowestSeverity));
        }
        else
        {
            rootPage.NotifyUser(L"Unable to find a good time to do work", NotifyType::ErrorMessage);
        }
    }
}
