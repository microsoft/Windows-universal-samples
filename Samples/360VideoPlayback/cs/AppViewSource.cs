using Windows.ApplicationModel.Core;
using System;

namespace _360VideoPlayback
{
    // The entry point for the app.
    internal class AppViewSource : IFrameworkViewSource
    {
        public AppViewSource(Uri sourceUri)
        {
            this.sourceUri = sourceUri;
        }

        public IFrameworkView CreateView()
        {
            return new AppView(sourceUri);
        }

        private Uri sourceUri;
    }
}
