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

using SDKTemplate.ViewModels;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// This Scenario demonstrates simple Reactive license Acquisition
    /// </summary>
    public sealed partial class Scenario1_ReactiveRequest : Page
    {
        public Scenario1_ReactiveRequest()
        {
            this.InitializeComponent();
            this.DataContext = new ReactiveViewModel(mediaElement);
        }
    }
}
