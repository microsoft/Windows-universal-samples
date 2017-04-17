using System;
using System.Windows.Input;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Shapes;

namespace RefreshableListView
{
    /// <summary>
    /// Extension of the ListView control that allows "Pull To Refresh" operations on touch devices.
    /// </summary>
    [TemplatePart(Name = PART_ROOT, Type = typeof(UIElement))]
    [TemplatePart(Name = PART_SCROLLER, Type = typeof(ScrollViewer))]
    [TemplatePart(Name = PART_CONTENT_TRANSFORM, Type = typeof(CompositeTransform))]
    [TemplatePart(Name = PART_SCROLLER_CONTENT, Type = typeof(UIElement))]
    [TemplatePart(Name = PART_REFRESH_INDICATOR, Type = typeof(FrameworkElement))]
    [TemplatePart(Name = PART_REFRESH_INDICATOR_TRANSFORM, Type = typeof(CompositeTransform))]
    [TemplatePart(Name = PART_DEFAULT_REFRESH_INDICATOR_CONTENT, Type = typeof(TextBlock))]
    public class RefreshableListView : ListView
    {
        #region Private Variables
        // By default a refresh is requested when 80% of the Refresh Indicator is pulled down.
        const double DEFAULT_REFRESH_INDICATOR_THRESHOLD_RATIO = 0.8;

        // Maximum ScrollViewer overpan ratio: overpan amount / viewport size.
        const double MAX_SCROLLVIEWER_OVERPAN_RATIO = 0.1;

        // Only allow a refresh when the touch manipulation starts with a ScrollViewer.VerticalOffset.
        // smaller than 1.0
        const double INITIAL_VERTICALOFFSET_THRESHOLD = 1.0;

        // Amount of time required to cancel a refresh request when no longer meeting the minimum threshold.
        const int DEACTIVATION_TIMEOUT_MSEC = 1000;

        const string PULL_TO_REFRESH = "Pull to Refresh";
        const string RELEASE_TO_REFRESH = "Release to Refresh";
        const string RELEASE_THEN_PULL_TO_REFRESH = "Release then Pull to Refresh";
        const string REFRESHING = "Refreshing...";

        const string PART_ROOT = "Root";
        const string PART_SCROLLER = "ScrollViewer";
        const string PART_CONTENT_TRANSFORM = "ContentTransform";
        const string PART_SCROLLER_CONTENT = "ScrollerContent";
        const string PART_REFRESH_INDICATOR = "RefreshIndicator";
        const string PART_REFRESH_INDICATOR_TRANSFORM = "RefreshIndicatorTransform";
        const string PART_DEFAULT_REFRESH_INDICATOR_CONTENT = "DefaultRefreshIndicatorContent";

        // Root element in template.
        private UIElement m_root;
        // Container of Refresh Indicator.
        private FrameworkElement m_refreshIndicatorContainer;
        // Main ScrollViewer used by the ListView.
        private ScrollViewer m_scroller;
        // Container for main content.
        private UIElement m_scrollerContent;
        // Container for default content for the Refresh Indicator.
        private TextBlock m_defaultRefreshIndicatorTextBlock;
        // Rectangles used to pad the ScrollViewer's content in AutoRefresh mode
        // during a refresh operation to exit the overpan mode.
        private Rectangle m_overpanSuppressorHead;
        private Rectangle m_overpanSuppressorTail;
        // Composition animation used to adjust the maximum overpan amount
        // of the main content.
        private ExpressionAnimation m_scrollerContentAnimation;
        // Composition animation used to position the Refresh Indicator 
        // directly on top of the main content.
        private ExpressionAnimation m_refreshIndicatorContainerAnimation;
        // Composition animation used to feed the PullRatio variable in
        // the RefreshPropertySet.
        private ExpressionAnimation m_pullRatioAnimation;
        // Composition animation used to feed the PullProgress variable in
        // the RefreshPropertySet.
        private ExpressionAnimation m_pullProgressAnimation;
        // Composition visual used to adjust the maximum overpan amount
        // of the main content.
        private Visual m_scrollerContentVisual;
        // Composition visual used to position the Refresh Indicator 
        // directly on top of the main content.
        private Visual m_refreshIndicatorContainerVisual;
        // Marks the time of the most recent refresh activation.
        DateTime m_lastRefreshActivation = default(DateTime);
        // Indicates whether a refresh operation must be requested
        // at the completion of the touch manipulation.
        private bool m_refreshActivated = false;
        // Indicates whether a refresh is currently activatable.
        private bool m_refreshActivatable = false;
        // Indicates whether an auto refresh is in progress.
        private bool m_isAutoRefreshing = false;
        // Indicates whether an animation is bringing up the content at the end of an auto refresh.
        private bool m_isSuppressingOverpan = false;
        // Indicates whether a touch pointer was received by the ScrollViewer
        // and a DirectManipulationStarted event is expected.
        private bool m_pointerPressed = false;
        // ScrollViewer ZoomFactor value at the beginning of the touch manipulation.
        private float m_zoomFactor = 1.0f;
        #endregion

