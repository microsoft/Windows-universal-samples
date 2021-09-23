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

// This sample shows how the default InkCanvas supports tactile signals.
// Simply begin inking in the InkCanvas with a supported pen and tactile signals
// will automatically be played for each of the different brush types.
namespace SDKTemplate
{
    public sealed partial class Scenario1_InkCanvasTactileFeedback : Page
    {

        public Scenario1_InkCanvasTactileFeedback()
        {
            InitializeComponent();
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            outputGrid.Width = RootGrid.ActualWidth;
            outputGrid.Height = RootGrid.ActualHeight / 2;
        }
    }
}
