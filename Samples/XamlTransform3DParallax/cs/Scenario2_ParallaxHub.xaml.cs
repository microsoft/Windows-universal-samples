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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Transform3DParallax
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_ParallaxHub : Page
    {
        private static DependencyProperty s_desiredHubSectionWidthProperty
            = DependencyProperty.Register("DesiredHubSectionWidth", typeof(double), typeof(Scenario2_ParallaxHub), new PropertyMetadata(560.0));

        public static DependencyProperty DesiredHubSectionWidthProperty
        {
            get { return s_desiredHubSectionWidthProperty; }
        }

        public double DesiredHubSectionWidth
        {
            get { return (double)GetValue(s_desiredHubSectionWidthProperty); }
            set { SetValue(s_desiredHubSectionWidthProperty, value); }
        }

        public Scenario2_ParallaxHub()
        {
            this.InitializeComponent();
        }

        private void ParallaxBackgroundHub_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            // For adaptability, we want to keep hub sections within one screen width.
            DesiredHubSectionWidth = e.NewSize.Width * .9;
        }
    }
}
