using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace Commanding
{
    /// <summary>
    /// We extend the CommandBar by adding logic to reflow commands as space is reduced
    /// </summary>
    public class DynamicCommandBar : CommandBar
    {
        public DynamicCommandBar()
        {
            //this.SizeChanged += CommandBar_SizeChanged;
        }

        public double ContentMinWidth
        {
            get { return (double)GetValue(ContentMinWidthProperty); }
            set { SetValue(ContentMinWidthProperty, value); }
        }

        public static readonly DependencyProperty ContentMinWidthProperty =
            DependencyProperty.Register("ContentMinWidth", typeof(double), typeof(DynamicCommandBar), new PropertyMetadata(0.0d));


        private Button moreButton;
        private ItemsControl primaryCommands;
        protected override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
            moreButton = this.GetTemplateChild("MoreButton") as Button;
            primaryCommands = this.GetTemplateChild("PrimaryItemsControl") as ItemsControl;
        }

        // Store the item, the width before moving, if it is a separator, and if it appears in the overflow
        private Stack<Tuple<ICommandBarElement, double>> overflow = new Stack<Tuple<ICommandBarElement, double>>();
        private Queue<Tuple<ICommandBarElement, double>> separatorQueue = new Queue<Tuple<ICommandBarElement, double>>();
        private double separatorQueueWidth = 0.0;

        protected override Size MeasureOverride(Size availableSize)
        {
            var sizeToReport = base.MeasureOverride(availableSize);

            // Account for the size of the Content area
            var contentWidth = this.ContentMinWidth;

            // Account for the size of the More button 
            var expandButtonWidth = 0.0;
            if (moreButton != null && moreButton.Visibility == Visibility.Visible)
            {
                expandButtonWidth = moreButton.DesiredSize.Width;
            }

            double requestedWidth = expandButtonWidth + contentWidth;

            // Include the size of all the PrimaryCommands
            foreach (var cmd in this.PrimaryCommands)
            {
                var uie = cmd as UIElement;
                requestedWidth += uie.DesiredSize.Width;
            }

            // First, move items to the overflow until the remaining PrimaryCommands fit
            for (int i = this.PrimaryCommands.Count - 1; i > 0 && requestedWidth > availableSize.Width; i--)
            {
                var item = this.PrimaryCommands[i];
                var element = item as UIElement;

                if (element == null)
                {
                    continue;
                }

                requestedWidth -= element.DesiredSize.Width;

                if (this.overflow.Count == 0 && !(element is AppBarSeparator))
                {
                    // Insert a separator to differentiate between the items that were already in the overflow versus
                    // those we moved
                    var abs = new AppBarSeparator();
                    abs.Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));
                    this.separatorQueue.Enqueue(new Tuple<ICommandBarElement, double>(abs, abs.DesiredSize.Width));
                    separatorQueueWidth += abs.DesiredSize.Width;
                }

                this.PrimaryCommands.RemoveAt(i);

                // move separators we've queued up 
                while (separatorQueue.Count > 0)
                {
                    var next = separatorQueue.Dequeue();
                    separatorQueueWidth -= next.Item2;
                    overflow.Push(next);
                    this.SecondaryCommands.Insert(0, next.Item1);
                }

                // We store the measured size before it moves to overflow and will rely on that value
                // when determining how many items to move back out the overflow.
                overflow.Push(new Tuple<ICommandBarElement, double>(item, element.DesiredSize.Width));

                this.SecondaryCommands.Insert(0, (ICommandBarElement)element);

                // if a separator was adjacent to the one we removed then move the separator to our holding queue so that it doesn't appear without actually separating the content
                var last = this.PrimaryCommands.LastOrDefault() as AppBarSeparator;
                if (last != null)
                {
                    this.PrimaryCommands.RemoveAt(this.PrimaryCommands.Count - 1);
                    this.separatorQueue.Enqueue(new Tuple<ICommandBarElement, double>(last, last.DesiredSize.Width));
                    separatorQueueWidth += last.DesiredSize.Width;
                    requestedWidth -= last.DesiredSize.Width;
                }
            }

            //else if (requestedWidth < availableSize.Width) // Check if we have room to move items out of the overflow
            //{
                // Next move items out of the overflow if room is available
                while (overflow.Count > 0 && requestedWidth + separatorQueueWidth + overflow.Peek().Item2 <= availableSize.Width)
                {
                    var t = overflow.Pop();

                    this.SecondaryCommands.Remove(t.Item1);

                    if (this.overflow.Count == 1)
                    {
                        overflow.Pop();
                        this.SecondaryCommands.RemoveAt(0);
                    }

                    if (t.Item1 is AppBarSeparator)
                    {
                        this.separatorQueue.Enqueue(t);
                        separatorQueueWidth += t.Item2;
                        continue;
                    }
                    else
                    {
                        while (separatorQueue.Count > 0)
                        {
                            var next = separatorQueue.Dequeue();
                            separatorQueueWidth -= next.Item2;
                            this.PrimaryCommands.Add(next.Item1);
                            requestedWidth += next.Item2;
                        }
                    }

                    // Sometimes this property is being set to disabled
                    ((Control)t.Item1).IsEnabled = true;

                    this.PrimaryCommands.Add(t.Item1);
                    requestedWidth += t.Item2;

                    // check to see if after moving this item we leave a separator at the top of the overflow
                    if (this.overflow.Count > 0)
                    {
                        var test = this.overflow.Peek();
                        if (test.Item1 is AppBarSeparator)
                        {
                            // we won't leave an orphaned separator
                            this.SecondaryCommands.RemoveAt(0);
                            var top = this.overflow.Pop();

                            var element = top.Item1 as UIElement;
                            element.Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));
                            this.separatorQueue.Enqueue(new Tuple<ICommandBarElement, double>(top.Item1, element.DesiredSize.Width));
                            separatorQueueWidth += element.DesiredSize.Width;
                        }
                    }
                }
            //}

            return sizeToReport;
        }

        private void CommandBar_SizeChanged(object sender, Windows.UI.Xaml.SizeChangedEventArgs e)
        {
            // Account for the size of Content
            Size unbounded = new Size(double.PositiveInfinity, double.PositiveInfinity);
            var contentWidth = 0.0;
            if (this.Content != null)
            {
                contentWidth = this.ContentMinWidth;
            }

            // Account for the size of the More button 
            var expandButtonWidth = 0.0;
            if (moreButton != null && moreButton.Visibility == Visibility.Visible)
            {
                moreButton.Measure(unbounded);
                expandButtonWidth = moreButton.DesiredSize.Width;
            }

            double width = expandButtonWidth + contentWidth;
            foreach (var cmd in this.PrimaryCommands)
            {
                var uie = cmd as UIElement;
                uie.Measure(unbounded);
                width += uie.DesiredSize.Width;
            }

            // If we're shrinking then remove items to fit
            // Also when it first appears the Previous size will be 0 and we need 
            // to size to what's currently available
            if (e.NewSize.Width < e.PreviousSize.Width || e.PreviousSize.Width == 0.0
                && e.PreviousSize.Height == 0.0)
            {
                for (int i = this.PrimaryCommands.Count - 1; i > 0 && width > e.NewSize.Width; i--)
                {
                    var item = this.PrimaryCommands[i];
                    var element = item as UIElement;
                    width -= element.DesiredSize.Width;

                    if (this.overflow.Count == 0 && !(element is AppBarSeparator))
                    {
                        // Insert a separator to differentiate between the two sets
                        this.SecondaryCommands.Insert(0, new AppBarSeparator());
                    }

                    overflow.Push(new Tuple<ICommandBarElement, double>(item, element.DesiredSize.Width));

                    this.PrimaryCommands.RemoveAt(i);
                    this.SecondaryCommands.Insert(0, (ICommandBarElement)element);
                }
            }
            else if (e.NewSize.Width > e.PreviousSize.Width) // otherwise if we're growing then move items back out
            {
                while (overflow.Count > 0 && width < e.NewSize.Width)
                {
                    if (width + overflow.Peek().Item2 <= e.NewSize.Width)
                    {
                        var t = overflow.Pop();


                        ((Control)t.Item1).LayoutUpdated += (s, a) =>
                        {
                        };

                        this.SecondaryCommands.Remove(t.Item1);
                        if (this.overflow.Count == 0 && this.SecondaryCommands[0] is AppBarSeparator)
                        {
                            this.SecondaryCommands.RemoveAt(0);
                        }

                        // Sometimes this property is being set to disabled
                        ((Control)t.Item1).IsEnabled = true;

                        this.PrimaryCommands.Insert(this.PrimaryCommands.Count, t.Item1);
                        var control = t.Item1 as Control;
                        width += t.Item2;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
}
