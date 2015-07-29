using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;

namespace Notifications.Controls
{
    public class StepsControl : ItemsControl
    {
        public event EventHandler<int> StepChanged = delegate { };

        public StepsControl()
        {
            this.DefaultStyleKey = typeof(StepsControl);
            base.Loaded += StepsControl_Loaded;
        }

        private void StepsControl_Loaded(object sender, RoutedEventArgs e)
        {
            StepsHelperControl stepsHelperControl = base.ItemsPanelRoot as StepsHelperControl;
            if (stepsHelperControl == null)
                throw new NullReferenceException("Expected stepsHelperControl as ItemsPanelRoot");

            stepsHelperControl.SetBinding(StepsHelperControl.StepProperty, new Binding() { Path = new PropertyPath("Step"), Source = this });
        }

        protected override void OnApplyTemplate()
        {
            Button buttonNext = GetTemplateChild("ButtonNext") as Button;
            if (buttonNext == null)
                throw new NullReferenceException("Expected ButtonNext child");

            buttonNext.Click += ButtonNext_Click;

            FrameworkElement completed = GetTemplateChild("Completed") as FrameworkElement;
            if (completed == null)
                throw new NullReferenceException("Expected Completed child");

            completed.Tapped += Completed_Tapped;
        }

        private void Completed_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            Frame frame = Window.Current.Content as Frame;
            if (frame != null)
            {
                if (frame.CanGoBack)
                {
                    e.Handled = true;
                    frame.GoBack();
                    return;
                }
            }
        }

        private void ButtonNext_Click(object sender, RoutedEventArgs e)
        {
            Step++;
        }

        private StepsHelperControl GetStepsHelperControl()
        {
            return base.ItemsPanelRoot as StepsHelperControl;
        }

        public static readonly DependencyProperty StepProperty = DependencyProperty.Register("Step", typeof(int), typeof(StepsControl), new PropertyMetadata(1, OnStepPropertyChanged));

        private static void OnStepPropertyChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            (sender as StepsControl).OnStepPropertyChanged(e);
        }

        private void OnStepPropertyChanged(DependencyPropertyChangedEventArgs e)
        {
            if (Step > base.Items.Count)
                SetCompleted(true);
            else
                SetCompleted(false);

            StepChanged(this, Step);
        }

        public int Step
        {
            get { return (int)GetValue(StepProperty); }
            set { SetValue(StepProperty, value); }

            //get
            //{
            //    var helper = GetStepsHelperControl();
            //    if (helper != null)
            //        return helper.Step;

            //    return 1;
            //}

            //set
            //{
            //    int oldValue = Step;

            //    var helper = GetStepsHelperControl();
            //    if (helper != null)
            //        helper.Step = value;

            //    if (value > base.Items.Count)
            //        SetCompleted(true);
            //    else
            //        SetCompleted(false);

            //    if (oldValue != value)
            //        StepChanged(this, value);
            //}
        }

        private void SetCompleted(bool isCompleted)
        {
            FrameworkElement normal = GetTemplateChild("Normal") as FrameworkElement;
            if (normal == null)
                return;

            FrameworkElement completed = GetTemplateChild("Completed") as FrameworkElement;
            if (completed == null)
                return;

            if (isCompleted)
            {
                completed.Visibility = Visibility.Visible;
                normal.Visibility = Visibility.Collapsed;
            }

            else
            {
                completed.Visibility = Visibility.Collapsed;
                normal.Visibility = Visibility.Visible;
            }
        }

        public static readonly DependencyProperty IsNextEnabledProperty = DependencyProperty.Register("IsNextEnabled", typeof(bool), typeof(StepsControl), new PropertyMetadata(true));

        public bool IsNextEnabled
        {
            get { return (bool)GetValue(IsNextEnabledProperty); }
            set { SetValue(IsNextEnabledProperty, value); }
        }

        public static readonly DependencyProperty NextButtonVisibilityProperty = DependencyProperty.Register("NextButtonVisibility", typeof(Visibility), typeof(StepsControl), new PropertyMetadata(Visibility.Visible));

        public Visibility NextButtonVisibility
        {
            get { return (Visibility)GetValue(NextButtonVisibilityProperty); }
            set { SetValue(NextButtonVisibilityProperty, value); }
        }
    }
}
