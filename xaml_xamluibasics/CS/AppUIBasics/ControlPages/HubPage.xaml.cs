//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using System.Collections.Generic;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class HubPage : Page
    {
        public HubPage()
        {
            this.InitializeComponent();
            Loaded += HubPage_Loaded;
        }

        void HubPage_Loaded(object sender, RoutedEventArgs e)
        {
            List<string> sections = new List<string>();

            foreach (HubSection section in NewsHub.Sections)
            {
                if (section.Header != null)
                {
                    sections.Add(section.Header.ToString());
                }
            }

            ZoomedOutList.ItemsSource = sections;
        }
    }
}
