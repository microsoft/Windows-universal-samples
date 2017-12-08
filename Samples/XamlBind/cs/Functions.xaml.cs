using System;
using System.Collections.ObjectModel;
using System.Reflection;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using xBindSampleModel;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace xBindSampleCS
{
    public sealed partial class Functions : Page
    {
        #region Model fields, properties & DP's
        public DataModel Model { get; set; }

        public CSharpModel CSharpModel { get; set; }

        public xBindSampleModel.IEmployee NullEmployee;
        public String NullStringProperty;

        ObservableCollection<ColorEntry> ColorEntries = new ObservableCollection<ColorEntry>();

        #endregion

        public Functions()
        {
            this.Model = new DataModel();
            this.CSharpModel = new CSharpModel();
            this.InitializeComponent();
            InitializeValues();
        }

        #region Model initialization & updates
        void InitializeValues()
        {
        }

        private void UpdateValuesClick(object sender, RoutedEventArgs e)
        {
            this.Model.UpdateValues();
            this.CSharpModel.UpdateValues();
        }

        private void ResetValuesClick(object sender, RoutedEventArgs e)
        {
            this.Model.InitializeValues();
            this.CSharpModel.InitializeValues();
            this.InitializeValues();
        }
        #endregion

        string GetTime()
        {
            return DateTime.Now.ToString();
        }

        string Display(string stringArg, int intArg, double floatArg, bool boolArg)
        {
            return $"String is: {stringArg}, Integer is: {intArg}, Float is: {floatArg}, Boolean is {boolArg}";
        }

        string countVowels(string words)
        {
            int count = 0;
            foreach (char c in words.ToLower())
            {
                if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
                {
                    count++;
                }
            }
            return count.ToString();
        }

        public string CountEmployees(IEmployee e)
        {
            return CountEmployeesinternal(e).ToString();
        }

        public int CountEmployeesinternal(IEmployee e)
        {
            int result = 1;
            if (e is Manager)
                foreach (IEmployee c in ((Manager)e).ReportsList)
                {
                    result += CountEmployeesinternal(c);
                }
            return result;
        }

        public SolidColorBrush MakeColor(double red, double green, double blue)
        {
            return new SolidColorBrush(Color.FromArgb(255, (byte)red, (byte)green, (byte)blue));
        }

        public string ColorValue(double red, double green, double blue)
        {
            return $"#{(int)red:X}{(int)green:X}{(int)blue:X}";
        }

        public void ProcessColor(string value)
        {
            try
            {
                if (value.Length == 7 && value.StartsWith("#"))
                {
                    int red = int.Parse(value.Substring(1, 2), System.Globalization.NumberStyles.HexNumber);
                    int green = int.Parse(value.Substring(3, 2), System.Globalization.NumberStyles.HexNumber);
                    int blue = int.Parse(value.Substring(5, 2), System.Globalization.NumberStyles.HexNumber);

                    RedSlider.Value = red;
                    GreenSlider.Value = green;
                    BlueSlider.Value = blue;
                }
            }
            catch (Exception ex)
            {
                // raise error in the application
            }
        }

        private void PopulateColorsClick(object sender, RoutedEventArgs e)
        {
            foreach (var p in typeof(Windows.UI.Colors).GetTypeInfo().DeclaredProperties)
            {
                if (p.PropertyType == typeof(Color))
                {
                    ColorEntry ce = new ColorEntry() { ColorName = p.Name };
                    ce.Color = (Color)p.GetMethod.Invoke(null, new object[0]);
                    ColorEntries.Add(ce);
                }
            }
        }

        private void ClearColorsClick(object sender, RoutedEventArgs e)
        {
            ColorEntries.Clear();
        }


        public Visibility ShowPlaceholder(int count)
        {
            return (count > 0) ? Visibility.Collapsed : Visibility.Visible;
        }

        public static SolidColorBrush TextColor(Color c)
        {
            return new SolidColorBrush(((c.R * 0.299 + c.G * 0.587 + c.B * 0.114) > 150) ? Colors.Black : Colors.White);
        }
    }


    class ColorEntry
    {
        public string ColorName { get; set; }
        public Color Color { get; set; }

        public SolidColorBrush Brushify(Color c)
        {
            return new SolidColorBrush(c);
        }


    }
}
