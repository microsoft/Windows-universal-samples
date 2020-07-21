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
    public sealed partial class VoicemailPanel : UserControl
    {
        VoicemailViewModel voicemailVM;

        /// <summary>
        /// A user control that hosts the voicemail panel in the main pivot/tab.
        /// </summary>
        public VoicemailPanel()
        {
            this.InitializeComponent();
            voicemailVM = ViewModelDispatcher.VoicemailViewModel;
        }
    }
}
