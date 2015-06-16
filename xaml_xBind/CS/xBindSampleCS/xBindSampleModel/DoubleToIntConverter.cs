using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Data;

namespace xBindSampleModel
{
    public sealed class DoubleToIntConverter : IValueConverter 
    {
        object IValueConverter.Convert(object value, Type targetType, object parameter, string language)
        {
            if (!value.GetType().Equals(typeof(double)))
            {
                throw new ArgumentException("Only Double is supported");
            }
            if (targetType.Equals(typeof(Int32)))
            {
                return (Int32)((double)value);
            }
            else if (targetType.Equals(typeof(UInt32)))
            {
                return (UInt32)((double)value);
            }
            else
            {
                throw new ArgumentException("Unsuported type {0}", targetType.FullName);
            }
        }

        object IValueConverter.ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return (double)((Int32)value);
        }
    }

    public sealed class IntToDoubleConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            return (double)((Int32)value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return (Int32)((double)value);
        }
    }
}
