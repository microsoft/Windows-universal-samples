using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace ScreenCasting.Controls
{
 public sealed class ProjectedMediaTransportControls : MediaTransportControls
    {

        public ProjectedMediaTransportControls()
        {
            this.DefaultStyleKey = typeof(ProjectedMediaTransportControls);
        }

        protected override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            //Find the Swap Views button and subscribe to the clicked event.
            Button swapViewsButton = GetTemplateChild("SwapViewsButton") as Button;
            swapViewsButton.Click += SwapViewsButton_Click;

            //Find the Stop Projecting button and subscribe to the clicked event.
            Button stopProjectingButton = GetTemplateChild("StopProjectingButton") as Button;
            stopProjectingButton.Click += StopProjectingButton_Click;

            this.IsCompact = true;
        }

        public event EventHandler StopProjectingButtonClick;
        public event EventHandler SwapViewsButtonClick;

        private void StopProjectingButton_Click (object sender, RoutedEventArgs e)
        {
            if (StopProjectingButtonClick != null)
                StopProjectingButtonClick(this, EventArgs.Empty);
        }

        private void SwapViewsButton_Click(object sender, RoutedEventArgs e)
        {
            if (SwapViewsButtonClick != null)
                SwapViewsButtonClick(this, EventArgs.Empty);
        }
    }
}
