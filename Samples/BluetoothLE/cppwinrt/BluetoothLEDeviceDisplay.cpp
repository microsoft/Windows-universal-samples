#include "pch.h"
#include "BluetoothLEDeviceDisplay.h"
#include "BluetoothLEDeviceDisplay.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Enumeration;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Media::Imaging;

namespace winrt::SDKTemplate::implementation
{
    BluetoothLEDeviceDisplay::BluetoothLEDeviceDisplay(Windows::Devices::Enumeration::DeviceInformation const& deviceInfoIn)
        : m_deviceInformation(deviceInfoIn)
    {
        UpdateGlyphBitmapImage();
    }

    bool BluetoothLEDeviceDisplay::LookupBooleanProperty(param::hstring const& property)
    {
        auto value = m_deviceInformation.Properties().TryLookup(property);
        return value && unbox_value<bool>(value);
    }

    void BluetoothLEDeviceDisplay::Update(DeviceInformationUpdate const& deviceInfoUpdate)
    {
        m_deviceInformation.Update(deviceInfoUpdate);

        OnPropertyChanged(L"Id");
        OnPropertyChanged(L"Name");
        OnPropertyChanged(L"DeviceInformation");
        OnPropertyChanged(L"IsPaired");
        OnPropertyChanged(L"IsConnected");
        OnPropertyChanged(L"Properties");
        OnPropertyChanged(L"IsConnectable");

        UpdateGlyphBitmapImage();
    }

    event_token BluetoothLEDeviceDisplay::PropertyChanged(PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }

    void BluetoothLEDeviceDisplay::PropertyChanged(event_token const& token) noexcept
    {
        m_propertyChanged.remove(token);
    }

    void BluetoothLEDeviceDisplay::OnPropertyChanged(param::hstring const& property)
    {
        m_propertyChanged(*this, PropertyChangedEventArgs(property));
    }

    fire_and_forget BluetoothLEDeviceDisplay::UpdateGlyphBitmapImage()
    {
        auto lifetime = get_strong();
        DeviceThumbnail deviceThumbnail = co_await m_deviceInformation.GetGlyphThumbnailAsync();
        BitmapImage glyphBitmapImage;
        co_await glyphBitmapImage.SetSourceAsync(deviceThumbnail);
        m_glyphBitmapImage = glyphBitmapImage;
        OnPropertyChanged(L"GlyphBitmapImage");
    }
}
