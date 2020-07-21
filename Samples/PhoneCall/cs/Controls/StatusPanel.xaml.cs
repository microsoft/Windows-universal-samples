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
using PhoneCall.ViewModels;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PhoneCall.Controls
{
    public sealed partial class StatusPanel : UserControl
    {
        StatusViewModel statusVM;

        /// <summary>
        /// A user control that hosts the status panel in the main pivot/tab.
        /// </summary>
        public StatusPanel()
        {
            this.InitializeComponent();
            statusVM = ViewModelDispatcher.StatusViewModel;
        }
    }
}
