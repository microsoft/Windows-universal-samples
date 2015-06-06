using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using xBindSampleModel;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace xBindSampleCS
{
    public sealed partial class BasicTests : Page
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

        public BasicTests()
        {
            this.Model = new DataModel();
            this.CSharpModel = new CSharpModel();
            this.InitializeComponent();
            InitializeValues();
        }

        void InitializeValues()
        {
            this.DPOnPage = "DP on page";
        }

        private void UpdateValuesClick(object sender, RoutedEventArgs e)
        {
            this.Model.UpdateValues();
            this.CSharpModel.UpdateValues();
            this.DPOnPage += "-";
        }

        private void ResetValuesClick(object sender, RoutedEventArgs e)
        {
            this.Model.InitializeValues();
            this.CSharpModel.InitializeValues();
            this.InitializeValues();
        }

    }
}
