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
#include "Item.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Item : ItemT<Item>
    {
        Item(int index) : id(to_hstring(index)), imageSource(GenerateImageSource(index)) {}

        auto Id() { return id; }
        auto ImageSource() { return imageSource; }

        static Windows::Foundation::Collections::IObservableVector<SDKTemplate::Item> GetItems(int count);

    private:
        hstring id;
        Windows::UI::Xaml::Media::ImageSource imageSource{ nullptr };

        static Windows::UI::Xaml::Media::ImageSource GenerateImageSource(int index);
    };
}