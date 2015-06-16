using System;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using xBindSampleModel;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace xBindSampleCS
{
    public sealed partial class OtherBindings : Page
    {
        public int MyInt { get; set; }
        public DataModel Model { get; set; }
        public CSharpModel CSharpModel { get; set; }

        public String DPOnPage
        {
            get { return (String)GetValue(DPOnPageProperty); }
            set { SetValue(DPOnPageProperty, value); }
        }
        public static DependencyProperty DPOnPageProperty { get { return _DPOnPage; } }
        private const string DPOnPageName = "DPOnPage";
        private static readonly DependencyProperty _DPOnPage =
        DependencyProperty.Register(DPOnPageName, typeof(int), typeof(DataModel), new PropertyMetadata(0));

        public xBindSampleModel.IEmployee NullEmployee;
        public String NullStringProperty;

        public OtherBindings()
        {
            this.InitializeComponent();
            this.CSharpModel = new CSharpModel();
            this.Model = new DataModel();
            InitializeValues();
        }

        void InitializeValues()
        {
            this.DPOnPage = "DP on page";
            this.Background = new SolidColorBrush(Colors.Red);
        }

        private void UpdateValuesClick(object sender, RoutedEventArgs e)
        {
            this.Model.UpdateValues();
            this.CSharpModel.UpdateValues();
            this.DPOnPage += "-";
            this.Background = null;
        }

        private void ResetValuesClick(object sender, RoutedEventArgs e)
        {
            this.Model.InitializeValues();
            this.CSharpModel.InitializeValues();
            this.InitializeValues();
        }

        private void UndeferElementClick(object sender, RoutedEventArgs e)
        {
            this.FindName("deferedTextBlock");
        }
    }
}
