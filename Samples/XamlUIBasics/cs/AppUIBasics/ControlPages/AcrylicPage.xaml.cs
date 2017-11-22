using System.Linq;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Shapes;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class AcrylicPage : Page
    {
        public AcrylicPage()
        {
            this.InitializeComponent();
            Loaded += AcrylicPage_Loaded;
        }

        private void AcrylicPage_Loaded(object sender, RoutedEventArgs e)
        {
            ColorSelector.SelectedIndex = ColorSelectorInApp.SelectedIndex = 0;
            FallbackColorSelector.SelectedIndex = FallbackColorSelectorInApp.SelectedIndex = 0;
            OpacitySlider.Value = OpacitySliderInApp.Value = 0.8;
        }

        private void Slider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            Rectangle shape = (sender == OpacitySliderInApp) ? CustomAcrylicShapeInApp : CustomAcrylicShape;
            ((AcrylicBrush)shape.Fill).TintOpacity = e.NewValue;
        }

        private void ColorSelector_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Rectangle shape = (sender == ColorSelectorInApp) ? CustomAcrylicShapeInApp : CustomAcrylicShape;
            ((AcrylicBrush)shape.Fill).TintColor = ((SolidColorBrush)e.AddedItems.First()).Color;
        }

        private void FallbackColorSelector_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Rectangle shape = (sender == FallbackColorSelectorInApp) ? CustomAcrylicShapeInApp : CustomAcrylicShape;
            ((AcrylicBrush)shape.Fill).FallbackColor = ((SolidColorBrush)e.AddedItems.First()).Color;
        }
    }
}
