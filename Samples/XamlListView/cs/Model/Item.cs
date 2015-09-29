using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.UI.Xaml.Media.Imaging;

namespace ListViewSample.Model
{
    public class Item
    {
        #region Properties
        private BitmapImage imageSource;
        private static List<BitmapImage> sources;
        private string id;

        public BitmapImage ImageSource
        {
            get
            {
                return imageSource;
            }

            set
            {
                imageSource = value;
            }
        }

        public string Id
        {
            get
            {
                return id;
            }

            set
            {
                id = value;
            }
        }

        public Item()
        {
            imageSource = new BitmapImage();
            id = string.Empty;

            sources = new List<BitmapImage>()
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
        }

        public static Item GetNewItem(int i, int j)
        {
            return new Item()
            {
                imageSource = GenerateImageSource(j),
                id = i.ToString()
            };
        }

        public static ObservableCollection<Item> GetItems(int numberOfContacts)
        {
            ObservableCollection<Item> items = new ObservableCollection<Item>();

            for (int i = 0; i < numberOfContacts; i++)
            {
                items.Add(GetNewItem(i, i % 11));
            }
            return items;
        }

        private static BitmapImage GenerateImageSource(int i)
        {
            return sources[i];
        }
        #endregion
    }
}