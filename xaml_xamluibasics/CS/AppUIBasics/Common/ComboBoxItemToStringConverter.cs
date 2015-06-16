using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Data;

namespace AppUIBasics.Common
{
    public class ComboBoxItemToStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if (value is Windows.UI.Xaml.Controls.ComboBoxItem)
            {
                return (value as Windows.UI.Xaml.Controls.ComboBoxItem).Content;
            }
            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
