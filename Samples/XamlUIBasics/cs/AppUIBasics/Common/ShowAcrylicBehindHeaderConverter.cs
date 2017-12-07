using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;

namespace AppUIBasics.Common
{
    public class ShowAcrylicBehindHeaderConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            switch (value)
            {
                case AllControlsPage page:
                    return Visibility.Visible;
                default:
                    return Visibility.Collapsed;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}