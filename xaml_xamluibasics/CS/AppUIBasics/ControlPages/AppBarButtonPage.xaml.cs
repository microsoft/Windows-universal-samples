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
    public sealed partial class AppBarButtonPage : Page
    {
        AppBarToggleButton compactButton = null;

        public AppBarButtonPage()
        {
            this.InitializeComponent();
            Loaded += AppBarButtonPage_Loaded;
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            // Remove compact button from the command bar.
            base.OnNavigatingFrom(e);
            Frame rootFrame = Window.Current.Content as Frame;
            ItemPage rootPage = rootFrame.Content as ItemPage;

            if (rootPage != null)
            {
                CommandBar appBar = rootPage.BottomCommandBar;
                compactButton.Click -= CompactButton_Click;
                appBar.PrimaryCommands.Remove(compactButton);
            }
        }

        void AppBarButtonPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Add compact button to the command bar. It provides functionality specific
            // to this page, and is removed when leaving the page.
            ItemPage itemPage = NavigationRootPage.RootFrame.Content as ItemPage;

            if (itemPage != null)
            {
                CommandBar appBar = itemPage.BottomCommandBar;

                appBar.PrimaryCommands.Insert(0, new AppBarSeparator());

                compactButton = new AppBarToggleButton();
                compactButton.Icon = new SymbolIcon(Symbol.FontSize);
                compactButton.Label = "IsCompact";
                compactButton.Click += CompactButton_Click;
                appBar.PrimaryCommands.Insert(0, compactButton);
            }
        }
        
        private void CompactButton_Click(object sender, RoutedEventArgs e)
        {
            ToggleButton toggle = sender as ToggleButton;
            if (toggle != null && toggle.IsChecked != null)
            {
                Button1.IsCompact =
                Button2.IsCompact =
                Button3.IsCompact =
                Button4.IsCompact = (bool)toggle.IsChecked;
            }
        }

        private void AppBarButton_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;

            if (b != null)
            {
                string name = b.Name;

                switch (name)
                {
                    case "Button1":
                        Control1Output.Text = "You clicked: " + name;
                        break;
                    case "Button2":
                        Control2Output.Text = "You clicked: " + name;
                        break;
                    case "Button3":
                        Control3Output.Text = "You clicked: " + name;
                        break;
                    case "Button4":
                        Control4Output.Text = "You clicked: " + name;
                        break;
                }
            }
        }
    }
}
