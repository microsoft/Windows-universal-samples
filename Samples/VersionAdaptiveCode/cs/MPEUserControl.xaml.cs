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
using Windows.Media.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace SDKTemplate
{
    public sealed partial class MPEUserControl : UserControl
    {
        MediaPlayerElement MPE;
        public MPEUserControl()
        {
            this.InitializeComponent();

            MPE = new MediaPlayerElement();
            MPE.AreTransportControlsEnabled = true;
            MPE.AutoPlay = true;

            MPE.HorizontalAlignment = HorizontalAlignment.Stretch;
            MPE.VerticalAlignment = VerticalAlignment.Stretch;
            MPE.MediaPlayer.IsLoopingEnabled = true;
            RootGrid.Children.Add(MPE);
            MPE.SetValue(Grid.RowProperty, 1);
        }

        private void Control_Loaded(object sender, RoutedEventArgs e)
        {
            Uri uri = new Uri("ms-appx:///Assets/ladybug.wmv");
            MPE.Source = MediaSource.CreateFromUri(uri);
        }

        private void Control_Unloaded(object sender, RoutedEventArgs e)
        {
            MPE.Source = null;
        }

        private void FrameStepForward_Click(object sender, RoutedEventArgs e)
        {
            MPE.MediaPlayer.StepForwardOneFrame();
        }

        private void FrameStepBackward_Click(object sender, RoutedEventArgs e)
        {
            MPE.MediaPlayer.StepBackwardOneFrame();
        }
    }
}
