#include "pch.h"
#include "Helpers.h"

using namespace winrt;
using namespace winrt::BackgroundTask;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Globalization::DateTimeFormatting;
using namespace winrt::Windows::UI::Notifications;

hstring BackgroundTask::FormatDateTime(Windows::Foundation::DateTime value, hstring const& format)
{
    return DateTimeFormatter(format).Format(value);
}

// The "brief" format carries only 6 significant digits.
hstring BackgroundTask::to_brief_hstring(double value)
{
    char temp[32];
    std::to_chars_result result = std::to_chars(std::begin(temp), std::end(temp), value, std::chars_format::general, 6);
    WINRT_ASSERT(result.ec == std::errc{});
    return to_hstring(std::string_view(temp, result.ptr - temp));
}

// Looks up a JsonArray from the saved LocalSettings, or an empty JsonArray if not found.
JsonArray BackgroundTask::LoadSavedJson(hstring const& key)
{
    auto values = Windows::Storage::ApplicationData::Current().LocalSettings().Values();
    hstring json = values.TryLookup(key).try_as<hstring>().value_or(L"");

    if (json.empty())
    {
        // Not present. Return an empty array.
        return JsonArray();
    }
    else
    {
        return JsonValue::Parse(json).GetArray();
    }
}

void BackgroundTask::DisplayToast(hstring const& message)
{
    // Display a toast for each geofence event.
    ToastNotifier ToastNotifier = ToastNotificationManager::CreateToastNotifier();

    // Create a two line toast and add audio reminder

    // Here the xml that will be passed to the
    // ToastNotification for the toast is retrieved
    XmlDocument toastXml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastText02);

    // Set both lines of text
    XmlNodeList toastNodeList = toastXml.GetElementsByTagName(L"text");
    toastNodeList.Item(0).AppendChild(toastXml.CreateTextNode(L"Geolocation Sample"));
    toastNodeList.Item(1).AppendChild(toastXml.CreateTextNode(message));

    // now create a xml node for the audio source
    IXmlNode toastNode = toastXml.SelectSingleNode(L"/toast");
    XmlElement audio = toastXml.CreateElement(L"audio");
    audio.SetAttribute(L"src", L"ms-winsoundevent:Notification.SMS");

    ToastNotification toast(toastXml);
    ToastNotifier.Show(toast);
}

hstring winrt::to_hstring(Windows::Devices::Geolocation::VisitStateChange value)
{
    switch (value)
    {
    case VisitStateChange::TrackingLost:
        return L"TrackingLost";
    case VisitStateChange::Arrived:
        return L"Arrived";
    case VisitStateChange::Departed:
        return L"Departed";
    case VisitStateChange::OtherMovement:
        return L"OtherMovement";
    }
    return to_hstring(static_cast<uint32_t>(value));
}
