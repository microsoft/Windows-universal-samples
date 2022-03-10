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
#include "Item.h"
#include "Item.g.cpp"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;

namespace winrt::SDKTemplate::implementation
{
    ImageSource Item::GenerateImageSource(int index)
    {
        static std::vector<BitmapImage> sources({
            BitmapImage(Uri(L"ms-appx:///Assets/cliff.jpg")),
            BitmapImage(Uri(L"ms-appx:///Assets/grapes_background.jpg")),
            BitmapImage(Uri(L"ms-appx:///Assets/LandscapeImage13.jpg")),
            BitmapImage(Uri(L"ms-appx:///Assets/LandscapeImage2.jpg")),
            BitmapImage(Uri(L"ms-appx:///Assets/LandscapeImage24.jpg")),
            BitmapImage(Uri(L"ms-appx:///Assets/LandscapeImage3.jpg")),
            BitmapImage(Uri(L"ms-appx:///Assets/LandscapeImage6.jpg")),
            BitmapImage(Uri(L"ms-appx:///Assets/LandscapeImage7.jpg")),
            BitmapImage(Uri(L"ms-appx:///Assets/Ring01.jpg")),
            BitmapImage(Uri(L"ms-appx:///Assets/Ring03Part1.jpg")),
            BitmapImage(Uri(L"ms-appx:///Assets/Ring03Part3.jpg")),
            BitmapImage(Uri(L"ms-appx:///Assets/sunset.jpg")),
            });

        return sources[index % sources.size()];
    }

    IObservableVector<SDKTemplate::Item> Item::GetItems(int count)
    {
        std::vector<SDKTemplate::Item> items;
        std::generate_n(std::back_inserter(items), count, [i = 0]() mutable { return make<Item>(i++); });
        return single_threaded_observable_vector(std::move(items));
    }
}