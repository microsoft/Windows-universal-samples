using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace XamlPullToRefresh
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private ObservableCollection<int> feed = new ObservableCollection<int>();
        private DispatcherTimer timer = new DispatcherTimer();
        public MainPage()
        {
            this.InitializeComponent();
            Loaded += MainPage_Loaded;
            timer.Interval = TimeSpan.FromSeconds(1);
            timer.Tick += Timer_Tick;
            PopulateFeed();
        }

        private void Timer_Tick(object sender, object e)
        {
            timer.Stop();
            VisualStateManager.GoToState(this, "PullToRefresh", false);
            for (int i = 20; i < 40; i++)
            {
                feed.Insert(0, i);
            }
        }

        private void PopulateFeed()
        {
            for (int i = 0; i < 10; i++)
            {
                feed.Add(i);
            }
            lv.ItemsSource = feed;
        }

        private void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Hide the refresh indicator
            SV1.ChangeView(null, 100, null);
        }

        private void ScrollViewer_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            ScrollViewer sv = sender as ScrollViewer;
            if (sv.VerticalOffset == 0)
            {
                timer.Start();
                VisualStateManager.GoToState(this, "Refreshing", false);
            }

        }

    }

    public class MyBorder : Panel
    {
        public Size myAvailableSize { get; set; }

        public Size myFinalSize { get; set; }

        protected override Size MeasureOverride(Size availableSize)
        {
            this.myAvailableSize = availableSize;
            this.Children[0].Measure(availableSize);

            return this.Children[0].DesiredSize;
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            this.myFinalSize = finalSize;

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

            this.Children[0].Measure(myBorder.myAvailableSize);
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

            this.Children[0].Arrange(new Rect(0, 0, this.Children[0].DesiredSize.Width, this.Children[0].DesiredSize.Height));
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
