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
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Sample page to call TryShow/TryHide methods.
    /// </summary>
    public sealed partial class Scenario3_ShowHideMethods : Page
    {
        public Scenario3_ShowHideMethods()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            FadeOutResults.Stop();
        }

        void WordListBox_OnKeyUp(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Enter)
            {
                string text = WordListBox.Text;
                if (text.Length > 0)
                {
                    Windows.UI.ViewManagement.InputPane.GetForCurrentView().TryHide();
                    ResultsTextBlock.Text = text;                    
                    FadeOutResults.Begin();
                }
            }
        }

        void OnFadeOutCompleted(object sender, object e)
        {
            ResultsTextBlock.Text = "";
            Windows.UI.ViewManagement.InputPane.GetForCurrentView().TryShow();
        }
    }
}
