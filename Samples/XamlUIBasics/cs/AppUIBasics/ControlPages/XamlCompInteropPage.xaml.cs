using System;
using System.Numerics;
using Windows.Foundation.Metadata;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace AppUIBasics.ControlPages
{
    public sealed partial class XamlCompInteropPage : Page
    {
        public XamlCompInteropPage()
        {
            this.InitializeComponent();
        }

        Compositor _compositor = Window.Current.Compositor;
        private SpringVector3NaturalMotionAnimation _springAnimation;

        private void NaturalMotionExample_Loaded(object sender, RoutedEventArgs e)
        {
            UpdateSpringAnimation(1.0f);
        }

        private void UpdateSpringAnimation(float finalValue)
        {
            if (_springAnimation == null)
            {
                _springAnimation = _compositor.CreateSpringVector3Animation();
                _springAnimation.Target = "Scale";
            }

            _springAnimation.FinalValue = new Vector3(finalValue);
            _springAnimation.DampingRatio = GetDampingRatio();
            _springAnimation.Period = GetPeriod();
        }

        float GetDampingRatio()
        {
            foreach (RadioButton rb in DampingStackPanel.Children)
            {
                if (rb.IsChecked == true)
                {
                    return (float)Convert.ToDouble(rb.Content);
                }
            }
            return 0.6f;
        }

        TimeSpan GetPeriod()
        {
            return TimeSpan.FromMilliseconds(PeriodSlider.Value);
        }

        private void StartAnimationIfAPIPresent(UIElement sender, Windows.UI.Composition.CompositionAnimation animation)
        {
            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))
            {
                (sender as UIElement).StartAnimation(animation);
            }
        }

        private void element_PointerEntered(object sender, PointerRoutedEventArgs e)
        {
            UpdateSpringAnimation(1.5f);

            StartAnimationIfAPIPresent((sender as UIElement), _springAnimation);
        }

        private void element_PointerExited(object sender, PointerRoutedEventArgs e)
        {
            UpdateSpringAnimation(1f);

            StartAnimationIfAPIPresent((sender as UIElement), _springAnimation);
        }


        private void ExpressionSample_Loaded(object sender, RoutedEventArgs e)
        {
            var anim = _compositor.CreateExpressionAnimation();
            anim.Expression = "Vector3(1/scaleElement.Scale.X, 1/scaleElement.Scale.Y, 1)";
            anim.Target = "Scale";

            // Only establish the reference parameter if the API exists to do so.
            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))
            {
                anim.SetExpressionReferenceParameter("scaleElement", rectangle);
            }

            StartAnimationIfAPIPresent(ellipse, anim);
        }

        private void StackedButtonsExample_Loaded(object sender, RoutedEventArgs e)
        {
            // Only establish the reference parameter if the API exists to do so.
            if (!(ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))) return;

            var anim = _compositor.CreateExpressionAnimation();
            anim.Expression = "(above.Scale.Y - 1) * 50 + above.Translation.Y % (50 * index)";
            anim.Target = "Translation.Y";

            anim.SetExpressionReferenceParameter("above", ExpressionButton1);
            anim.SetScalarParameter("index", 1);
            ExpressionButton2.StartAnimation(anim);

            anim.SetExpressionReferenceParameter("above", ExpressionButton2);
            anim.SetScalarParameter("index", 2);
            ExpressionButton3.StartAnimation(anim);

            anim.SetExpressionReferenceParameter("above", ExpressionButton3);
            anim.SetScalarParameter("index", 3);
            ExpressionButton4.StartAnimation(anim);
        }

    }
}
