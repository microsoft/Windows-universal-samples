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

namespace SDKTemplate
{
    /// <summary>
    /// 
    /// </summary>
    public sealed partial class Scenario12 : Page
    {
        public Scenario12()
        {
            this.InitializeComponent();

            // Initialize the InkCanvas
            inkCanvas.InkPresenter.InputDeviceTypes =
                Windows.UI.Core.CoreInputDeviceTypes.Mouse |
                Windows.UI.Core.CoreInputDeviceTypes.Pen;
        }

        private void OnDrawingAttributesChanged(InkToolbar sender, object args)
        {
            // Enable tilt support.
            sender.InkDrawingAttributes.IgnoreTilt = false;
            inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(sender.InkDrawingAttributes);
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            HelperFunctions.UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
        }

        
    }
}