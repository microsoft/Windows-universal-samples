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

namespace AllJoynSecureClient
{
    public sealed partial class Scenario1 : Page
    {
        public Scenario1()
        {
            this.InitializeComponent();
            this.DataContext = new ClientViewModel();
        }
    }
}
