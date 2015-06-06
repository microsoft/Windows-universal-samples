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
 public sealed class MediaTransportControlsWithoutCastButton : MediaTransportControls
    {

        public MediaTransportControlsWithoutCastButton()
        {
            this.DefaultStyleKey = typeof(MediaTransportControlsWithoutCastButton);          
        }

        protected override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
        }
    }
}
