using System;
using System.Collections.Generic;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

#if !BUILD_WINDOWS
using DataSource = Microsoft.UI.Xaml.Controls.DataSource;
using DataSourceChangedEventArgs = Microsoft.UI.Xaml.Controls.DataSourceChangedEventArgs;
using LayoutContext = Microsoft.UI.Xaml.Controls.LayoutContext;
using VirtualizingLayoutBase = Microsoft.UI.Xaml.Controls.VirtualizingLayoutBase;
using VirtualizingLayoutContext = Microsoft.UI.Xaml.Controls.VirtualizingLayoutContext;
#endif

namespace AppUIBasics.Common
{
    class ActivityFeedLayout : VirtualizingLayoutBase
    {
        #region Layout parameters

        // Cached copy of dependency properties so that we do not have to do GetValue in the host path during
        // layout. This can be quite expensive due to the number of times we end up calling these.
        private double _rowSpacing;
        private double _itemSpacing;
        private Size _minItemSize = Size.Empty;

        public double RowSpacing
        {
            get { return _rowSpacing; }
            set { SetValue(RowSpacingProperty, value); }
        }

        public static readonly DependencyProperty RowSpacingProperty =
            DependencyProperty.Register("RowSpacing", typeof(double), typeof(ActivityFeedLayout), new PropertyMetadata(0, OnPropertyChanged));

        public double ItemSpacing
        {
            get { return _itemSpacing; }
            set { SetValue(ItemSpacingProperty, value); }
        }

        public static readonly DependencyProperty ItemSpacingProperty =
            DependencyProperty.Register("ItemSpacing", typeof(double), typeof(ActivityFeedLayout), new PropertyMetadata(0, OnPropertyChanged));

        public Size MinItemSize
        {
            get { return _minItemSize; }
            set { SetValue(MinItemSizeProperty, value); }
        }

        public static readonly DependencyProperty MinItemSizeProperty =
            DependencyProperty.Register("MinItemSize", typeof(Size), typeof(ActivityFeedLayout), new PropertyMetadata(Size.Empty, OnPropertyChanged));

        #endregion

        #region Setup / teardown

        protected override void OnAttachedCore(LayoutContext context)
        {
            base.OnAttachedCore(context);

            var state = context.LayoutState as ActivityFeedLayoutState;
            if (state == null)
            {
                // Store any state we might need since (in theory) the layout could be in use by multiple elements simultaneously
                // In reality for the Xbox Activity Feed there's probably only a single instance.
                context.LayoutState = new ActivityFeedLayoutState();
            }
        }

        protected override void OnDetachingCore(LayoutContext context)
        {
            base.OnDetachingCore(context);

            // clear any state
            context.LayoutState = null;
        }

        #endregion

        #region Layout

        protected override Size MeasureLayoutCore(Size availableSize, LayoutContext context)
        {
            // In the Xbox scenario there isn't a user grabbing the window and resizing it.  Otherwise, it
            // might be useful to skip a full layout here and return a cached size until some condition is met
            // such as having enough room to fit another column.  That would require extra work like 
            // tracking some additional state, comparing the virtualizing rect with the cached value, or setting
            // a timeout to invalidate the layout and resetting the timer while the available width is changing.

            var virtualizingContext = context as VirtualizingLayoutContext;
            if (virtualizingContext == null)
            {
                throw new InvalidOperationException("This layout requires a virtualizing host like Repeater");
            }

            if (this.MinItemSize == Size.Empty)
            {
                var firstElement = virtualizingContext.GetElementAt(0);
                firstElement.Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));

                // setting the member value directly to skip invalidating layout
                this._minItemSize = firstElement.DesiredSize;
            }

            // ideal item width that will expand/shrink to fill available space
            double desiredItemWidth = Math.Max(this.MinItemSize.Width, (availableSize.Width - this.ItemSpacing * 3) / 4);

            var state = context.LayoutState as ActivityFeedLayoutState;

            var realizationRect = virtualizingContext.GetRealizationRect();

            // estimate the extent by finding the last item and getting its bottom/right position
            var extentHeight = ((int)(context.ItemCount / 3) - 1) * (this.MinItemSize.Height + this.RowSpacing) + this.MinItemSize.Height;

            // Determine which rows need to be realized
            var firstRow = Math.Max((int)(realizationRect.Y / (this.MinItemSize.Height + this.RowSpacing)) - 1, 0);
            var lastRow = Math.Min((int)(realizationRect.Bottom / (this.MinItemSize.Height + this.RowSpacing)) + 1, (int)(context.ItemCount / 3));

            // Determine which items fall on those rows and determine the rect for each item
            SortedDictionary<int, Tuple<UIElement, Rect>> positions = new SortedDictionary<int, Tuple<UIElement, Rect>>();

            Rect[] rectArray = new Rect[3];
            rectArray[0].Height = rectArray[1].Height = rectArray[2].Height = this.MinItemSize.Height;

