using AppUIBasics.Data;
using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

namespace AppUIBasics.Common
{
    public class ImageLoader
    {
        public static string GetSource(DependencyObject obj)
        {
            return (string)obj.GetValue(SourceProperty);
        }

        public static void SetSource(DependencyObject obj, string value)
        {
            obj.SetValue(SourceProperty, value);
        }

        // Using a DependencyProperty as the backing store for Path.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty SourceProperty =
            DependencyProperty.RegisterAttached("Source", typeof(string), typeof(ImageLoader), new PropertyMetadata(string.Empty, OnPropertyChanged));

        private async static void OnPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var image = d as Image;
            if (image != null)
            {
                var item = await ControlInfoDataSource.Instance.GetItemAsync(e.NewValue?.ToString());
                if (item?.ImagePath != null)
                {
                    Uri imageUri = new Uri(item.ImagePath, UriKind.Absolute);
                    BitmapImage imageBitmap = new BitmapImage(imageUri);
                    image.Source = imageBitmap;
                }
            }
        }
    }
}