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
#include "ListViewEdgeTappedEventArgs.g.h"
#include "EdgeTappedListView.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct ListViewEdgeTappedEventArgs : ListViewEdgeTappedEventArgsT<ListViewEdgeTappedEventArgs>
    {
        ListViewEdgeTappedEventArgs(Windows::UI::Xaml::Controls::ListViewItem const& item) : item(item) {}

        auto Item() { return item; }

    private:
        Windows::UI::Xaml::Controls::ListViewItem item;
    };

    struct EdgeTappedListView : EdgeTappedListViewT<EdgeTappedListView>
    {
        using ListViewEdgeTappedEventHandler = Windows::Foundation::TypedEventHandler<SDKTemplate::EdgeTappedListView, SDKTemplate::ListViewEdgeTappedEventArgs>;

        EdgeTappedListView();

        auto IsItemLeftEdgeTapEnabled() { return isItemLeftEdgeTapEnabled; }
        void IsItemLeftEdgeTapEnabled(bool value) { isItemLeftEdgeTapEnabled = value; }
        auto LeftEdgeBrush() { return leftEdgeBrush; }
        void LeftEdgeBrush(Windows::UI::Xaml::Media::Brush const& value) { leftEdgeBrush = value; }
        auto ItemLeftEdgeTapped(ListViewEdgeTappedEventHandler const& handler) { return itemLeftEdgeTappedEvent.add(handler); }
        void ItemLeftEdgeTapped(event_token const& token) noexcept { return itemLeftEdgeTappedEvent.remove(token); }

    private:
        bool isItemLeftEdgeTapEnabled = false;
        Windows::UI::Xaml::Media::Brush leftEdgeBrush{ nullptr };
        event<ListViewEdgeTappedEventHandler> itemLeftEdgeTappedEvent;
        Windows::UI::Xaml::Controls::ListViewItem listViewItemHighlighted{ nullptr };
        Windows::UI::Xaml::Shapes::Rectangle visualIndicator{ nullptr };

        static constexpr double HIT_TARGET = 32;
        static constexpr double VISUAL_INDICATOR_WIDTH = 12;
        static constexpr wchar_t VISUAL_INDICATOR_NAME[] = L"VisualIndicator";

        void OnContainerContentChanging(Windows::UI::Xaml::Controls::ListViewBase const&, Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs const& args);
        void Element_PointerPressed(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void Element_PointerReleased(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::PointerRoutedEventArgs const&);
        void Element_PointerCaptureLost(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::PointerRoutedEventArgs const&);
        void Element_PointerExited(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::PointerRoutedEventArgs const&);
        void ShowVisual();
        void ClearVisual();

#pragma region Managing container event tokens

        struct ContainerEventTokens : implements<ContainerEventTokens, Windows::Foundation::IInspectable>
        {
            event_token pointerPressedToken{};
            event_token pointerReleasedToken{};
            event_token pointerCaptureLostToken{};
            event_token pointerExitedToken{};
        };

        static Windows::UI::Xaml::DependencyProperty s_containerEventTokensProperty;
        static void RegisterDependencyPropertiesOnce();

#pragma endregion
    };
}
namespace winrt::SDKTemplate::factory_implementation
{
    struct EdgeTappedListView : EdgeTappedListViewT<EdgeTappedListView, implementation::EdgeTappedListView>
    {
    };
}
