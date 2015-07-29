using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Transform3DAnimations.ViewModels;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace Transform3DAnimations
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private Random _random;
        private DispatcherTimer _updateTimer;


        public MainPage()
        {
            this.InitializeComponent();

            _random = new Random();

            _updateTimer = new DispatcherTimer();
            _updateTimer.Interval = TimeSpan.FromSeconds(3.0);
            _updateTimer.Tick += UpdateTimer_Tick;
        }

        private void UpdateRandomSection()
        {
            // Updating the section triggers a cool animation!
            // See SectionView.xaml and SectionView.xaml.cs

            var sectionsInView = HeadlinesHub.SectionsInView;
            var sectionsCount = sectionsInView.Count;

            if (sectionsCount > 0)
            {
                var sectionToUpdate = sectionsInView[_random.Next(sectionsCount)];
                sectionToUpdate.DataContext = new HeroArticlesViewModel();
            }
        }

        private void UpdateTimer_Tick(object sender, object e)
        {
            UpdateRandomSection();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            _updateTimer.Start();
        }

        private void SectionView_ArticleSelected(object sender, ArticleViewModel e)
        {
            // In a real app, you might pass a unique ID for the article as a navigation parameter.
            // Here we pass the title.
            Frame.Navigate(typeof(ArticlePage), e.Headline);
        }
    }
}
