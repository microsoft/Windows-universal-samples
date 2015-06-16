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
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace ScreenCasting.Controls
{
 public sealed class MediaTransportControlsWithCustomCastButton : MediaTransportControls
    {

        public MediaTransportControlsWithCustomCastButton()
        {
            this.DefaultStyleKey = typeof(MediaTransportControlsWithCustomCastButton);
        }

        public event EventHandler CastButtonClicked;
        private void CastButton_Click(object sender, RoutedEventArgs e)
        {
            if (CastButtonClicked != null)
                CastButtonClicked(this, EventArgs.Empty);
        }

        public Button CastButton
        {
            get { return this.GetTemplateChild("CustomCastButton") as Button; }
        }
        protected override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            this.IsCompact = true;

            CastButton.Click += CastButton_Click;
        }
    }
}
