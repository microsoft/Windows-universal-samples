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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.StartScreen;

namespace SDKTemplate
{
    public sealed partial class Scenario3_CustomItems : Page
    {
        public Scenario3_CustomItems()
        {
            this.InitializeComponent();
        }

        private void DisplayNameLocalizedResource_Click(object sender, RoutedEventArgs e)
        {
            DisplayName.IsEnabled = !(bool)DisplayNameLocalizedResource.IsChecked;
            DisplayName.Text = "ms-resource:///Resources/CustomJumpListItemDisplayName";
        }

        private void DescriptionLocalizedResource_Click(object sender, RoutedEventArgs e)
        {
            Description.IsEnabled = !(bool)DescriptionLocalizedResource.IsChecked;
            Description.Text = "ms-resource:///Resources/CustomJumpListItemDescription";
        }

        private void GroupNameLocalizedResource_Click(object sender, RoutedEventArgs e)
        {
            GroupName.IsEnabled = !(bool)GroupNameLocalizedResource.IsChecked;
            GroupName.Text = "ms-resource:///Resources/CustomJumpListGroupName";
        }

        private async void AddToJumpList_Click(object sender, RoutedEventArgs e)
        {
            var jumpList = await JumpList.LoadCurrentAsync();

            var item = JumpListItem.CreateWithArguments(Arguments.Text, DisplayName.Text);
            item.Description = Description.Text;
            item.GroupName = GroupName.Text;
            item.Logo = new Uri("ms-appx:///Assets/smalltile-sdk.png");

            jumpList.Items.Add(item);

            await jumpList.SaveAsync();
        }
    }
}
