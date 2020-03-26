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
namespace winrt::MasterDetailApp
{
    struct ItemsDataSource
    {
        static Windows::Foundation::Collections::IVector<Data::Item> GetAllItems()
        {
            static auto items = CreateItems();
            return items;
        }

        static Data::Item GetItemById(int id)
        {
            return GetAllItems().GetAt(id);
        }

    private:
        static Windows::Foundation::Collections::IVector<Data::Item> CreateItems()
        {
            auto now = winrt::clock::now();

            return winrt::single_threaded_vector<Data::Item>({
                Data::Item(0, now, L"Item 1",
                    LR"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer id facilisis lectus. Cras nec convallis ante, quis pulvinar tellus. Integer dictum accumsan pulvinar. Pellentesque eget enim sodales sapien vestibulum consequat.

Maecenas eu sapien ac urna aliquam dictum.

Nullam eget mattis metus.Donec pharetra, tellus in mattis tincidunt, magna ipsum gravida nibh, vitae lobortis ante odio vel quam.)"),
                Data::Item(1, now, L"Item 2",
                LR"(Quisque accumsan pretium ligula in faucibus. Mauris sollicitudin augue vitae lorem cursus condimentum quis ac mauris. Pellentesque quis turpis non nunc pretium sagittis. Nulla facilisi. Maecenas eu lectus ante. Proin eleifend vel lectus non tincidunt. Fusce condimentum luctus nisi, in elementum ante tincidunt nec.

Aenean in nisl at elit venenatis blandit ut vitae lectus.Praesent in sollicitudin nunc.Pellentesque justo augue, pretium at sem lacinia, scelerisque semper erat.Ut cursus tortor at metus lacinia dapibus.)"),
                Data::Item(2, now, L"Item 3",
                LR"(Ut consequat magna luctus justo egestas vehicula. Integer pharetra risus libero, et posuere justo mattis et.

Proin malesuada, libero vitae aliquam venenatis, diam est faucibus felis, vitae efficitur erat nunc non mauris.Suspendisse at sodales erat.
Aenean vulputate, turpis non tincidunt ornare, metus est sagittis erat, id lobortis orci odio eget quam.Suspendisse ex purus, lobortis quis suscipit a, volutpat vitae turpis.)"),
                Data::Item(3, now, L"Item 4",
                LR"(Duis facilisis, quam ut laoreet commodo, elit ex aliquet massa, non varius tellus lectus et nunc. Donec vitae risus ut ante pretium semper. Phasellus consectetur volutpat orci, eu dapibus turpis. Fusce varius sapien eu mattis pharetra.

Nam vulputate eu erat ornare blandit.Proin eget lacinia erat.Praesent nisl lectus, pretium eget leo et, dapibus dapibus velit.Integer at bibendum mi, et fringilla sem.)"),
                });
        }
    };
}