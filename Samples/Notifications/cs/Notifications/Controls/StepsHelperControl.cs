using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace Notifications.Controls
{
    public class StepsHelperControl : Panel
    {
        protected override Size MeasureOverride(Size availableSize)
        {
            Size resultSize = new Size(availableSize.Width, 0);

            foreach (UIElement child in Children)
            {
                child.Measure(availableSize);

                if (child.DesiredSize.Height > resultSize.Height)
                    resultSize.Height = child.DesiredSize.Height;
            }

            //return availableSize;
            return resultSize;
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            //Clip = new Windows.UI.Xaml.Media.RectangleGeometry() { Rect = new Rect(new Point(), finalSize) };

            for (int i = 0; i < Children.Count; i++)
            {
                Children[i].Arrange(new Rect(0, 0, finalSize.Width, finalSize.Height));
            }

            UpdateVisibilities();

            return finalSize;
        }

        public static readonly DependencyProperty StepProperty = DependencyProperty.Register("Step", typeof(int), typeof(StepsHelperControl), new PropertyMetadata(1, OnStepChanged));

        private static void OnStepChanged(object sender, DependencyPropertyChangedEventArgs args)
        {
            (sender as StepsHelperControl).OnStepChanged(args);
        }

        private void OnStepChanged(DependencyPropertyChangedEventArgs args)
        {
            UpdateVisibilities();
        }

        private void UpdateVisibilities()
        {
            int adaptedStep = Step - 1;

            for (int i = 0; i < Children.Count; i++)
            {
                Children[i].Visibility = (i == adaptedStep) ? Visibility.Visible : Visibility.Collapsed;
            }
        }

        public int Step
        {
            get { return (int)GetValue(StepProperty); }
            set { SetValue(StepProperty, value); }
        }
    }
}
