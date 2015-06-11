using System;
using Windows.UI.Xaml.Data;
using Windows.Storage.Streams;
using System.Globalization;

namespace SDKTemplate.Helpers
{
    public class BufferConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            String metadata = String.Empty;
            IBuffer buffer = value as IBuffer;
            if (buffer != null)
            {
                using (var metadataReader = DataReader.FromBuffer(buffer))
                {
                    metadata = metadataReader.ReadString(buffer.Length);
                }
                metadata = String.Format(CultureInfo.InvariantCulture, "({0})", metadata);
            }
            return metadata;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
