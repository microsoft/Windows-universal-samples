//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using AppUIBasics.Data;
using System.Linq;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace AppUIBasics
{
    /// <summary>
    /// A page that displays an overview of a single group, including a preview of the items
    /// within the group.
    /// </summary>
    public sealed partial class SectionPage : ItemsPageBase
    {
        public SectionPage()
        {
            this.InitializeComponent();
        }
        
        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            var group = await ControlInfoDataSource.Instance.GetGroupAsync((string)e.Parameter);

            var menuItem = NavigationRootPage.Current.NavigationView.MenuItems.Cast<NavigationViewItem>().Single(i => (string)i.Tag == group.UniqueId);
            menuItem.IsSelected = true;
            NavigationRootPage.Current.NavigationView.Header = menuItem.Content;
            
            Items = group.Items.OrderBy(i => i.Title).ToList();
        }
        
        protected override bool GetIsNarrowLayoutState()
        {
            return LayoutVisualStates.CurrentState == NarrowLayout;
        }
    }
}