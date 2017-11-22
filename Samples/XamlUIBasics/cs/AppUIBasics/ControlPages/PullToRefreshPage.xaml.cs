//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
//using Microsoft.UI.Preview;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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

// From ControlInfoData.json:
//{
//  "UniqueId": "PullToRefresh",
//  "Title": "Pull to refresh",
//  "Subtitle": "Touch gesture for dragging down to fetch new data.",
//  "ImagePath": "ms-appx:///Assets/PullToRefresh.png",
//  "Description": "Touch gesture for dragging down to fetch new data.",
//  "Content": "",
//  "IsNew": true,
//	"Docs": [
//		{
//			"Title": "RefreshContainer",
//			"Uri": "https://docs.microsoft.com/uwp/api/Windows.UI.Xaml.Controls.RefreshContainer"
//		},
//		{
//			"Title": "RefreshVisualizer",
//			"Uri": "https://docs.microsoft.com/uwp/api/Windows.UI.Xaml.Controls.RefreshVisualizer"
//		}
//	],
//  "RelatedControls": [
//    "GridView",
//    "ListView"
//  ]
//},

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class PullToRefreshPage : Page
    {
#if false
        private ObservableCollection<string> items = new ObservableCollection<string>();

        public PullToRefreshPage()
        {
            this.InitializeComponent();
            foreach (var c in @"AcrylicBrush ColorPicker NavigationView ParallaxView PersonPicture PullToRefresh RatingControl RevealBrush TreeView".Split(' '))
                items.Add(c);
            lv.ItemsSource = items;
        }

        private void Rc_Loaded(object sender, RoutedEventArgs e)
        {
            rc.RefreshVisualizer.RefreshRequested += RefreshVisualizer_RefreshRequested;
        }

        private void RefreshVisualizer_RefreshRequested(Windows.UI.Xaml.Controls.RefreshVisualizer sender, Windows.UI.Xaml.Controls.RefreshRequestedEventArgs args)
        {
            items.Insert(0, "NewControl");
            args.GetDeferral().Complete();
        }
#else
        public PullToRefreshPage()
        {
            this.InitializeComponent();
        }
#endif
    }
}
