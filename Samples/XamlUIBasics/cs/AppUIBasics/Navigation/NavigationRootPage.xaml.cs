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
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class NavigationRootPage : Page
    {
        public static NavigationRootPage Current;
        public static Frame RootFrame = null;

        private IEnumerable<ControlInfoDataGroup> _groups;
        public IEnumerable<ControlInfoDataGroup> Groups
        {
            get { return _groups; }
        }

        public event EventHandler GroupsLoaded;

        public static SplitView RootSplitView
        {
            get { return Current.rootSplitView; }
        }

        private RootFrameNavigationHelper rootFrameNavigationHelper;

        public NavigationRootPage()
        {
            this.InitializeComponent();
            this.rootFrameNavigationHelper = new RootFrameNavigationHelper(rootFrame);
            LoadGroups();
            Current = this;
            RootFrame = rootFrame;

            this.GotFocus += (object sender, RoutedEventArgs e) =>
            {
                // helpful for debugging focus problems w/ keyboard & gamepad
                FrameworkElement focus = FocusManager.GetFocusedElement() as FrameworkElement;
                if (focus != null)
                    Debug.WriteLine("got focus: " + focus.Name + " (" + focus.GetType().ToString() + ")");
            };
        }

        private async void LoadGroups()
        {
            _groups = await ControlInfoDataSource.GetGroupsAsync();
            if (GroupsLoaded != null)
                GroupsLoaded(this, new EventArgs());
        }

        private void ControlGroupItems_ItemClick(object sender, ItemClickEventArgs e)
        {
            foreach (var item in groupsItemControl.Items)
            {
                var container = groupsItemControl.ContainerFromItem(item);
                var panel = VisualTreeHelper.GetChild(container, 0) as StackPanel;
                if (panel != null)
                {
                    var button = panel.Children[0] as ToggleButton;
                    if (button.IsChecked.Value)
                    {
                        var finished = VisualStateManager.GoToState(button, "Normal", true);
                        button.IsChecked = false;
                    }
                }
            }
            this.rootFrame.Navigate(typeof(ItemPage), (e.ClickedItem as ControlInfoDataItem).UniqueId);
            rootSplitView.IsPaneOpen = false;
        }

        private void homeButton_Click(object sender, RoutedEventArgs e)
        {
            this.rootFrame.Navigate(typeof(MainPage));
            rootSplitView.IsPaneOpen = false;
        }
    }
}
