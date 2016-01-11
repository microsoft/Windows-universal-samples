using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace XamlPullToRefresh
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private ObservableCollection<int> feed = new ObservableCollection<int>();
        private int lastValue = 0;
        public MainPage()
        {
            this.InitializeComponent();
            Loaded += MainPage_Loaded;
            Window.Current.SizeChanged += Current_SizeChanged;
            PopulateFeed();
            InnerCustomPanel.SizeChanged += InnerCustomPanel_SizeChanged;
        }

        private void InnerCustomPanel_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            SV1.ChangeView(null, 100.0, null, true);
        }

        private void Current_SizeChanged(object sender, Windows.UI.Core.WindowSizeChangedEventArgs e)
        {
            InnerCustomPanel.InvalidateMeasure();
        }

        private void PopulateFeed()
        {
            feed.Clear();
            for (int i = 90; i <= 100; i++)
            {
                feed.Add(i);
            }
            lv.ItemsSource = feed;
            lastValue = 89;
        }

        private void UpdateFeed()
        {
            for (int i = lastValue; i > lastValue - 10; i--)
            {
                feed.Insert(0, i);
            }
            lastValue = lastValue - 10;
            if(lastValue < 0)
            {
                PopulateFeed();
            }
            SV1.ChangeView(null, 0, null, true);
            VisualStateManager.GoToState(this, "PullToRefresh", false);
        }

        private void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Hide the refresh indicator (SV1 is the outer ScrollViewer)
            SV1.ChangeView(null, 100, null);
        }

        private void ScrollViewer_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            ScrollViewer sv = sender as ScrollViewer;
            if (sv.VerticalOffset == 0)
            {
                SV1.DirectManipulationCompleted += SV1_DirectManipulationCompleted;
                VisualStateManager.GoToState(this, "Refreshing", false);
            }
        }

        private void SV1_DirectManipulationCompleted(object sender, object e)
        {
            SV1.DirectManipulationCompleted -= SV1_DirectManipulationCompleted;
            UpdateFeed();
        }

    }

    public class MyBorder : Panel
    {
        public Size myAvailableSize { get; set; }

        public Size myFinalSize { get; set; }

        protected override Size MeasureOverride(Size availableSize)
        {
            this.myAvailableSize = availableSize;
            // Children[0] is the outer ScrollViewer
            this.Children[0].Measure(availableSize);
            return this.Children[0].DesiredSize;
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            this.myFinalSize = finalSize;
            // Children[0] is the outer ScrollViewer
            this.Children[0].Arrange(new Rect(0, 0, finalSize.Width, finalSize.Height));
            return finalSize;
        }
    }

    public class MyPanel : Panel, IScrollSnapPointsInfo
    {
        EventRegistrationTokenTable<EventHandler<object>> _verticaltable = new EventRegistrationTokenTable<EventHandler<object>>();
        EventRegistrationTokenTable<EventHandler<object>> _horizontaltable = new EventRegistrationTokenTable<EventHandler<object>>();

        protected override Size MeasureOverride(Size availableSize)
        {
            // need to get away from infinity
            var parent = this.Parent as FrameworkElement;
            while (!(parent is MyBorder))
            {
                parent = parent.Parent as FrameworkElement;
            }

            var myBorder = parent as MyBorder;

            // Children[0] is the Border that comprises the refresh UI
            this.Children[0].Measure(myBorder.myAvailableSize);
            // Children[1] is the ListView
            this.Children[1].Measure(new Size(myBorder.myAvailableSize.Width, myBorder.myAvailableSize.Height));
            return new Size(this.Children[1].DesiredSize.Width, this.Children[0].DesiredSize.Height + myBorder.myAvailableSize.Height);
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            // need to get away from infinity
            var parent = this.Parent as FrameworkElement;
            while (!(parent is MyBorder))
            {
                parent = parent.Parent as FrameworkElement;
            }

            var myBorder = parent as MyBorder;

            // Children[0] is the Border that comprises the refresh UI
            this.Children[0].Arrange(new Rect(0, 0, this.Children[0].DesiredSize.Width, this.Children[0].DesiredSize.Height));
            // Children[1] is the ListView
            this.Children[1].Arrange(new Rect(0, this.Children[0].DesiredSize.Height, myBorder.myFinalSize.Width, myBorder.myFinalSize.Height));
            return finalSize;
        }

        bool IScrollSnapPointsInfo.AreHorizontalSnapPointsRegular
        {
            get { return false; }
        }

        bool IScrollSnapPointsInfo.AreVerticalSnapPointsRegular
        {
            get { return false; }
        }

        IReadOnlyList<float> IScrollSnapPointsInfo.GetIrregularSnapPoints(Orientation orientation, SnapPointsAlignment alignment)
        {
            if (orientation == Orientation.Vertical)
            {
                var l = new List<float>();
                l.Add((float)this.Children[0].DesiredSize.Height);
                return l;
            }
            else
            {
                return new List<float>();
            }
        }

        float IScrollSnapPointsInfo.GetRegularSnapPoints(Orientation orientation, SnapPointsAlignment alignment, out float offset)
        {
            throw new NotImplementedException();
        }

        event EventHandler<object> IScrollSnapPointsInfo.HorizontalSnapPointsChanged
        {
            add
            {
                return EventRegistrationTokenTable<EventHandler<object>>
                        .GetOrCreateEventRegistrationTokenTable(ref this._horizontaltable)
                        .AddEventHandler(value);

            }
            remove
            {
                EventRegistrationTokenTable<EventHandler<object>>
                    .GetOrCreateEventRegistrationTokenTable(ref this._horizontaltable)
                    .RemoveEventHandler(value);
            }
        }

        event EventHandler<object> IScrollSnapPointsInfo.VerticalSnapPointsChanged
        {
            add
            {
                return EventRegistrationTokenTable<EventHandler<object>>
                        .GetOrCreateEventRegistrationTokenTable(ref this._verticaltable)
                        .AddEventHandler(value);

            }
            remove
            {
                EventRegistrationTokenTable<EventHandler<object>>
                    .GetOrCreateEventRegistrationTokenTable(ref this._verticaltable)
                    .RemoveEventHandler(value);
            }
        }
    }
}
