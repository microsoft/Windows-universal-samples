using ScreenCasting.Controls;
using ScreenCasting.Util;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;

namespace ScreenCasting
{
    // This is a simple container for a set of data used to comminicate between the main and the projection view
    internal class ProjectionViewBroker
    {
        public CoreDispatcher MainPageDispatcher;
        public ViewLifetimeControl ProjectionViewPageControl;
        public int MainViewId;
        public event EventHandler ProjectionStopping;
        public ProjectionViewPage ProjectedPage;
        public void NotifyProjectionStopping()
        {
            try
            {
                if (ProjectionStopping != null)
                    ProjectionStopping(this, EventArgs.Empty);
            }
            catch { }
        }

    }
}
