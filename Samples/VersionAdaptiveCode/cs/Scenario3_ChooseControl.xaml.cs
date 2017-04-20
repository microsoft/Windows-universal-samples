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
using Windows.Foundation.Metadata;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// This scenario shows how to choose a user control based on whether a class is present.
    /// You can create two implementations of a user control, one that relies on a new feature,
    /// and one that does not, and choose the control to use at runtime.
    /// </summary>
    public sealed partial class Scenario3_ChooseControl : Page
    {
        public Scenario3_ChooseControl()
        {
            this.InitializeComponent();

            UserControl mediaControl;

            // If the MediaPlayerElement class is available, then create a user control that uses it.
            // Otherwise, create a user control that uses the older MediaElement control.
            if (ApiInformation.IsTypePresent("Windows.UI.Xaml.Controls.MediaPlayerElement"))
            {
                mediaControl = new MPEUserControl();
            }
            else
            {
                mediaControl = new MEUserControl();
            }

            // Insert the control into our page.
            MediaPlayerPlacement.Children.Add(mediaControl);
        }

    }
}
