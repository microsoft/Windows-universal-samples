//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using AppUIBasics.Common;
using AppUIBasics.Data;
using System.Collections.Generic;
using System.Linq;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class FlipViewPage : Page
    {
        private List<ControlInfoDataItem> _items;

        public FlipViewPage()
        {
            this.InitializeComponent();
        }
        public List<ControlInfoDataItem> Items
        {
            get { return this._items; }
        }

        protected async override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            var groups = NavigationRootPage.Current.Groups.Any() ? NavigationRootPage.Current.Groups : await ControlInfoDataSource.GetGroupsAsync();
            _items = new List<ControlInfoDataItem>();
            foreach (var group in groups.Take(3))
            {
                foreach (var item in group.Items)
                {
                    _items.Add(item);
                }
            }
        }
    }
}
