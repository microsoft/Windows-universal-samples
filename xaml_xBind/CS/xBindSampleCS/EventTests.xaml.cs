using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using xBindSampleModel;


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace xBindSampleCS
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class EventTests : Page
    {
        public DataModel Model { get; set; }
        public RoutedEventHandler clickDelegate;

        public EventTests()
        {
            this.InitializeComponent();
            this.Model = new DataModel();
            clickDelegate = this.Click_RegularArgs;
        }

        private void Click_RegularArgs(object sender, RoutedEventArgs e)
        {
            MessageDialog dlg = new MessageDialog("Clicked - Regular Args");
            var t = dlg.ShowAsync();
        }

        private void Click_NoArgs()
        {
            MessageDialog dlg = new MessageDialog("Clicked - No args");
            var t = dlg.ShowAsync();
        }

        private void Click_BaseArgs(object sender, object e)
        {
            MessageDialog dlg = new MessageDialog("Clicked - Base args");
            var t = dlg.ShowAsync();
        }
    }
}
