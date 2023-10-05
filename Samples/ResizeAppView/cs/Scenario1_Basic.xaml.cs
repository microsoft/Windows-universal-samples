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
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Basic : Page
    {
        private MainPage rootPage;

        public Scenario1_Basic()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            Window.Current.SizeChanged += OnWindowSizeChanged;
            UpdateContent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            Window.Current.SizeChanged -= OnWindowSizeChanged;
        }


        IMemoryBufferReference mbrr = null;
        bool done = false;
        private void AttachRef()
        {
            var buffer = new Windows.Storage.Streams.Buffer(10);
            var memorybuffer = Windows.Storage.Streams.Buffer.CreateMemoryBufferOverIBuffer(buffer);
            memorybuffer.CreateReference().Closed += (s, _) => {
                //mbrr = s;
                var x = 42;
                done = true;
            };
        }
        private void ResizeView_Click(object sender, RoutedEventArgs e)
        {
            var view = ApplicationView.GetForCurrentView();
            _ = Windows.System.Threading.ThreadPool.RunAsync((o) => {
                AttachRef();
                while (!done)
                {
                    GC.Collect();
                }
                mbrr = null;
                GC.Collect();
            });
        }

        private void SetMinimumSize_Click(object sender, RoutedEventArgs e)
        {
            var view = ApplicationView.GetForCurrentView();
            if (SetMinimumSize.IsChecked.Value)
            {
                // If this size is not permitted by the system, the nearest permitted value is used.
                view.SetPreferredMinSize(new Size { Width = 300, Height = 200 });
            }
            else
            {
                // Passing width = height = 0 returns to the default system behavior.
                view.SetPreferredMinSize(new Size());
            }
        }

        void OnWindowSizeChanged(object sender, WindowSizeChangedEventArgs e)
        {
            UpdateContent();
        }

        void UpdateContent()
        {
            Rect bounds = Window.Current.Bounds;
            ReportViewWidth.Text = bounds.Width.ToString();
            ReportViewHeight.Text = bounds.Height.ToString();
        }
    }
}
