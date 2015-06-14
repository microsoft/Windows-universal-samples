using System;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;


namespace xBindSampleModel
{
    public sealed class ColorToStringConverter : IValueConverter
    {
        static string[] colorNames = { "AliceBlue", "AntiqueWhite", "Aqua", "Aquamarine", "Azure", "Beige", "Bisque", "Black", "BlanchedAlmond", "Blue", "BlueViolet", "Brown", "BurlyWood", "CadetBlue", "Chartreuse", "Chocolate", "Coral", "CornflowerBlue", "Cornsilk", "Crimson", "Cyan", "DarkBlue", "DarkCyan", "DarkGoldenrod", "DarkGray", "DarkGreen", "DarkKhaki", "DarkMagenta", "DarkOliveGreen", "DarkOrange", "DarkOrchid", "DarkRed", "DarkSalmon", "DarkSeaGreen", "DarkSlateBlue", "DarkSlateGray", "DarkTurquoise", "DarkViolet", "DeepPink", "DeepSkyBlue", "DimGray", "DodgerBlue", "Firebrick", "FloralWhite", "ForestGreen", "Fuchsia", "Gainsboro", "GhostWhite", "Gold", "Goldenrod", "Gray", "Green", "GreenYellow", "Honeydew", "HotPink", "IndianRed", "Indigo", "Ivory", "Khaki", "Lavender", "LavenderBlush", "LawnGreen", "LemonChiffon", "LightBlue", "LightCoral", "LightCyan", "LightGoldenrodYellow", "LightGray", "LightGreen", "LightPink", "LightSalmon", "LightSeaGreen", "LightSkyBlue", "LightSlateGray", "LightSteelBlue", "LightYellow", "Lime", "LimeGreen", "Linen", "Magenta", "Maroon", "MediumAquamarine", "MediumBlue", "MediumOrchid", "MediumPurple", "MediumSeaGreen", "MediumSlateBlue", "MediumSpringGreen", "MediumTurquoise", "MediumVioletRed", "MidnightBlue", "MintCream", "MistyRose", "Moccasin", "NavajoWhite", "Navy", "OldLace", "Olive", "OliveDrab", "Orange", "OrangeRed", "Orchid", "PaleGoldenrod", "PaleGreen", "PaleTurquoise", "PaleVioletRed", "PapayaWhip", "PeachPuff", "Peru", "Pink", "Plum", "PowderBlue", "Purple", "Red", "RosyBrown", "RoyalBlue", "SaddleBrown", "Salmon", "SandyBrown", "SeaGreen", "SeaShell", "Sienna", "Silver", "SkyBlue", "SlateBlue", "SlateGray", "Snow", "SpringGreen", "SteelBlue", "Tan", "Teal", "Thistle", "Tomato", "Transparent", "Turquoise", "Violet", "Wheat", "White", "WhiteSmoke", "Yellow", "YellowGreen" };
        static Color[] colorValues = { Colors.AliceBlue, Colors.AntiqueWhite, Colors.Aqua, Colors.Aquamarine, Colors.Azure, Colors.Beige, Colors.Bisque, Colors.Black, Colors.BlanchedAlmond, Colors.Blue, Colors.BlueViolet, Colors.Brown, Colors.BurlyWood, Colors.CadetBlue, Colors.Chartreuse, Colors.Chocolate, Colors.Coral, Colors.CornflowerBlue, Colors.Cornsilk, Colors.Crimson, Colors.Cyan, Colors.DarkBlue, Colors.DarkCyan, Colors.DarkGoldenrod, Colors.DarkGray, Colors.DarkGreen, Colors.DarkKhaki, Colors.DarkMagenta, Colors.DarkOliveGreen, Colors.DarkOrange, Colors.DarkOrchid, Colors.DarkRed, Colors.DarkSalmon, Colors.DarkSeaGreen, Colors.DarkSlateBlue, Colors.DarkSlateGray, Colors.DarkTurquoise, Colors.DarkViolet, Colors.DeepPink, Colors.DeepSkyBlue, Colors.DimGray, Colors.DodgerBlue, Colors.Firebrick, Colors.FloralWhite, Colors.ForestGreen, Colors.Fuchsia, Colors.Gainsboro, Colors.GhostWhite, Colors.Gold, Colors.Goldenrod, Colors.Gray, Colors.Green, Colors.GreenYellow, Colors.Honeydew, Colors.HotPink, Colors.IndianRed, Colors.Indigo, Colors.Ivory, Colors.Khaki, Colors.Lavender, Colors.LavenderBlush, Colors.LawnGreen, Colors.LemonChiffon, Colors.LightBlue, Colors.LightCoral, Colors.LightCyan, Colors.LightGoldenrodYellow, Colors.LightGray, Colors.LightGreen, Colors.LightPink, Colors.LightSalmon, Colors.LightSeaGreen, Colors.LightSkyBlue, Colors.LightSlateGray, Colors.LightSteelBlue, Colors.LightYellow, Colors.Lime, Colors.LimeGreen, Colors.Linen, Colors.Magenta, Colors.Maroon, Colors.MediumAquamarine, Colors.MediumBlue, Colors.MediumOrchid, Colors.MediumPurple, Colors.MediumSeaGreen, Colors.MediumSlateBlue, Colors.MediumSpringGreen, Colors.MediumTurquoise, Colors.MediumVioletRed, Colors.MidnightBlue, Colors.MintCream, Colors.MistyRose, Colors.Moccasin, Colors.NavajoWhite, Colors.Navy, Colors.OldLace, Colors.Olive, Colors.OliveDrab, Colors.Orange, Colors.OrangeRed, Colors.Orchid, Colors.PaleGoldenrod, Colors.PaleGreen, Colors.PaleTurquoise, Colors.PaleVioletRed, Colors.PapayaWhip, Colors.PeachPuff, Colors.Peru, Colors.Pink, Colors.Plum, Colors.PowderBlue, Colors.Purple, Colors.Red, Colors.RosyBrown, Colors.RoyalBlue, Colors.SaddleBrown, Colors.Salmon, Colors.SandyBrown, Colors.SeaGreen, Colors.SeaShell, Colors.Sienna, Colors.Silver, Colors.SkyBlue, Colors.SlateBlue, Colors.SlateGray, Colors.Snow, Colors.SpringGreen, Colors.SteelBlue, Colors.Tan, Colors.Teal, Colors.Thistle, Colors.Tomato, Colors.Transparent, Colors.Turquoise, Colors.Violet, Colors.Wheat, Colors.White, Colors.WhiteSmoke, Colors.Yellow, Colors.YellowGreen };

        public object Convert(object value, Type targetType, object parameter, string language)
        {
            Color src = (Color)value;
            for (int i = 0; i < colorValues.Length; i++)
            {
                if (src == colorValues[i])
                {
                    return colorNames[i];
                }
            }
            return string.Format("#{0:X2}{1:X2}{2:X2}{3:X2}", src.A, src.R, src.G, src.B);
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            string src = (string)value;
            for (int i = 0; i < colorValues.Length; i++)
            {
                if (src == colorNames[i])
                {
                    return colorValues[i];
                }
            }
            if (src.StartsWith("#"))
            {
                return Color.FromArgb(Byte.Parse(src.Substring(1, 2), System.Globalization.NumberStyles.HexNumber),
                    Byte.Parse(src.Substring(3, 2), System.Globalization.NumberStyles.HexNumber),
                    Byte.Parse(src.Substring(5, 2), System.Globalization.NumberStyles.HexNumber),
                    Byte.Parse(src.Substring(7, 2), System.Globalization.NumberStyles.HexNumber));
            }
            throw new ArgumentException("Cannot parse the color");
        }
    }
}
