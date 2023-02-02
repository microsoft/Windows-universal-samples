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
#include "Scenario6_ScrollIntoView.h"
#include "Scenario6_ScrollIntoView.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;

namespace winrt::SDKTemplate::implementation
{
    Scenario6_ScrollIntoView::Scenario6_ScrollIntoView()
    {
        InitializeComponent();
    }

    void Scenario6_ScrollIntoView::Scroll_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        hstring text = scrollId().Text();
        wchar_t* end;
        unsigned long index = std::wcstoul(text.c_str(), &end, 10);
        if (text.c_str() == end)
        {
            index = ~0UL; // invalid
        }
        if (index < ItemsListView().Items().Size())
        {
            ScrollIntoViewAlignment alignment = ScrollAlignment().SelectedItem().as<FrameworkElement>().Tag().as<ScrollIntoViewAlignment>();
            ItemsListView().ScrollIntoView(ItemsListView().Items().GetAt(index), alignment);
            scrollId().BorderBrush(ValidFormatBrush());
        }
        else
        {
            scrollId().BorderBrush(InvalidFormatBrush());
        }
    }
}
