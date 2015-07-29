using System;
using Windows.UI.Xaml.Data;

namespace Notifications.Converters
{
    public class TypeToUIElementConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if (value is Type)
            {
                return Activator.CreateInstance(value as Type);
            }

            return value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