            for (int rowIndex = firstRow; rowIndex < lastRow; rowIndex++)
            {
                var yoffset = rowIndex * (this.MinItemSize.Height + this.RowSpacing);

                // initialize Y positions of the rects
                rectArray[0].Y = rectArray[1].Y = rectArray[2].Y = yoffset;

                var firstItemIndex = rowIndex * 3;
                var wnn = firstItemIndex % 3 == 0 && firstItemIndex % 6 != 0;
                var nnw = firstItemIndex % 3 == 0 && firstItemIndex % 6 == 0;

                // Update the rects to what is required for a given row
                if (wnn)
                {
                    // Left tile (wide)
                    rectArray[0].X = 0;
                    rectArray[0].Width = (desiredItemWidth * 2 + this.ItemSpacing);
                    // Middle tile (narrow)
                    rectArray[1].X = rectArray[0].Right + this.ItemSpacing;
                    rectArray[1].Width = desiredItemWidth;
                    // Right tile (narrow)
                    rectArray[2].X = rectArray[1].Right + this.ItemSpacing;
                    rectArray[2].Width = desiredItemWidth;
                }
                else if (nnw)
                {
                    // Left tile (narrow)
                    rectArray[0].X = 0;
                    rectArray[0].Width = desiredItemWidth;
                    // Middle tile (narrow)
                    rectArray[1].X = rectArray[0].Right + this.ItemSpacing;
                    rectArray[1].Width = desiredItemWidth;
                    // Right tile (wide)
                    rectArray[2].X = rectArray[1].Right + this.ItemSpacing;
                    rectArray[2].Width = desiredItemWidth * 2 + this.ItemSpacing;
                }


                UIElement container = null;
                for (int i = 0; i < 3; i++)
                {
                    var index = firstItemIndex + i;
                    if (state.Containers.ContainsKey(index))
                    {
                        container = state.Containers[index].Item1;
                        var rect = rectArray[index % 3];
                        positions.Add(index, new Tuple<UIElement, Rect>(container, rect));

                        state.Containers.Remove(index);
                    }
                    else
                    {
                        // GetElementAt will return a new element each time it is called in a virtualizing
                        // context so in any given layout pass it should only be called once for a given 
                        // index and only for those that need to be measured again.  In this layout we
                        // only call measure on the items that are being realized for the first time or
                        // have just been recycled.
                        container = virtualizingContext.GetElementAt(index);
                        container.Measure(new Size(rectArray[i].Width, rectArray[i].Height));
                        positions.Add(index, new Tuple<UIElement, Rect>(container, rectArray[i]));
                    }
                }
            }

            // clear anything that wasn't used
            foreach (var item in state.Containers)
            {
                virtualizingContext.ClearElement(state.Containers[item.Key].Item1);
            }

            // store the containers we used for use on the next layout pass
            state.Containers = positions;

            // Report a desired size for the layout
            return new Size(desiredItemWidth * 4 + this.ItemSpacing * 2, extentHeight);
        }

        protected override Size ArrangeLayoutCore(Size finalSize, LayoutContext context)
        {
            // walk through the cache of containers and arrange
            var state = context.LayoutState as ActivityFeedLayoutState;
            if (state == null) throw new InvalidOperationException();

            foreach (var key in state.Containers.Keys)
            {
                var container = state.Containers[key].Item1;
                var rect = state.Containers[key].Item2;
                container.Arrange(rect);
            }

            return finalSize;
        }

        #endregion
        protected override void OnDataSourceChangedCore(DataSource source, DataSourceChangedEventArgs args, VirtualizingLayoutContext context)
        {
            // In this layout we don't really do anything special, but the default implementation of this method
            // throws a runtime exception so minimally we need to make this a no-op
            //var state = context.LayoutState as ActivityFeedLayoutState;
            //if (state == null) throw new InvalidOperationException();
        }

        private static void OnPropertyChanged(DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            var layout = obj as ActivityFeedLayout;
            if (args.Property == RowSpacingProperty)
            {
                layout._rowSpacing = (double)args.NewValue;
            }
            else if (args.Property == ItemSpacingProperty)
            {
                layout._itemSpacing = (double)args.NewValue;
            }
            else if (args.Property == MinItemSizeProperty)
            {
                layout._minItemSize = (Size)args.NewValue;
            }
            else
            {
                throw new InvalidOperationException("Don't know what you are talking about !");
            }

            layout.OnLayoutInvalidated();
        }

    }

    internal class ActivityFeedLayoutState
    {
        private SortedDictionary<int, Tuple<UIElement, Rect>> _containers = new SortedDictionary<int, Tuple<UIElement, Rect>>();
        /// <summary>
        /// The set of realized containers and their layout Rect ordered by item index in the data source.  For this layout we know
        /// which items are realized simply by looking at the first and last index.  The layout guarantees all items in between those
        /// two will be realized so we could instead use a list and treat it as a sliding window.  It just seemed easier in the 
        /// MeasureLayoutCore to walk through once pulling from the existing set of containers to re-use and then clearing any container
        /// that hadn't been used.
        /// </summary>
        public SortedDictionary<int, Tuple<UIElement, Rect>> Containers
        {
            get
            {
                return _containers;
            }

            internal set
            {
                _containers = value;
            }
        }
    }
}
