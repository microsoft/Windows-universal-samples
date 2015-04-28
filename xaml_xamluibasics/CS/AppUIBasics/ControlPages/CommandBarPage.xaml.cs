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
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class CommandBarPage : Page
    {
        public CommandBarPage()
        {
            this.InitializeComponent();
        }

        private void OpenButton_Click(object sender, RoutedEventArgs e)
        {
            ItemPage itemPage = NavigationRootPage.RootFrame.Content as ItemPage;

            if (itemPage != null)
            {
                CommandBar appBar = itemPage.BottomCommandBar;

                appBar.IsOpen = true;
                appBar.IsSticky = true;
            }
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            ItemPage itemPage = NavigationRootPage.RootFrame.Content as ItemPage;

            if (itemPage != null)
            {
                CommandBar commandBar = itemPage.BottomCommandBar;

                commandBar.IsOpen = false;
            }
        }

        private void AddSecondaryCommands_Click(object sender, RoutedEventArgs e)
        {
            // Add compact button to the command bar. It provides functionality specific
            // to this page, and is removed when leaving the page.
            ItemPage itemPage = NavigationRootPage.RootFrame.Content as ItemPage;

            if (itemPage != null)
            {
                CommandBar commandBar = itemPage.BottomCommandBar;

                if (commandBar.SecondaryCommands.Count == 1)
                {
                    var newButton = new AppBarButton();
                    newButton.Icon = new SymbolIcon(Symbol.Add);
                    newButton.Label = "Button 1";
                    commandBar.SecondaryCommands.Add(newButton);

                    newButton = new AppBarButton();
                    newButton.Icon = new SymbolIcon(Symbol.Delete);
                    newButton.Label = "Button 2";
                    commandBar.SecondaryCommands.Add(newButton);

                    commandBar.SecondaryCommands.Add(new AppBarSeparator());

                    newButton = new AppBarButton();
                    newButton.Icon = new SymbolIcon(Symbol.FontDecrease);
                    newButton.Label = "Button 3";
                    commandBar.SecondaryCommands.Add(newButton);

                    newButton = new AppBarButton();
                    newButton.Icon = new SymbolIcon(Symbol.FontIncrease);
                    newButton.Label = "Button 4";
                    commandBar.SecondaryCommands.Add(newButton);
                }
            }
        }

        private void RemoveSecondaryCommands_Click(object sender, RoutedEventArgs e)
        {
            RemoveSecondaryCommands();
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            RemoveSecondaryCommands();
            base.OnNavigatingFrom(e);
        }

        private void RemoveSecondaryCommands()
        {
            ItemPage itemPage = NavigationRootPage.RootFrame.Content as ItemPage;

            if (itemPage != null)
            {
                CommandBar commandBar = itemPage.BottomCommandBar;

                while (commandBar.SecondaryCommands.Count > 1)
                {
                    commandBar.SecondaryCommands.RemoveAt(commandBar.SecondaryCommands.Count - 1);
                }
            }
        }
    }
}
