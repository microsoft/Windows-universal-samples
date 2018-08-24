//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using System.Collections.ObjectModel;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace AppUIBasics.ControlPages
{
    public sealed partial class SwipeControlPage : Page
    {
        private bool isArchived = false;

        private bool isFlagged = false;
        private bool isAccepted = false;

        public SwipeControlPage()
        {
            this.InitializeComponent();
            var source = @"Swipe Item 1,Swipe Item 2,Swipe Item 3,Swipe Item 4".Split(',');
            foreach (var item in source)
                items.Add(item);
            lv.ItemsSource = items;
        }

        ObservableCollection<object> items = new ObservableCollection<object>();

        private void DeleteOne_ItemInvoked(SwipeItem sender, SwipeItemInvokedEventArgs args)
        {
            isArchived = !isArchived;

            if(isArchived)
            {
                ((TextBlock)args.SwipeControl.Content).Text = "Archived - Swipe Left";
            }
            else
            {
                ((TextBlock)args.SwipeControl.Content).Text = "Swipe Left";
            }
        }

        private void DeleteItem_ItemInvoked(SwipeItem sender, SwipeItemInvokedEventArgs args)
        {
            items.Remove(args.SwipeControl.DataContext);
        }

        private void Accept_ItemInvoked(SwipeItem sender, SwipeItemInvokedEventArgs args)
        {
            isAccepted = !isAccepted;
            CheckAcceptFlagBool(args.SwipeControl);

            if (isAccepted)
            {
                FontIconSource cancelIcon = new FontIconSource() { Glyph = "\ue711" };
                sender.IconSource = cancelIcon;
                sender.Text = "Cancel";
            }
            else
            {
                FontIconSource acceptIcon = new FontIconSource() { Glyph = "\ue10B" };
                sender.IconSource = acceptIcon;
                sender.Text = "Accept";
            }
        }

        private void Flag_ItemInvoked(SwipeItem sender, SwipeItemInvokedEventArgs args)
        {
            isFlagged = !isFlagged;
            CheckAcceptFlagBool(args.SwipeControl);

            if (isFlagged)
            {
                FontIconSource filledFlagIcon = new FontIconSource() { Glyph = "\ueB4B" };
                sender.IconSource = filledFlagIcon;
                sender.Text = "Unmark";
            }
            else
            {
                FontIconSource flagIcon = new FontIconSource() { Glyph = "\ue129" };
                sender.IconSource = flagIcon;
                sender.Text = "Flag";
            }
        }

        private void CheckAcceptFlagBool(SwipeControl swipeCtrl)
        {
            if(isAccepted && !isFlagged)
            {
                ((TextBlock)swipeCtrl.Content).Text = "Swipe Right - Accepted";
            }
            else if (isAccepted && isFlagged)
            {
                ((TextBlock)swipeCtrl.Content).Text = "Swipe Right - Accepted & Flagged";
            }
            else if (!isAccepted && isFlagged)
            {
                ((TextBlock)swipeCtrl.Content).Text = "Swipe Right - Flagged";
            }
            else
            {
                ((TextBlock)swipeCtrl.Content).Text = "Swipe Right";
            }
        }
    }
}
