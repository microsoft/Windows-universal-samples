using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Data;

namespace AppUIBasics.Common
{
    public class DoubleToIntConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if(value != null && value is double)
            {
                int retVal;
                if (int.TryParse(value.ToString(), out retVal))
                    return retVal;
            }
            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            if(value != null && value is int)
            {
                return (double)value;
            }
            return null;
        }
    }
}
