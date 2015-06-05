using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Core;

namespace MediaProjection
{
    // This is a simple container for a set of data used to initialize the projection view
    internal class ProjectionViewPageInitializationData
    {
        public CoreDispatcher MainDispatcher;
        public ViewLifetimeControl ProjectionViewPageControl;
        public int MainViewId;
    }
}
