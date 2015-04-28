//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class AppBarPage : Page
    {
        public AppBarPage()
        {
            this.InitializeComponent();
        }

        private void topAppBar_Opened(object sender, object e)
        {
            ItemPage itemPage = NavigationRootPage.RootFrame.Content as ItemPage;

            if (itemPage != null)
            {
                CommandBar bottomAppBar = itemPage.BottomCommandBar;
                bottomAppBar.IsOpen = false;
            }
        }

        private void OpenButton_Click(object sender, RoutedEventArgs e)
        {
            this.TopAppBar.IsOpen = true;
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            this.TopAppBar.IsOpen = false;
        }

        private void NavBarButton_Click(object sender, RoutedEventArgs e)
        {
            ButtonBase b = (ButtonBase)sender;

            Frame rootFrame = Window.Current.Content as Frame;
            if (rootFrame != null && b.Tag != null)
            {
                if (b.Tag.ToString() == "Home")
                {
                    rootFrame.Navigate(typeof(AppUIBasics.MainPage));
                }
                else
                {
                    rootFrame.Navigate(typeof(SectionPage), b.Tag.ToString());
                }
            }
        }

        private void AddButton_Click(object sender, RoutedEventArgs e)
        {
            Button homeButton = AppBarContentPanel.Children[0] as Button;

            if (homeButton != null && homeButton.Tag.ToString() != "Home")
            {
                homeButton = new Button();
                homeButton.Content = "Home";
                homeButton.Tag = "Home";
                homeButton.Click += NavBarButton_Click;

                AppBarContentPanel.Children.Insert(0, homeButton);
            }
        }

        private void RemoveButton_Click(object sender, RoutedEventArgs e)
        {
            RemoveHomeButton();
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            RemoveHomeButton();
            base.OnNavigatingFrom(e);
        }

        private void RemoveHomeButton()
        {
            Button homeButton = AppBarContentPanel.Children[0] as Button;

            if (homeButton != null && homeButton.Tag.ToString() == "Home")
            {
                homeButton.Click -= NavBarButton_Click;
                AppBarContentPanel.Children.RemoveAt(0);
            }
        }
    }
}
