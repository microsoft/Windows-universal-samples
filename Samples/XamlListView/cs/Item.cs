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
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.UI.Xaml.Media.Imaging;

namespace SDKTemplate
{
    public class Item
    {
        private static List<BitmapImage> sources = new List<BitmapImage>()
            {
                new BitmapImage(new Uri("ms-appx:///Assets/cliff.jpg")),
                new BitmapImage(new Uri("ms-appx:///Assets/grapes_background.jpg")),
                new BitmapImage(new Uri("ms-appx:///Assets/LandscapeImage13.jpg")),
                new BitmapImage(new Uri("ms-appx:///Assets/LandscapeImage2.jpg")),
                new BitmapImage(new Uri("ms-appx:///Assets/LandscapeImage24.jpg")),
                new BitmapImage(new Uri("ms-appx:///Assets/LandscapeImage3.jpg")),
                new BitmapImage(new Uri("ms-appx:///Assets/LandscapeImage6.jpg")),
                new BitmapImage(new Uri("ms-appx:///Assets/LandscapeImage7.jpg")),
                new BitmapImage(new Uri("ms-appx:///Assets/Ring01.jpg")),
                new BitmapImage(new Uri("ms-appx:///Assets/Ring03Part1.jpg")),
                new BitmapImage(new Uri("ms-appx:///Assets/Ring03Part3.jpg")),
                new BitmapImage(new Uri("ms-appx:///Assets/sunset.jpg"))
            };

        public BitmapImage ImageSource { get; set; } = new BitmapImage();

        public string Id { get; set; } = string.Empty;

        public static Item GetNewItem(int i, int j)
        {
            return new Item()
            {
                ImageSource = GenerateImageSource(j),
                Id = i.ToString()
            };
        }

        public static ObservableCollection<Item> GetItems(int count)
        {
            ObservableCollection<Item> items = new ObservableCollection<Item>();

            for (int i = 0; i < count; i++)
            {
                items.Add(GetNewItem(i, i % 11));
            }
            return items;
        }

        private static BitmapImage GenerateImageSource(int i)
        {
            return sources[i];
        }
    }
}