//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate;
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
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace ContextMenuCS
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;
        private bool _IsShiftPressed = false;
        private bool _IsPointerPressed = false;
        public List<SampleDataModel> AllItems = null;

        public Scenario1()
        {
            this.InitializeComponent();
            AllItems = SampleDataModel.GetSampleData().ToList();
            this.DataContext = this;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }


        protected override void OnKeyDown(KeyRoutedEventArgs e)
        {
            // Handle Shift+F10
            // Handle MenuKey

            if (e.Key == Windows.System.VirtualKey.Shift)
            {
                _IsShiftPressed = true;
            }

            // Shift+F10
            else if (_IsShiftPressed && e.Key == Windows.System.VirtualKey.F10)
            {
                var FocusedElement = FocusManager.GetFocusedElement() as UIElement;

                SampleDataModel MyObject = null;
                if (FocusedElement is ContentControl)
                {
                    MyObject = ((ContentControl)FocusedElement).Content as SampleDataModel;
                }
                ShowContextMenu(MyObject, FocusedElement, new Point(0, 0));
                e.Handled = true;
            }

            // The 'Menu' key next to Right Ctrl on most keyboards
            else if (e.Key == Windows.System.VirtualKey.Application || e.Key == Windows.System.VirtualKey.GamepadMenu)
            {
                var FocusedElement = FocusManager.GetFocusedElement() as UIElement;
                SampleDataModel MyObject = null;
                if (FocusedElement is ContentControl)
                {
                    MyObject = ((ContentControl)FocusedElement).Content as SampleDataModel;
                }
                ShowContextMenu(MyObject, FocusedElement, new Point(0, 0));
                e.Handled = true;
            }

            base.OnKeyDown(e);
        }

        protected override void OnKeyUp(KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Shift)
            {
                _IsShiftPressed = false;
            }

            base.OnKeyUp(e);
        }
        protected override void OnHolding(HoldingRoutedEventArgs e)
        {
            // Responding to HoldingState.Started will show a context menu while your finger is still down, while 
            // HoldingState.Completed will wait until the user has removed their finger. 
            if (e.HoldingState == Windows.UI.Input.HoldingState.Completed)
            {
                var PointerPosition = e.GetPosition(null);

                var MyObject = (e.OriginalSource as FrameworkElement).DataContext as SampleDataModel;
                ShowContextMenu(MyObject, null, PointerPosition);
                e.Handled = true;

                // This, combined with a check in OnRightTapped prevents the firing of RightTapped from
                // launching another context menu
                _IsPointerPressed = false;

                // This prevents any scrollviewers from continuing to pan once the context menu is displayed.  
                // Ideally, you should find the ListViewItem itself and only CancelDirectMinpulations on that item.  
                var ItemsToCancel = VisualTreeHelper.FindElementsInHostCoordinates(PointerPosition, ItemListView);
                foreach (var Item in ItemsToCancel)
                {
                    var Result = Item.CancelDirectManipulations();
                }
            }

            base.OnHolding(e);
        }

        protected override void OnPointerPressed(PointerRoutedEventArgs e)
        {
            _IsPointerPressed = true;

            base.OnPointerPressed(e);
        }

        protected override void OnRightTapped(RightTappedRoutedEventArgs e)
        {
            if (_IsPointerPressed)
            {
                var MyObject = (e.OriginalSource as FrameworkElement).DataContext as SampleDataModel;

                ShowContextMenu(MyObject, null, e.GetPosition(null));
                e.Handled = true;
            }

            base.OnRightTapped(e);
        }

        private void ShowContextMenu(SampleDataModel data, UIElement target, Point offset)
        {
            var MyFlyout = this.Resources["SampleContextMenu"] as MenuFlyout;

            System.Diagnostics.Debug.WriteLine("MenuFlyout shown '{0}', '{1}'", target, offset);

            MyFlyout.ShowAt(target, offset);
        }
    }
}
