using Windows.Foundation.Metadata;
using Windows.UI;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace AppUIBasics.ControlPages
{
    public sealed partial class CommandBarFlyoutPage : Page
    {
        private bool wasLeftPointerPressed = false;

        public CommandBarFlyoutPage()
        {
            this.InitializeComponent();
        }

        public void OnClosed(object sender, object args)
        {
            myImageBorder.BorderBrush = new SolidColorBrush(Colors.Transparent);
        }

        public void Image_PointerPressed(object sender, PointerRoutedEventArgs args)
        {
            wasLeftPointerPressed = args.GetCurrentPoint(myImageBorder).Properties.IsLeftButtonPressed;
        }

        public void Image_PointerReleased(object sender, PointerRoutedEventArgs args)
        {
            myImageBorder.BorderBrush = new SolidColorBrush(Colors.Blue);

            if (wasLeftPointerPressed)
            {
                if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))
                {
                    FlyoutShowOptions myOption = new FlyoutShowOptions();
                    myOption.ShowMode = FlyoutShowMode.Transient;
                    myOption.Placement = FlyoutPlacementMode.RightEdgeAlignedTop;
                    CommandBarFlyout1.ShowAt(Image1, myOption);
                }
                else
                {
                    CommandBarFlyout1.ShowAt(Image1);
                }
                wasLeftPointerPressed = false;
            }
            args.Handled = true;
        }

        private void OnElementClicked(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // Do custom logic
            SelectedOptionText.Text = "You clicked: " + (sender as AppBarButton).Label;
        }
    }
}
