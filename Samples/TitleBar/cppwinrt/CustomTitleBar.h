//
// Declaration of the CustomTitleBar class.
//

#pragma once

#include "CustomTitleBar.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct CustomTitleBar : CustomTitleBarT<CustomTitleBar>
    {
        CustomTitleBar();

        Windows::UI::Xaml::UIElement SetPageContent(Windows::UI::Xaml::UIElement const& newContent);
        void EnableControlsInTitleBar(bool enable);

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler) { return propertyChanged.add(handler); }

        void PropertyChanged(event_token const& token) noexcept { propertyChanged.remove(token); }

        Windows::UI::Xaml::Thickness CoreTitleBarPadding();
        double CoreTitleBarHeight() { return coreTitleBar.Height(); }

        void CustomTitleBar_Loaded(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void CustomTitleBar_Unloaded(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
    private:
        Windows::ApplicationModel::Core::CoreApplicationViewTitleBar coreTitleBar = Windows::ApplicationModel::Core::CoreApplication::GetCurrentView().TitleBar();
        Windows::UI::Xaml::UIElement pageContent{ nullptr };

        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged;

        event_token layoutMetricsChangedToken;
        event_token isVisibleChangedToken;
        event_token windowSizeChangedToken;

        void UpdateLayoutMetrics();
        void UpdatePositionAndVisibility();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct CustomTitleBar : CustomTitleBarT<CustomTitleBar, implementation::CustomTitleBar>
    {
    };
}
