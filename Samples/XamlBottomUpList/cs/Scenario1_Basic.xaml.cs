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

using System;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Basic : Page
    {
        private int messageNumber = 0;
        private string savedScrollPosition;

        public Scenario1_Basic()
        {
            this.InitializeComponent();

            // Start with one message, so the screen doesn't look empty.
            AddItemToEnd();
        }

        private void AddItemToEnd()
        {
            BottomUpList.Items.Add($"{DateTime.Now}: Message {++messageNumber}");
        }

        private void DeleteSelectedItem()
        {
            int selectedIndex = BottomUpList.SelectedIndex;
            BottomUpList.Items.Remove(BottomUpList.SelectedItem);
            if (selectedIndex < BottomUpList.Items.Count)
            {
                BottomUpList.SelectedIndex = selectedIndex;
            }
        }

        private void SaveScrollPosition()
        {
            // We let the item be its own key. In a more general program, the item would have
            // a unique ID that identifies it in the list.
            this.savedScrollPosition = ListViewPersistenceHelper.GetRelativeScrollPosition(BottomUpList, (o) => o.ToString());
        }

        private async void RestoreScrollPosition()
        {
            if (this.savedScrollPosition != null)
            {
                await ListViewPersistenceHelper.SetRelativeScrollPositionAsync(BottomUpList, this.savedScrollPosition, (s) => Task.FromResult((object)s).AsAsyncOperation());
            }
        }
    }
}
