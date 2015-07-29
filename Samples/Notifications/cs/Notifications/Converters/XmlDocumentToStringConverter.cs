using System;
using Windows.Data.Xml.Dom;
using Windows.UI.Xaml.Data;

namespace Notifications.Converters
{
    public class XmlDocumentToStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            XmlDocument doc = value as XmlDocument;
            if (doc == null)
                return value;

            return doc.GetXml();
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
