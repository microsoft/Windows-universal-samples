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

using SDKTemplate;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Commanding
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3 : Page
    {
        public Scenario3()
        {
            this.InitializeComponent();
            MainPage.Current.NotifyUser("CommandBar: Background Opacity at 50%", NotifyType.StatusMessage);
        }

        private void CommandBar_Opening(object sender, object e)
        {
            this.translucentBackgroundBrush.Opacity = 1.0;
            MainPage.Current.NotifyUser("CommandBar: OPENING.  Setting Background Opacity to 100%", NotifyType.ErrorMessage);
        }

        private void CommandBar_Closing(object sender, object e)
        {
            MainPage.Current.NotifyUser("CommandBar: CLOSING", NotifyType.ErrorMessage);
        }

        private void CommandBar_Opened(object sender, object e)
        {
            MainPage.Current.NotifyUser("CommandBar: OPENED", NotifyType.StatusMessage);
        }

        private void CommandBar_Closed(object sender, object e)
        {
            this.translucentBackgroundBrush.Opacity = 0.5;
            MainPage.Current.NotifyUser("CommandBar: CLOSED.  Setting Background Opacity to 50%", NotifyType.StatusMessage);
        }
    }
}
