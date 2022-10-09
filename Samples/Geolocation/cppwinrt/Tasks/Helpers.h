#pragma once
namespace winrt::BackgroundTask
{
    hstring FormatDateTime(Windows::Foundation::DateTime value, hstring const& format);
    hstring to_brief_hstring(double value);
    Windows::Data::Json::JsonArray LoadSavedJson(hstring const& key);
    void DisplayToast(hstring const& message);
}

namespace winrt
{
    hstring to_hstring(Windows::Devices::Geolocation::VisitStateChange value);
}
