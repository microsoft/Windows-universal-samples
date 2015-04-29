using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media;
using System.Reflection;
using Windows.UI.Xaml.Controls;

namespace AppUIBasics.Common
{
    public class StringToBrushConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if(value is ComboBoxItem)
            {
                //First see if this brush is defined as a resource
                var brushName = (value as ComboBoxItem).Content.ToString();
                if (App.Current.Resources.ContainsKey(brushName))
                    return (Brush)App.Current.Resources[brushName];

                //Try to get the brush from the Colors static class
                var prop = typeof(Colors).GetRuntimeProperty(brushName);
                if (prop != null)
                {
                    var color = prop.GetValue(null);
                    if (color != null)
                        return new SolidColorBrush((Color)color);
                }
            }
            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
