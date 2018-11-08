using System;
using System.Numerics;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace AppUIBasics.ControlPages
{
    public sealed partial class ImplicitTransitionPage : Page
    {
        public ImplicitTransitionPage()
        {
            this.InitializeComponent();

            SetupImplicitTransitionsIfAPIAvailable();
        }

        void SetupImplicitTransitionsIfAPIAvailable()
        {
            // If the API is not present, simply no-op.
            if (!(ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))) return;

            OpacityRectangle.OpacityTransition = new ScalarTransition();
            RotationRectangle.RotationTransition = new ScalarTransition();
            ScaleRectangle.ScaleTransition = new Vector3Transition();
            TranslateRectangle.TranslationTransition = new Vector3Transition();
            BrushPresenter.BackgroundTransition = new BrushTransition();
            ThemeExampleGrid.BackgroundTransition = new BrushTransition();
        }

        private void OpacityButton_Click(object sender, RoutedEventArgs e)
        {
            // If the implicit animation API is not present, simply no-op. 
            if (!(ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))) return;

            var customValue = Convert.ToDouble(OpacityTextBox.Text);

            if (customValue >= 0.0 && customValue <= 1.0)
            {
                OpacityRectangle.Opacity = customValue;
            }
        }
        private void RotationButton_Click(object sender, RoutedEventArgs e)
        {
            // If the implicit animation API is not present, simply no-op. 
            if (!(ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))) return;

            RotationRectangle.CenterPoint = new System.Numerics.Vector3((float)RotationRectangle.ActualWidth / 2, (float)RotationRectangle.ActualHeight / 2, 0f);

            float customValue = (float)Convert.ToDouble(RotationTextBox.Text);

            if (customValue >= 0.0 && customValue <= 360.0)
            {
                RotationRectangle.Rotation = customValue;
            }
            else
            {
                RotationRectangle.Rotation = 0;
            }
        }
        private void ScaleButton_Click(object sender, RoutedEventArgs e)
        {
            // If the implicit animation API is not present, simply no-op. 
            if (!(ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))) return;

            var _scaleTransition = ScaleRectangle.ScaleTransition;

            _scaleTransition.Components = ((ScaleX.IsChecked == true) ? Vector3TransitionComponents.X : 0) |
                                         ((ScaleY.IsChecked == true) ? Vector3TransitionComponents.Y : 0) |
                                         ((ScaleZ.IsChecked == true) ? Vector3TransitionComponents.Z : 0);

            float customValue;
            if (sender != null && (sender as Button).Tag != null)
            {
                customValue = (float)Convert.ToDouble((sender as Button).Tag);
            }
            else
            {
                customValue = (float)Convert.ToDouble(ScaleTextBox.Text);
            }

            if (customValue > 0.0 && customValue <= 5)
            {
                ScaleRectangle.Scale = new Vector3(customValue);
            }
        }

        private void TranslateButton_Click(object sender, RoutedEventArgs e)
        {
            // If the implicit animation API is not present, simply no-op. 
            if (!(ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))) return;

            var _translationTransition = TranslateRectangle.TranslationTransition;

            _translationTransition.Components = ((TranslateX.IsChecked == true) ? Vector3TransitionComponents.X : 0) |
                                         ((TranslateY.IsChecked == true) ? Vector3TransitionComponents.Y : 0) |
                                         ((TranslateZ.IsChecked == true) ? Vector3TransitionComponents.Z : 0);

            float customValue;
            if (sender != null && (sender as Button).Tag != null)
            {
                customValue = (float)Convert.ToDouble((sender as Button).Tag);
            }
            else
            {
                customValue = (float)Convert.ToDouble(TranslationTextBox.Text);
            }

            if (customValue >= 0.0 && customValue <= 200.0)
            {
                TranslateRectangle.Translation = new Vector3(customValue);
            }
        }

        private void TextBox_KeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Enter)
            {
                if ((String)(sender as TextBox).Header == "Opacity")
                {
                    OpacityButton_Click(null, null);
                }
                if ((String)(sender as TextBox).Header == "Rotation")
                {
                    RotationButton_Click(null, null);
                }
                if ((String)(sender as TextBox).Header == "Scale")
                {
                    ScaleButton_Click(null, null);
                }
                if ((String)(sender as TextBox).Header == "Translation")
                {
                    TranslateButton_Click(null, null);
                }
            }
        }

        private void BackgroundButton_Click(object sender, RoutedEventArgs e)
        {
            // If the implicit animation API is not present, simply no-op. 
            if (!(ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))) return;

            if ((BrushPresenter.Background as SolidColorBrush).Color == Windows.UI.Colors.Blue)
            {
                BrushPresenter.Background = new SolidColorBrush(Windows.UI.Colors.Yellow);
            }
            else
            {
                BrushPresenter.Background = new SolidColorBrush(Windows.UI.Colors.Blue);
            }
        }

        private void ThemeButton_Click(object sender, RoutedEventArgs e)
        {
            ThemeExampleGrid.RequestedTheme = ThemeExampleGrid.RequestedTheme == ElementTheme.Dark ? ElementTheme.Light : ElementTheme.Dark;
        }
    }
}
