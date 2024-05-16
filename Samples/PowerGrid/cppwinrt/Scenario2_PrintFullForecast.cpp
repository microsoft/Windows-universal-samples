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
#include "Scenario2_PrintFullForecast.h"
#include "Scenario2_PrintFullForecast.g.cpp"
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
    Scenario2_PrintFullForecast::Scenario2_PrintFullForecast()
    {
        InitializeComponent();
    }

    void Scenario2_PrintFullForecast::OnNavigatedTo(NavigationEventArgs const&)
    {
        forecastUpdatedToken = PowerGridForecast::ForecastUpdated({ get_weak(), &Scenario2_PrintFullForecast::PowerGridForecast_ForecastUpdated });
    }

    void Scenario2_PrintFullForecast::OnNavigatedFrom(NavigationEventArgs const&)
    {
        PowerGridForecast::ForecastUpdated(forecastUpdatedToken);
    }

    void Scenario2_PrintFullForecast::PowerGridForecast_ForecastUpdated(IInspectable const&, IInspectable const&)
    {
        // Notify the user that a new forecast is available
        rootPage.NotifyUser(L"New forecast is available, click \"Get forecast\" to show the new forecast", NotifyType::StatusMessage);
    }

    fire_and_forget Scenario2_PrintFullForecast::GetForecastButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Erase the previous forecast.
        ForecastStartTimeRun().Text(L"");
        ForecastBlockDurationRun().Text(L"");
        ForecastList().ItemsSource(nullptr);

        // Use a background thread to get the forecast, so we don't block the UI thread.
        co_await winrt::resume_background();
        PowerGridForecast gridForecast = PowerGridForecast::GetForecast();
        co_await winrt::resume_foreground(Dispatcher());

        // If the API cannot obtain a forecast, the forecast is empty
        IVectorView<PowerGridData> forecast = gridForecast.Forecast();
        if (forecast.Size() == 0)
        {
            rootPage.NotifyUser(L"No forecast available. Try again later.", NotifyType::ErrorMessage);
            co_return;
        }

        // Add the forecast to the output.
        DateTime blockStartTime = gridForecast.StartTime();
        TimeSpan blockDuration = gridForecast.BlockDuration();

        // Set the UI for the start time and duration
        DateTimeFormatter dateFormatter = GetDateFormatter();
        DecimalFormatter severityFormatter = GetSeverityFormatter();

        ForecastStartTimeRun().Text(dateFormatter.Format(blockStartTime));
        ForecastBlockDurationRun().Text(winrt::to_hstring(std::chrono::round<std::chrono::minutes>(blockDuration).count()));

        // Start with a header item.
        std::vector<SDKTemplate::ForecastItem> items({
            make<ForecastItem>(L"Date/Time", L"Severity", L"Low impact?")
        });

        // Add each block as an item in the forecast list.
        for (PowerGridData const& data : forecast)
        {
            items.push_back(make<ForecastItem>(dateFormatter.Format(blockStartTime),
                severityFormatter.Format(data.Severity()),
                winrt::to_hstring(data.IsLowUserExperienceImpact())));
            blockStartTime += blockDuration;
        }

        ForecastList().ItemsSource(winrt::single_threaded_observable_vector(std::move(items)));
    }
}
