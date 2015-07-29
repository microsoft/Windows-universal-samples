using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Transform3DAnimations.ViewModels;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace Transform3DAnimations
{
    public sealed partial class SectionView : UserControl
    {
        private UISettings _uiSettings;

        // We will use the property changed handler to create a custom transition.
        // See SectionView.xaml and SectionView.OnViewModelChanged.
        private static DependencyProperty s_viewModelProperty
            = DependencyProperty.Register(
                "ViewModel",
                typeof(HeroArticlesViewModel),
                typeof(SectionView),
                new PropertyMetadata(null, new PropertyChangedCallback(OnViewModelPropertyChanged))
                );

        public static DependencyProperty ViewModelProperty
        {
            get { return s_viewModelProperty; }
        }

        public HeroArticlesViewModel ViewModel
        {
            get { return (HeroArticlesViewModel)GetValue(s_viewModelProperty); }
            set { SetValue(s_viewModelProperty, value); }
        }

        public SectionView()
        {
            this.InitializeComponent();

            VisualStateManager.GoToState(this, "ContentSteadyState", false);
            _uiSettings = new UISettings();
        }


        private void UpdateForSizeChanged(double newWidth, double newHeight)
        {
            // The rotation creates a rectangular prism effect.
            // The center of rotation should be at X = Width / 2 and Z = -Width / 2
            var centerX = newWidth / 2.0;
            RootTransform.CenterX = centerX;
            NextContentTransform.CenterX = centerX;

            var centerZ = -newWidth / 2.0;
            RootTransform.CenterZ = centerZ;
            NextContentTransform.CenterZ = centerZ;

            // Clip to our left and right bounds so the effect doesn't get too crazy.
            ClipGeometry.Rect = new Rect(0.0, -1024.0, newWidth, newHeight + 1024.0);
        }

        private void LayoutRoot_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            var newSize = e.NewSize;
            UpdateForSizeChanged(newSize.Width, newSize.Height);
        }

        private void ContentTransitionStoryboard_Completed(object sender, object e)
        {
            CurrentContentPresenter.Content = NextContentPresenter.Content;
            VisualStateManager.GoToState(this, "ContentSteadyState", false);
        }

        private void OnViewModelChanged(HeroArticlesViewModel oldValue, HeroArticlesViewModel newValue)
        {
            if (oldValue != null && _uiSettings.AnimationsEnabled)
            {
                NextContentPresenter.Content = newValue;
                CurrentContentPresenter.Content = oldValue;

                // Play content transition
                VisualStateManager.GoToState(this, "ContentTransitionState", true);
            }
            else
            {
                CurrentContentPresenter.Content = newValue;
            }
        }

        private static void OnViewModelPropertyChanged(DependencyObject o, DependencyPropertyChangedEventArgs e)
        {
            ((SectionView)o).OnViewModelChanged((HeroArticlesViewModel)e.OldValue, (HeroArticlesViewModel)e.NewValue);
        }

        private void ArticleTapped(object sender, TappedRoutedEventArgs e)
        {
            if (ArticleSelected != null)
            {
                ArticleSelected(this, (ArticleViewModel)((FrameworkElement)e.OriginalSource).DataContext);
            }
        }

        public event TypedEventHandler<object, ArticleViewModel> ArticleSelected;
    }
}
