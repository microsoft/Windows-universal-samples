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

namespace VideoPlayback
{
    public sealed class CustomMediaTransportControls : MediaTransportControls
    {
        public event EventHandler<EventArgs> Liked;

        public CustomMediaTransportControls()
        {
            this.DefaultStyleKey = typeof(CustomMediaTransportControls);
        }

        protected override void OnApplyTemplate()
        {
            // Find the custom button and create an event handler for its click method.
            var likeButton = GetTemplateChild("LikeButton") as Button;
            likeButton.Click += LikeButton_Click;

            base.OnApplyTemplate();
        }

        private void LikeButton_Click(object sender, RoutedEventArgs e)
        {
            //  Raise an event on the custom control when 'like' is clicked
            var handler = Liked;
            if (handler != null)
            {
                handler(this, EventArgs.Empty);
            }
        }
    }
}
