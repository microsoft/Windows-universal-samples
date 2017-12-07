using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;

namespace SDKTemplate
{
    class BooleanToVisibilityConverter : IValueConverter
    {
        public bool IsInverse { get; set; }
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            bool visibility = (bool)value;
            if (IsInverse)
            {
                visibility = !visibility;
            }

            return visibility ? Visibility.Visible : Visibility.Collapsed;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
