#pragma once

#include "Scenario1_Basic.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Basic : Scenario1_BasicT<Scenario1_Basic>
    {
        Scenario1_Basic();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        // These methods are public so they can be called by binding.
        void ResizeView_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void SetMinimumSize_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
        event_token windowSizeChangedToken;

        void UpdateContent();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Basic : Scenario1_BasicT<Scenario1_Basic, implementation::Scenario1_Basic>
    {
    };
}
