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
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;

        public Scenario1()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        /// <summary>
        /// This method demonstrates logging with a LoggingChannel constructed
        /// in Windows 8.1 compatibility mode.
        /// </summary>
        private void DoWin81Mode(object sender, RoutedEventArgs e)
        {
            new LoggingChannelScenario().LogWithWin81Constructor();
            this.rootPage.NotifyUser("Complete: Windows 8.1 mode", NotifyType.StatusMessage);
        }

        /// <summary>
        /// This method demonstrates logging with a LoggingChannel constructed
        /// in Windows 10 enhanced mode.
        /// </summary>
        private void DoWin10Mode(object sender, RoutedEventArgs e)
        {
            new LoggingChannelScenario().LogWithWin10Constructor();
            this.rootPage.NotifyUser("Complete: Windows 10 mode", NotifyType.StatusMessage);
        }
    }
}
