using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Data;

namespace xBindSampleModel
{
    public sealed class BoolToVisibilityConverter : IValueConverter 
    {
        object IValueConverter.Convert(object value, Type targetType, object parameter, string language)
        {
            if (!value.GetType().Equals(typeof(bool)))
            {
                throw new ArgumentException("Only Boolean is supported");
            }
            if (targetType.Equals(typeof(Windows.UI.Xaml.Visibility)))
            {
                return ((bool)value) ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;
            }
            else
            {
                throw new ArgumentException("Unsuported type {0}", targetType.FullName);
            }
        }

        object IValueConverter.ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
