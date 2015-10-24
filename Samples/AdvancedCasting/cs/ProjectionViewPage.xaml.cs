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

using ScreenCasting.Controls;
using System;
using System.Threading.Tasks;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace ScreenCasting
{
    public sealed partial class ProjectionViewPage : Page
    {
        public ProjectionViewPage()
        {
            this.InitializeComponent();

            ProjectedMediaTransportControls pmtcs = this._player.TransportControls as ProjectedMediaTransportControls;
            if (pmtcs != null)
                pmtcs.StopProjectingButtonClick += ProjectionViewPage_StopProjectingButtonClick;

            this._player.MediaOpened += Player_MediaOpened;
        }

        private void Player_MediaOpened(object sender, RoutedEventArgs e)
        {
            this._player.IsFullWindow = true;
            this._player.AreTransportControlsEnabled = true;
        }

        private void ProjectionViewPage_StopProjectingButtonClick(object sender, EventArgs e)
        {
            this.StopProjecting();
        }

        

        public async Task<bool> SetMediaSource (Uri source, TimeSpan position)
        {            
            await this._player.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {                
                this._player.Source = source;
                this._player.Position = position;
                this._player.Play();
            });
            return true;
        }

        public MediaElement Player
        {
            get { return this._player; }
        }

        ProjectionViewBroker broker = null;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            broker = (ProjectionViewBroker)e.Parameter;
            broker.ProjectedPage = this;

            // Listen for when it's time to close this view
            broker.ProjectionViewPageControl.Released += thisViewControl_Released;
        }
        
        private async void thisViewControl_Released(object sender, EventArgs e)
        {                        
            // There are two major cases where this event will get invoked:
            // 1. The view goes unused for some time, and the system cleans it up
            // 2. The app calls "StopProjectingAsync"
            broker.ProjectionViewPageControl.Released -= thisViewControl_Released;
            await broker.ProjectionViewPageControl.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    this.broker.NotifyProjectionStopping();
                    MainPage.Current.ProjectionViewPageControl = null;
                });

            this._player.Stop();
            this._player.Source = null;

            Window.Current.Close();
        }

        public async void SwapViews()
        {
            // The view might arrive on the wrong display. The user can
            // easily swap the display on which the view appears
            broker.ProjectionViewPageControl.StartViewInUse();
            await ProjectionManager.SwapDisplaysForViewsAsync(
                ApplicationView.GetForCurrentView().Id,
                broker.MainViewId
            );
            broker.ProjectionViewPageControl.StopViewInUse();                       
        }
        private void SwapViews_Click(object sender, RoutedEventArgs e)
        {
            SwapViews();
        }

        public async void StopProjecting()
        {
            broker.NotifyProjectionStopping();
           
            // There may be cases to end the projection from the projected view
            // (e.g. the presentation hosted in that view concludes)
            // broker.ProjectionViewPageControl.StartViewInUse();
            await this._player.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                this._player.Stop();
                this._player.Source = null;

                broker.ProjectionViewPageControl.StartViewInUse();

                try
                {
                    await ProjectionManager.StopProjectingAsync(
                        broker.ProjectionViewPageControl.Id,
                        broker.MainViewId                     
                        );
                }
                catch { }
                Window.Current.Close();

            });
            
           broker.ProjectionViewPageControl.StopViewInUse();           
        }
        private void StopProjecting_Click(object sender, RoutedEventArgs e)
        {
            StopProjecting();
        }
    }
}