        #region Events
        /// <summary>
        /// Occurs when the user has requested content to be refreshed.
        /// </summary>
        public event EventHandler<RefreshRequestedEventArgs> RefreshRequested;

        /// <summary>
        /// Occurs when ListView overpan amount is changed or the ScrollViewer's
        /// inertial status changes.
        /// </summary>
        public event EventHandler<RefreshProgressEventArgs> PullProgressChanged;
        #endregion

        /// <summary>
        /// Creates a new instance of <see cref="RefreshableListView"/>
        /// </summary>
        public RefreshableListView()
        {
            this.DefaultStyleKey = typeof(RefreshableListView);
            this.Loaded += RefreshableListView_Loaded;
            this.SizeChanged += RefreshableListView_SizeChanged;
        }

        #region Properties

        /// <summary>
        /// Represents a CompositionPropertySet that contains two properties called PullRatio and PullProgress.
        /// They both vary from 0 to 1 during a pull-to-refresh operation. 
        /// PullRatio reaches 1 when the maximum amount of overpan is reached.
        /// PullProgress reaches 1 when the amount of overpan is enough to hit the PullThreshold value.
        /// Consumer of this RefreshableListView can use them as independent animation sources.
        /// </summary>
        public CompositionPropertySet RefreshPropertySet
        {
            get;
            private set;
        }

        #endregion

        #region Methods

        /// <summary>
        /// Invoked whenever application code or internal processes (such as a rebuilding
        /// layout pass) call <see cref="OnApplyTemplate"/>. In simplest terms, this means the method
        /// is called just before a UI element displays in an application. Override this
        /// method to influence the default post-template logic of a class.
        /// </summary>
        protected override void OnApplyTemplate()
        {
            m_root = this.GetTemplateChild(PART_ROOT) as UIElement;
            m_scroller = this.GetTemplateChild(PART_SCROLLER) as ScrollViewer;
            m_scrollerContent = this.GetTemplateChild(PART_SCROLLER_CONTENT) as UIElement;
            m_refreshIndicatorContainer = this.GetTemplateChild(PART_REFRESH_INDICATOR) as FrameworkElement;
            m_defaultRefreshIndicatorTextBlock = this.GetTemplateChild(PART_DEFAULT_REFRESH_INDICATOR_CONTENT) as TextBlock;

            if (m_root != null &&
                m_scroller != null &&
                m_scrollerContent != null &&
                m_refreshIndicatorContainer != null)
            {
                m_scroller.DirectManipulationStarted += Scroller_DirectManipulationStarted;
                m_scroller.DirectManipulationCompleted += Scroller_DirectManipulationCompleted;
                m_scroller.ViewChanged += Scroller_ViewChanged;
                m_scroller.AddHandler(UIElement.PointerPressedEvent, new PointerEventHandler(Scroller_PointerPressed), true);

                // In AutoRefresh mode, zoom factors smaller or larger than 1.0 are not supported.
                if (this.AutoRefresh)
                {
                    m_scroller.MinZoomFactor = 1.0f;
                    m_scroller.MaxZoomFactor = 1.0f;
                    m_scroller.ZoomMode = ZoomMode.Disabled;
                }

                m_refreshIndicatorContainer.SizeChanged += RefreshIndicatorContainer_SizeChanged;
            }

            if (m_defaultRefreshIndicatorTextBlock != null)
            {
                m_defaultRefreshIndicatorTextBlock.Visibility = this.RefreshIndicatorContent == null ? Visibility.Visible : Visibility.Collapsed;
            }

            base.OnApplyTemplate();
        }

        /// <summary>
        /// Handler for the ListView's Loaded event. 
        /// Creates the composition animations based on sizes and zoom factor value.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void RefreshableListView_Loaded(object sender, RoutedEventArgs e)
        {
            this.UpdateCompositionAnimations();
        }

