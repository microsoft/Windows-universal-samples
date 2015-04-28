//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ListBoxPage : Page
    {
        private List<Tuple<string, FontFamily>> _fonts = new List<Tuple<string, FontFamily>>()
        {
            new Tuple<string, FontFamily>("Arial", new FontFamily("Arial")),
            new Tuple<string, FontFamily>("Comic Sans MS", new FontFamily("Comic Sans MS")),
            new Tuple<string, FontFamily>("Courier New", new FontFamily("Courier New")),
            new Tuple<string, FontFamily>("Segoe UI", new FontFamily("Segoe UI")),
            new Tuple<string, FontFamily>("Times New Roman", new FontFamily("Times New Roman"))
        };

        public List<Tuple<string, FontFamily>> Fonts
        {
            get { return _fonts; }
        }
        public ListBoxPage()
        {
            this.InitializeComponent();
        }

        private void ColorListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string colorName = e.AddedItems[0].ToString();
            switch (colorName)
            {
                case "Yellow":
                    Control1Output.Fill = new SolidColorBrush(Windows.UI.Colors.Yellow);
                    break;
                case "Green":
                    Control1Output.Fill = new SolidColorBrush(Windows.UI.Colors.Green);
                    break;
                case "Blue":
                    Control1Output.Fill = new SolidColorBrush(Windows.UI.Colors.Blue);
                    break;
                case "Red":
                    Control1Output.Fill = new SolidColorBrush(Windows.UI.Colors.Red);
                    break;
            }
        }

        private void ListBox2_Loaded(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            ListBox2.SelectedIndex = 2;
        }
    }
}
