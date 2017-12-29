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
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media.Animation;

namespace AppUIBasics
{
    public abstract class ItemsPageBase : Page, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private string _itemId;
        private IEnumerable<ControlInfoDataItem> _items;
                
        public IEnumerable<ControlInfoDataItem> Items
        {
            get { return _items; }
            set { SetProperty(ref _items, value); }
        }

        /// <summary>
        /// Gets a value indicating whether the application's view is currently in "narrow" mode - i.e. on a mobile-ish device.
        /// </summary>
        protected virtual bool GetIsNarrowLayoutState()
        {
            throw new NotImplementedException();
        }

        protected void OnItemGridViewContainerContentChanging(ListViewBase sender, ContainerContentChangingEventArgs args)
        {
            if (sender.ContainerFromItem(sender.Items.LastOrDefault()) is GridViewItem container)
            {
                container.XYFocusDown = container;
            }
        }

        protected void OnItemGridViewItemClick(object sender, ItemClickEventArgs e)
        {
            var gridView = (GridView)sender;
            var item = (ControlInfoDataItem)e.ClickedItem;

            _itemId = item.UniqueId;

            if (gridView.ContainerFromItem(item) is GridViewItem)
            {
                gridView.PrepareConnectedAnimation("controlAnimation", item, "controlRoot");
            }

            this.Frame.Navigate(typeof(ItemPage), _itemId);
        }

        protected void OnItemGridViewKeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Up)
            {
                var nextElement = FocusManager.FindNextElement(FocusNavigationDirection.Up);
                if (nextElement.GetType() == typeof(NavigationViewItem))
                {
                    NavigationRootPage.Current.PageHeader.Focus(FocusState.Programmatic);
                }
                else
                {
                    FocusManager.TryMoveFocus(FocusNavigationDirection.Up);
                }
            }
        }

        protected async void OnItemGridViewLoaded(object sender, RoutedEventArgs e)
        {
            if (_itemId != null)
            {
                var gridView = (GridView)sender;
                var items = gridView.ItemsSource as IEnumerable<ControlInfoDataItem>;
                var item = items?.FirstOrDefault(s => s.UniqueId == _itemId);
                if (item != null)
                {
                    gridView.ScrollIntoView(item);

                    if (NavigationRootPage.Current.IsFocusSupported)
                    {
                        ((GridViewItem)gridView.ContainerFromItem(item))?.Focus(FocusState.Programmatic);
                    }

                    ConnectedAnimation animation = ConnectedAnimationService.GetForCurrentView().GetAnimation("controlAnimation");

                    if (animation != null)
                    {
                        await gridView.TryStartConnectedAnimationAsync(animation, item, "controlRoot");
                    }
                }
            }
        }
        
        protected void OnItemGridViewSizeChanged(object sender, SizeChangedEventArgs e)
        {
            var gridView = (GridView)sender;

            if (gridView.ItemsPanelRoot is ItemsWrapGrid wrapGrid)
            {
                if (GetIsNarrowLayoutState())
                {
                    wrapGrid.ItemWidth = gridView.ActualWidth - gridView.Padding.Left - gridView.Padding.Right;
                }
                else
                {
                    wrapGrid.ItemWidth = double.NaN;
                }
            }
        }
        
        protected bool SetProperty<T>(ref T storage, T value, [CallerMemberName] String propertyName = null)
        {
            if (Equals(storage, value)) return false;

            storage = value;
            NotifyPropertyChanged(propertyName);
            return true;
        }

        protected void NotifyPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}