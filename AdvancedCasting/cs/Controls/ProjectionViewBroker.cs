//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

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
