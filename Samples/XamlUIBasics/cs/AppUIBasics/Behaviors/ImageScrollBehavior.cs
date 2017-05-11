using Microsoft.Xaml.Interactivity;
using System.Linq;
using Windows.ApplicationModel.Core;
using Windows.UI;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace AppUIBasics.Behaviors
{
    public class ImageScrollBehavior : DependencyObject, IBehavior
    {
        private const int _opacityMaxValue = 250;
        private const int _alpha = 255;
        private const int _maxFontSize = 42;
        private const int _minFontSize = 24;
        private const int scrollViewerThresholdValue = 250;
        private ScrollViewer scrollViewer;
        private GridView gridView;

        public DependencyObject AssociatedObject { get; private set; }

        public Control TargetControl
        {
            get { return (Control)GetValue(TargetControlProperty); }
            set { SetValue(TargetControlProperty, value); }
        }

        // Using a DependencyProperty as the backing store for TargetControl.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty TargetControlProperty =
            DependencyProperty.Register("TargetControl", typeof(Control), typeof(ImageScrollBehavior), new PropertyMetadata(null));

        public void Attach(DependencyObject associatedObject)
        {
            AssociatedObject = associatedObject;
            if (!GetScrollViewer())
            {
                ((GridView)associatedObject).Loaded += GridView_Loaded;
            }
        }

        private void GridView_Loaded(object sender, RoutedEventArgs e)
        {
            GetScrollViewer();
            gridView = sender as GridView;
        }

        private bool GetScrollViewer()
        {
            scrollViewer = Common.UIHelper.GetDescendantsOfType<ScrollViewer>(AssociatedObject).FirstOrDefault();
            if (scrollViewer != null)
            {
                scrollViewer.ViewChanging += ScrollViewer_ViewChanging;
                return true;
            }
            return false;
        }

        private void ScrollViewer_ViewChanging(object sender, ScrollViewerViewChangingEventArgs e)
        {
            double verticalOffset = ((ScrollViewer)sender).VerticalOffset;
            var header = (PageHeader)TargetControl;
            header.BackgroundColorOpacity = verticalOffset / _opacityMaxValue;
            header.AcrylicOpacity = 0.3 * (1 - (verticalOffset / _opacityMaxValue));
            if (verticalOffset == 0)
            {
                header.BackgroundColorOpacity = 0;
                header.FontSize = 42;
                header.Foreground = new SolidColorBrush(Colors.White);
                header.AcrylicOpacity = 0.3;
            }
            else if (verticalOffset > scrollViewerThresholdValue)
            {
                TargetControl.FontSize = _minFontSize;
                if (gridView != null)
                {
                    gridView.Margin = Window.Current.Bounds.Width > 640 ? new Thickness(0, 70, 0, 0) : new Thickness(0, 10, 0, 0);
                }
                ApplicationView.GetForCurrentView().TitleBar.ButtonForegroundColor = Colors.Black;
            }
            else
            {
                TargetControl.FontSize = -(((verticalOffset / scrollViewerThresholdValue) * (_maxFontSize - _minFontSize)) - _maxFontSize);
                if (gridView != null)
                {
                    gridView.Margin = new Thickness(0);
                }
                ApplicationView.GetForCurrentView().TitleBar.ButtonForegroundColor = Window.Current.Bounds.Width > 640 ? Colors.White : Colors.Black;
            }

            if (App.Current.RequestedTheme != ApplicationTheme.Dark)
            {
                Color foreground = new Color() { A = _alpha };
                foreground.R = foreground.G = foreground.B = (byte)((verticalOffset > _alpha) ? 0 : (_alpha - verticalOffset));
                TargetControl.Foreground = new SolidColorBrush(foreground);
            }
            else
            {
                ApplicationView.GetForCurrentView().TitleBar.ButtonForegroundColor = Colors.White;
            }
        }

        public void Detach()
        {
            ((GridView)AssociatedObject).Loaded -= GridView_Loaded;
            AssociatedObject = null;
        }
    }
}
