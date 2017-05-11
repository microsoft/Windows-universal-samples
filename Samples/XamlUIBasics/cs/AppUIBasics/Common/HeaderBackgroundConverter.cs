using System;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media;

namespace AppUIBasics.Common
{
    public class HeaderBackgroundConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            switch (value)
            {
                case SearchResultsPage page:
                    return Application.Current.Resources["HomePageBackgroundBrush"] as SolidColorBrush;
                case SectionPage page:
                    return Application.Current.Resources["HomePageBackgroundBrush"] as SolidColorBrush;
                default:
                    return Application.Current.Resources["ApplicationPageBackgroundThemeBrush"] as SolidColorBrush;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