        /// <summary>
        /// Handler for the ListView's SizeChanged event.
        /// Updates the composition animations based on sizes and zoom factor value.
        /// </summary>
        private void RefreshableListView_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (m_refreshActivatable)
            {
                this.UpdateCompositionAnimations();
            }
        }

        /// <summary>
        /// Handler for the Refresh Indicator's SizeChanged event.
        /// Updates the composition animations based on sizes and zoom factor value.
        /// </summary>
        private void RefreshIndicatorContainer_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (m_refreshActivatable)
            {
                this.UpdateCompositionAnimations();
            }
        }

        /// <summary>
        /// Handler for the ScrollViewer's PointerPressed event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scroller_PointerPressed(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            // Expect a DirectManipulationStarted event only when a touch pointer is seen.
            m_pointerPressed |= e.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Touch;
        }

        /// <summary>
        /// Handler for the ScrollViewer's ViewChanging event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scroller_ViewChanging(object sender, ScrollViewerViewChangingEventArgs e)
        {
            // Cancel refreshable status in inertia mode.
            if (e.IsInertial && !m_isAutoRefreshing)
            {
                this.UpdateRefreshableStatus(false /*refreshActivatable*/, false /*force*/);
            }
        }

        /// <summary>
        /// Handler for the ScrollViewer's ViewChanged event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scroller_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            // Cancel refreshable status and stop the composition animations when the zoom factor changes.
            if (m_scroller.ZoomFactor != m_zoomFactor)
            {
                this.StopCompositionAnimations();
                this.UpdateRefreshableStatus(false /*refreshActivatable*/, false /*force*/);
            }

            if (!e.IsIntermediate && m_isSuppressingOverpan)
            {
                // The content finished animating up at the end of an auto refresh.
                // Hide the two padding rectangles, re-enable touch interactions with
                // the ScrollViewer and make sure its new VerticalOffset is 0.
                m_isSuppressingOverpan = false;
                m_scrollerContent.ManipulationMode = ManipulationModes.System;
                this.RemoveOverpanSuppressors();
                m_scroller.ChangeView(null, 0, null, true);
                this.UpdateCompositionAnimations();
                this.UpdateRefreshableStatus(true /*refreshActivatable*/, true /*force*/);
            }
        }

        /// <summary>
        /// Handler for the ScrollViewer's DirectManipulationStarted event.
        /// Updates the refreshable status based on initial VerticalOffset.
        /// </summary>
        private void Scroller_DirectManipulationStarted(object sender, object e)
        {
            System.Diagnostics.Debug.Assert(
                m_root != null &&
                m_scroller != null &&
                m_scrollerContent != null &&
                m_refreshIndicatorContainer != null);

            if (!m_pointerPressed)
            {
                return;
            }
            m_pointerPressed = false;

            this.UpdateCompositionAnimations();

            m_zoomFactor = m_scroller.ZoomFactor;

            // Manipulation needs to start with no initial offset to allow a refresh.
            if (m_scroller.VerticalOffset < INITIAL_VERTICALOFFSET_THRESHOLD)
            {
                Windows.UI.Xaml.Media.CompositionTarget.Rendering += CompositionTarget_Rendering;
                m_scroller.ViewChanging += Scroller_ViewChanging;
            }

            this.UpdateRefreshableStatus(m_scroller.VerticalOffset < INITIAL_VERTICALOFFSET_THRESHOLD /*refreshActivatable*/, true /*force*/);
        }

        /// <summary>
        /// Handler for the ScrollViewer's DirectManipulationCompleted event.
        /// Raises the RefreshRequested event and executes the RefreshCommand
        /// command when a refresh was activated during the manipulation.
        /// </summary>
        private void Scroller_DirectManipulationCompleted(object sender, object e)
        {
            System.Diagnostics.Debug.Assert(
                m_root != null &&
                m_scroller != null &&
                m_scrollerContent != null &&
                m_refreshIndicatorContainer != null);

            Windows.UI.Xaml.Media.CompositionTarget.Rendering -= CompositionTarget_Rendering;
            m_scroller.ViewChanging -= Scroller_ViewChanging;

            if (m_refreshActivated)
            {
                if (this.RefreshRequested != null)
                {
                    RefreshRequestedEventArgs refreshRequestedEventArgs = new RefreshRequestedEventArgs(
                        this.AutoRefresh ? new DeferralCompletedHandler(RefreshCompleted) : null);
                    this.RefreshRequested(this, refreshRequestedEventArgs);
                    if (this.AutoRefresh)
                    {
                        m_scrollerContent.ManipulationMode = ManipulationModes.None;
                        if (!refreshRequestedEventArgs.WasDeferralRetrieved)
                        {
                            // The Deferral object was not retrieved in the event handler.
                            // Animate the content up right away.
                            this.RefreshCompleted();
                        }
                    }
                }

                if (this.RefreshCommand != null && this.RefreshCommand.CanExecute(null))
                {
                    this.RefreshCommand.Execute(null);
                }
            }

            m_isAutoRefreshing = false;
            m_refreshActivated = false;
            m_lastRefreshActivation = default(DateTime);

            this.UpdateCompositionAnimations();
            if (m_scrollerContent.ManipulationMode == ManipulationModes.System)
            {
                this.UpdateRefreshableStatus(m_scroller.VerticalOffset < INITIAL_VERTICALOFFSET_THRESHOLD /*refreshActivatable*/, m_scroller.VerticalOffset < INITIAL_VERTICALOFFSET_THRESHOLD /*force*/);
            }
        }

        /// <summary>
        /// Event handler called before the UI thread renders a new frame.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CompositionTarget_Rendering(object sender, object e)
        {
            System.Diagnostics.Debug.Assert(
                m_root != null &&
                m_scroller != null &&
                m_scrollerContent != null &&
                m_refreshIndicatorContainer != null);

            if (!m_refreshActivatable)
            {
                // No refresh is activated when the scroller is in inertia or zooming mode.
                return;
            }

            Rect elementBounds = m_scrollerContent.TransformToVisual(m_root).TransformBounds(new Rect());
            double pullProgress = 0.0;
            double pullAmount = elementBounds.Y;
            double pullThreshold = double.IsNaN(this.PullThreshold) ? (m_refreshIndicatorContainer.ActualHeight * DEFAULT_REFRESH_INDICATOR_THRESHOLD_RATIO) : this.PullThreshold;

            pullThreshold *= m_scroller.ZoomFactor;

            if (pullAmount >= pullThreshold)
            {
                // Refresh threshold is reached.
                m_lastRefreshActivation = DateTime.Now;
                m_refreshActivated = true;
                pullProgress = 1.0;
                this.UpdateCompositionAnimations();
                if (!this.AutoRefresh && this.RefreshIndicatorContent == null && m_defaultRefreshIndicatorTextBlock != null)
                {
                    m_defaultRefreshIndicatorTextBlock.Text = RELEASE_TO_REFRESH;
                }
            }
            else
            {
                // Refresh threshold is not reached.
                if (m_lastRefreshActivation != DateTime.MinValue)
                {
                    // Refresh threshold was reached before.
                    TimeSpan timeSinceActivated = DateTime.Now - m_lastRefreshActivation;
                    if (timeSinceActivated.TotalMilliseconds > DEACTIVATION_TIMEOUT_MSEC)
                    {
                        // Refresh threshold has not been re-reached for more than a second, deactivate.
                        m_refreshActivated = false;
                        m_lastRefreshActivation = default(DateTime);
                        pullProgress = pullAmount / pullThreshold;
                        this.UpdateCompositionAnimations();
                        if (this.RefreshIndicatorContent == null && m_defaultRefreshIndicatorTextBlock != null)
                        {
                            m_defaultRefreshIndicatorTextBlock.Text = PULL_TO_REFRESH;
                        }
                    }
                    else
                    {
                        // Pretend the threshold is still met.
                        pullProgress = 1.0;
                    }
                }
                else
                {
                    pullProgress = pullAmount / pullThreshold;
                }
            }

            if (this.PullProgressChanged != null)
            {
                this.PullProgressChanged(this, new RefreshProgressEventArgs() { PullProgress = pullProgress, IsRefreshable = true });
            }

            if (m_refreshActivated && this.AutoRefresh)
            {
                this.AddOverpanSuppressors();
                m_isAutoRefreshing = true;
                UpdateCompositionAnimations();
                m_scroller.UpdateLayout();
                m_scrollerContent.CancelDirectManipulations();

                if (this.RefreshIndicatorContent == null && m_defaultRefreshIndicatorTextBlock != null)
                {
                    m_defaultRefreshIndicatorTextBlock.Text = REFRESHING;
                }
            }
        }

        /// <summary>
        /// Updates the default refresh indicator text based on the new refreshable status.
        /// Raises the PullProgressChanged event with new PullProgress and IsRefreshable values.
        /// </summary>
        /// <param name="refreshActivatable"></param>
        /// <param name="force"></param>
        private void UpdateRefreshableStatus(bool refreshActivatable, bool force)
        {
            if (refreshActivatable != m_refreshActivatable || force)
            {
                m_refreshActivatable = refreshActivatable;

                if (this.RefreshIndicatorContent == null && m_defaultRefreshIndicatorTextBlock != null)
                {
                    m_defaultRefreshIndicatorTextBlock.Text = refreshActivatable ? PULL_TO_REFRESH : RELEASE_THEN_PULL_TO_REFRESH;
                }

                if (this.PullProgressChanged != null)
                {
                    this.PullProgressChanged(this, new RefreshProgressEventArgs() { PullProgress = 0, IsRefreshable = refreshActivatable });
                }
            }
        }

        /// <summary>
        /// Shows two rectangles on top and bottom of the ScrollViewer content during an auto refresh.
        /// They allow to exit the overpan mode at the beginning of the operation and then animate the
        /// content up at the end.
        /// </summary>
        private void AddOverpanSuppressors()
        {
            if (m_overpanSuppressorHead == null)
            {
                m_overpanSuppressorHead = new Rectangle();
                m_overpanSuppressorTail = new Rectangle();
                Grid.SetRow(m_overpanSuppressorTail, 2);
                Panel panel = m_scrollerContent as Panel;
                if (panel != null)
                {
                    panel.Children.Insert(0, m_overpanSuppressorHead);
                    panel.Children.Add(m_overpanSuppressorTail);
                }
            }

            m_overpanSuppressorHead.Height = double.IsNaN(this.PullThreshold) ? (m_refreshIndicatorContainer.ActualHeight * DEFAULT_REFRESH_INDICATOR_THRESHOLD_RATIO) : this.PullThreshold;
            m_overpanSuppressorHead.Visibility = Visibility.Visible;

            m_overpanSuppressorTail.Height = m_scroller.ActualHeight / m_scroller.ZoomFactor;
            m_overpanSuppressorTail.Visibility = Visibility.Visible;
        }

        /// <summary>
        /// Hides the two padding rectangles used in AutoRefresh mode.
        /// </summary>
        private void RemoveOverpanSuppressors()
        {
            if (m_overpanSuppressorHead != null)
            {
                m_overpanSuppressorHead.Visibility = Visibility.Collapsed;
                m_overpanSuppressorTail.Visibility = Visibility.Collapsed;
            }
        }

        /// <summary>
        /// Invoked by application code when a deferred processing of the RefreshRequested event completes.
        /// </summary>
        private void RefreshCompleted()
        {
            // Animate the ScrollViewer content up since the refresh operation completed.
            // This will hide the refresh indicator UI.
            m_isSuppressingOverpan = true;
            m_scroller.ChangeView(null, m_overpanSuppressorHead.Height, null);
        }

        /// <summary>
        /// Sets up the composition animations that position the Refresh Indicator and main content
        /// based on the overpan amount.
        /// </summary>
        private void UpdateCompositionAnimations()
        {
            if (m_root != null &&
                m_scroller != null &&
                m_scrollerContent != null &&
                m_refreshIndicatorContainer != null)
            {
                if (m_scrollerContentAnimation == null)
                {
                    CompositionPropertySet scrollingProperties = ElementCompositionPreview.GetScrollViewerManipulationPropertySet(m_scroller);
                    Compositor compositor = scrollingProperties.Compositor;                    

                    // Ensure that the maximum overpan amount corresponds to the Refresh Indicator's height.
                    m_scrollerContentVisual = ElementCompositionPreview.GetElementVisual(m_scrollerContent);
                    m_scrollerContentAnimation = compositor.CreateExpressionAnimation(@"overpanMultiplier * max(0, scrollingProperties.Translation.Y)");
                    m_scrollerContentAnimation.SetReferenceParameter("scrollingProperties", scrollingProperties);

                    // Ensure that the Refresh Indicator is positioned on top of the main content.
                    m_refreshIndicatorContainerVisual = ElementCompositionPreview.GetElementVisual(m_refreshIndicatorContainer);
                    m_refreshIndicatorContainerAnimation = compositor.CreateExpressionAnimation(@"-refreshIndicatorContainerHeight");

                    // Create RefreshPropertySet and populate it with the PullRatio and PullProgress variables that vary from 0 to 1.
                    this.RefreshPropertySet = compositor.CreatePropertySet();

                    m_pullRatioAnimation = compositor.CreateExpressionAnimation(@"clamp(max(0, scrollingProperties.Translation.Y) / maxOverpan + staticRatio, 0, 1)");
                    m_pullRatioAnimation.SetReferenceParameter("scrollingProperties", scrollingProperties);
                    this.RefreshPropertySet.InsertScalar("PullRatio", 0);

                    m_pullProgressAnimation = compositor.CreateExpressionAnimation(@"clamp(max(0, scrollingProperties.Translation.Y) / thresholdOverpan + staticProgress, 0, 1)");
                    m_pullProgressAnimation.SetReferenceParameter("scrollingProperties", scrollingProperties);
                    this.RefreshPropertySet.InsertScalar("PullProgress", 0);
                }

                m_scrollerContentAnimation.SetScalarParameter("overpanMultiplier", 
                    (float)(m_refreshIndicatorContainer.ActualHeight / m_scroller.ActualHeight / MAX_SCROLLVIEWER_OVERPAN_RATIO - 1.0f / m_scroller.ZoomFactor));
                m_scrollerContentVisual.StartAnimation("Offset.Y", m_scrollerContentAnimation);

                m_refreshIndicatorContainerAnimation.SetScalarParameter("refreshIndicatorContainerHeight",
                    (float)m_refreshIndicatorContainer.ActualHeight);
                m_refreshIndicatorContainerVisual.StartAnimation("Offset.Y", m_refreshIndicatorContainerAnimation);

                float maxOverpan = (float)(m_scroller.ActualHeight * MAX_SCROLLVIEWER_OVERPAN_RATIO);
                m_pullRatioAnimation.SetScalarParameter("maxOverpan", maxOverpan);
                float staticRatio = (m_isAutoRefreshing || m_isSuppressingOverpan || m_scrollerContent.ManipulationMode == ManipulationModes.None) ? (float)DEFAULT_REFRESH_INDICATOR_THRESHOLD_RATIO : 0.0f;
                m_pullRatioAnimation.SetScalarParameter("staticRatio", staticRatio);
                this.RefreshPropertySet.StartAnimation("PullRatio", m_pullRatioAnimation);

                float thresholdOverpan = (float)(double.IsNaN(this.PullThreshold) ? 
                    (m_scroller.ActualHeight * MAX_SCROLLVIEWER_OVERPAN_RATIO * DEFAULT_REFRESH_INDICATOR_THRESHOLD_RATIO) : this.PullThreshold);
                m_pullProgressAnimation.SetScalarParameter("thresholdOverpan", thresholdOverpan);
                float staticProgress = (m_refreshActivated || m_isAutoRefreshing || m_isSuppressingOverpan || m_scrollerContent.ManipulationMode == ManipulationModes.None) ? 1.0f : 0.0f;
                m_pullProgressAnimation.SetScalarParameter("staticProgress", staticProgress);
                this.RefreshPropertySet.StartAnimation("PullProgress", m_pullProgressAnimation);
            }
        }

        /// <summary>
        /// Stops the composition animations that position the Refresh Indicator and main content
        /// based on the overpan amount.
        /// </summary>
        private void StopCompositionAnimations()
        {
            if (m_scrollerContentVisual != null)
            {
                m_scrollerContentVisual.StopAnimation("Offset.Y");
            }

            if (m_refreshIndicatorContainerVisual != null)
            { 
                m_refreshIndicatorContainerVisual.StopAnimation("Offset.Y");
            }
        }
        #endregion

        #region Dependency Properties

        /// <summary>
        /// Gets or sets the PullThreshold in pixels for when a refresh must be requested.
        /// Default value is double.NaN which means that a refresh is requested when 80% of
        /// the RefreshIndicator is displayed.
        /// </summary>
        public double PullThreshold
        {
            get
            {
                return (double)this.GetValue(PullThresholdProperty);
            }
            set
            {
                this.SetValue(PullThresholdProperty, value);
            }
        }

        /// <summary>
        /// Identifies the <see cref="PullThreshold"/> property.
        /// </summary>
        public static readonly DependencyProperty PullThresholdProperty =
            DependencyProperty.Register("PullThreshold", typeof(double), typeof(RefreshableListView), new PropertyMetadata(double.NaN));

        /// <summary>
        /// Gets or sets the Command that will be invoked when Refresh is requested.
        /// </summary>
        public ICommand RefreshCommand
        {
            get
            {
                return (ICommand)this.GetValue(RefreshCommandProperty);
            }
            set
            {
                this.SetValue(RefreshCommandProperty, value);
            }
        }

        /// <summary>
        /// Identifies the <see cref="RefreshCommand"/> property.
        /// </summary>
        public static readonly DependencyProperty RefreshCommandProperty =
            DependencyProperty.Register("RefreshCommand", typeof(ICommand), typeof(RefreshableListView), new PropertyMetadata(null));

        /// <summary>
        /// Gets or sets the Content of the Refresh Indicator.
        /// </summary>
        public object RefreshIndicatorContent
        {
            get
            {
                return (object)this.GetValue(RefreshIndicatorContentProperty);
            }
            set
            {
                if (m_defaultRefreshIndicatorTextBlock != null)
                {
                    m_defaultRefreshIndicatorTextBlock.Visibility = value == null ? Visibility.Visible : Visibility.Collapsed;
                }
                this.SetValue(RefreshIndicatorContentProperty, value);
            }
        }

        /// <summary>
        /// Identifies the <see cref="RefreshIndicatorContent"/> property.
        /// </summary>
        public static readonly DependencyProperty RefreshIndicatorContentProperty =
            DependencyProperty.Register("RefreshIndicatorContent", typeof(object), typeof(RefreshableListView), new PropertyMetadata(null));

        /// <summary>
        /// Indicates whether a refresh request is raised as soon as the overpan amount reaches the PullThreshold value.
        /// When set to True, RefreshRequestedEventArgs.GetDeferral can be invoked to access a Windows.Foundation.Deferral
        /// object to mark the completion of the refresh. At that point the refresh indicator UI is hidden.
        /// </summary>
        public bool AutoRefresh
        {
            get
            {
                return (bool) this.GetValue(AutoRefreshProperty);
            }
            set
            {
                // In AutoRefresh mode, zoom factors smaller or larger than 1.0 are not supported.
                if (value && m_scroller != null)
                {
                    m_scroller.MinZoomFactor = 1.0f;
                    m_scroller.MaxZoomFactor = 1.0f;
                    m_scroller.ZoomMode = ZoomMode.Disabled;
                }
                this.SetValue(AutoRefreshProperty, value);
            }
        }

        /// <summary>
        /// Identifies the <see cref="AutoRefresh"/> property.
        /// </summary>
        public static readonly DependencyProperty AutoRefreshProperty =
            DependencyProperty.Register("AutoRefresh", typeof(bool), typeof(RefreshableListView), new PropertyMetadata(true));
        #endregion
    }

    public class RefreshRequestedEventArgs : EventArgs
    {
        internal RefreshRequestedEventArgs(DeferralCompletedHandler deferralCompletedHandler)
        {
            this.Handler = deferralCompletedHandler;
        }

        private DeferralCompletedHandler Handler
        {
            get;
            set;
        }

        private Deferral Deferral
        {
            get;
            set;
        }

        internal bool WasDeferralRetrieved
        {
            get;
            set;
        }

        public Deferral GetDeferral()
        {
            if (this.Handler == null)
            {
                throw new InvalidOperationException("GetDeferral is only supported when RefreshableListView.AutoRefresh is set to True.");
            }
            if (this.Deferral == null)
            {
                this.Deferral = new Deferral(this.Handler);
            }
            this.WasDeferralRetrieved = true;
            return this.Deferral;
        }
    }

    public class RefreshProgressEventArgs : EventArgs
    {
        /// <summary>
        /// Value from 0.0 to 1.0 where 1.0 is active.
        /// </summary>
        public double PullProgress
        {
            get;
            set;
        }

        /// <summary>
        /// Indicates whether a refresh operation can be triggered.
        /// Returns False when:
        /// - the ScrollViewer is in inertia mode, or started the overpan in inertia mode.
        /// - the ScrollViewer's zoom factor changed during the manipulation.
        /// - the manipulation started with a vertical offset larger than the threshold INITIAL_VERTICALOFFSET_THRESHOLD.
        /// </summary>
        public bool IsRefreshable
        {
            get;
            set;
        }
    }
}
