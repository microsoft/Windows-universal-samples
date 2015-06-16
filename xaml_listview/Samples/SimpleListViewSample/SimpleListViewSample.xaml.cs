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

using ListViewSample.Model;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ListViewSample
{
    public sealed partial class SimpleListViewSample : Page
    {
        public SimpleListViewSample()
        {
            this.InitializeComponent();
            ContactsCVS.Source = Contact.GetContactsGrouped(250);
        }
        private void ShowSliptView(object sender, RoutedEventArgs e)
        {
            MySamplesPane.SamplesSplitView.IsPaneOpen = !MySamplesPane.SamplesSplitView.IsPaneOpen;
        }
    }
}
