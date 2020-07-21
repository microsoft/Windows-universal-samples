using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Globalization.Fonts;
using Windows.Graphics.Display;
using Windows.UI.Xaml;
using Windows.UI.Text;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    class LocalFontInfo
    {
        private FontFamily fontFamily;
        private FontWeight fontWeight;
        private FontStyle fontStyle;
        private FontStretch fontStretch;

        public void Set(TextBlock textBlock)
        {
            this.fontFamily = textBlock.FontFamily;
            this.fontWeight = textBlock.FontWeight;
            this.fontStyle = textBlock.FontStyle;
            this.fontStretch = textBlock.FontStretch;
        }

        public void Reset(TextBlock textBlock)
        {
            textBlock.FontFamily = this.fontFamily;
            textBlock.FontWeight = this.fontWeight;
            textBlock.FontStyle = this.fontStyle;
            textBlock.FontStretch = this.fontStretch;
        }
    }
}
