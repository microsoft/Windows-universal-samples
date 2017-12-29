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
    public sealed partial class AppBarSeparatorPage : Page
    {
        AppBarToggleButton compactButton = null;
        AppBarSeparator separator = null;

        public AppBarSeparatorPage()
        {
            this.InitializeComponent();
            Loaded += AppBarButtonPage_Loaded;
            Unloaded += AppBarSeparatorPage_Unloaded;
        }

        private void AppBarSeparatorPage_Unloaded(object sender, RoutedEventArgs e)
        {
            CommandBar appBar = NavigationRootPage.Current.PageHeader.TopCommandBar;
            compactButton.Click -= CompactButton_Click;
            appBar.PrimaryCommands.Remove(compactButton);
            appBar.PrimaryCommands.Remove(separator);
        }

        void AppBarButtonPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Add compact button to the command bar. It provides functionality specific
            // to this page, and is removed when leaving the page.

                CommandBar appBar = NavigationRootPage.Current.PageHeader.TopCommandBar;
                separator = new AppBarSeparator();
                appBar.PrimaryCommands.Insert(0, separator);

                compactButton = new AppBarToggleButton();
                compactButton.Icon = new SymbolIcon(Symbol.FontSize);
                compactButton.Label = "IsCompact";
                compactButton.Click += CompactButton_Click;
                appBar.PrimaryCommands.Insert(0, compactButton);
        }

        private void CompactButton_Click(object sender, RoutedEventArgs e)
        {
            // NOTE: If you use AppBarButton and AppBarSeparator controls in a CommandBar,
            // the CommandBar sets the IsCompact property automatically. You only set it
            // yourself if the control in not in a CommandBar.

            ToggleButton toggle = sender as ToggleButton;
            if (toggle != null && toggle.IsChecked != null)
            {
                foreach (ICommandBarElement element in Control1.Children)
                {
                    element.IsCompact = (bool)toggle.IsChecked;
                }
            }
        }
    }
}
