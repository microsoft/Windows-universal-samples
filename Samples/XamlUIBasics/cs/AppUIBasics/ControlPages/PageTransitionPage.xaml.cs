using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    public sealed partial class PageTransitionPage : Page
    {
        private NavigationTransitionInfo _transitionInfo = null;

        public PageTransitionPage()
        {
            this.InitializeComponent();

            ContentFrame.Navigate(typeof(SamplePages.SamplePage1));
        }

        private void ForwardButton1_Click(object sender, RoutedEventArgs e)
        {

            var pageToNavigateTo = ContentFrame.BackStackDepth % 2 == 1 ? typeof(SamplePages.SamplePage1) : typeof(SamplePages.SamplePage2);

            if (_transitionInfo == null)
            {
                // Default behavior, no transition set or used.
                ContentFrame.Navigate(pageToNavigateTo, null);
            }
            else
            {
                // Explicit transition info used.
                ContentFrame.Navigate(pageToNavigateTo, null, _transitionInfo);
            }
        }

        private void BackwardButton1_Click(object sender, RoutedEventArgs e)
        {
            if (ContentFrame.BackStackDepth > 0)
            {
                ContentFrame.GoBack();
            }
        }

        private void TransitionRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            var pageTransitionString = "";

            var senderTransitionString = (sender as RadioButton).Content.ToString();
            if (senderTransitionString != "Default")
            {
                pageTransitionString = ", new " + senderTransitionString + "NavigationTransitionInfo()";

                if (senderTransitionString == "Entrance")
                {
                    _transitionInfo = new EntranceNavigationTransitionInfo();
                }
                else if (senderTransitionString == "Drill")
                {
                    _transitionInfo = new DrillInNavigationTransitionInfo();
                }
                else if (senderTransitionString == "Suppress")
                {
                    _transitionInfo = new SuppressNavigationTransitionInfo();
                }
                else if (senderTransitionString == "Slide from Right")
                {
                    _transitionInfo = new SlideNavigationTransitionInfo() { Effect = SlideNavigationTransitionEffect.FromRight };
                    pageTransitionString = ", new SlideNavigationTransitionInfo() { Effect = SlideNavigationTransitionEffect.FromRight }";
                }
                else if (senderTransitionString == "Slide from Left")
                {
                    _transitionInfo = new SlideNavigationTransitionInfo() { Effect = SlideNavigationTransitionEffect.FromLeft };
                    pageTransitionString = ", new SlideNavigationTransitionInfo() { Effect = SlideNavigationTransitionEffect.FromLeft }";
                }
            }
            else
            {
                _transitionInfo = null;
            }

            if (transitionRun != null)
                transitionRun.Text = pageTransitionString;
        }
    }
}
