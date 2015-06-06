using System;
using LiveDash.Util;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Storage;

#if DEBUG
namespace LiveDashApp
{
    public sealed partial class Log : Page
    {
        private DispatcherTimer logTimer;
        public Log()
        {
            this.InitializeComponent();
            logTimer = new DispatcherTimer();
            scrollview.HorizontalScrollMode = ScrollMode.Enabled;
            scrollview.HorizontalScrollBarVisibility = ScrollBarVisibility.Visible;
            SetupLogging();
        }

        private void SetupLogging()
        {
            logTimer.Interval = TimeSpan.FromSeconds(0.25);

            logTimer.Tick += (sender, e) => output.Text = Logger.GetLog();
            logTimer.Start();
        }

    }
}
#endif
