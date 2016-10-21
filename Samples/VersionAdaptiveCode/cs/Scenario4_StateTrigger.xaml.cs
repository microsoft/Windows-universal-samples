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

using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// this page shows how to use a state trigger to manipulate setter properties if the property is available.
    /// In this particular case, we conditionally set the AllowFocusOnInteraction property.
    /// </summary>
    public sealed partial class Scenario4_StateTrigger : Page
    {
        public Scenario4_StateTrigger()
        {
            this.InitializeComponent();
        }
    }
}
