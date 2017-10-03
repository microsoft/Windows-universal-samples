using System;

namespace _360VideoPlayback.Common
{
    // A base class that tracks resources allocated by native code. This class is
    // used to release COM references to DirectX resources.
    public abstract class Disposer : IDisposable
    {
        #region Public methods

        // Releases resources allocated by native code (unmanaged resources).
        // All disposable objects that were added to the collection will be disposed of
        // when this method is called.
        public void Dispose()
        {
            if (!IsDisposed)
            {
                Dispose(true);
                IsDisposed = true;
            }
        }

        #endregion

        #region Protected methods and properties

        protected internal Disposer()
        {
        }

        // Indicates whether this instance is already disposed.
        protected internal bool IsDisposed { get; private set; }

        // Disposes all IDisposable object resources in the collection of disposable 
        // objects.
        // 
        // NOTE: Since this class exists to dispose of unmanaged resources, the 
        //       disposeManagedResources parameter is ignored.
        protected virtual void Dispose(bool disposeManagedResources)
        {
            // If the DisposeCollector exists, have it dispose of all COM objects.
            if (!IsDisposed && DisposeCollector != null)
            {
                DisposeCollector.Dispose();
            }

            // The DisposeCollector is done, and can be discarded.
            DisposeCollector = null;
        }

        // Adds an IDisposable object to the collection of disposable objects.
        protected internal T ToDispose<T>(T objectToDispose)
        {
            // If objectToDispose is not null, add it to the collection.
            if (!ReferenceEquals(objectToDispose, null))
            {
                // Create DisposeCollector if it doesn't already exist.
                if (DisposeCollector == null)
                {
                    DisposeCollector = new SharpDX.DisposeCollector();
                    IsDisposed = false;
                }

                return DisposeCollector.Collect(objectToDispose);
            }

            // Otherwise, return a default instance of type T.
            return default(T);
        }

        // Disposes of an IDisposable object immediately and also removes the object from the
        // collection of disposable objects.
        protected internal void RemoveAndDispose<T>(ref T objectToDispose)
        {
            // If objectToDispose is not null, and if the DisposeCollector is available, have 
            // the DisposeCollector get rid of objectToDispose.
            if (!ReferenceEquals(objectToDispose, null) && (DisposeCollector != null))
            {
                DisposeCollector.RemoveAndDispose(ref objectToDispose);
            }
        }

        // Removes an IDisposable object from the collection of disposable objects. Does not 
        // dispose of the object before removing it.
        protected internal void RemoveToDispose<T>(T objectToRemove)
        {
            // If objectToRemove is not null, have the DisposeCollector forget about it.
            if (!ReferenceEquals(objectToRemove, null) && (DisposeCollector != null))
            {
                DisposeCollector.Remove(objectToRemove);
            }
        }

        #endregion

        #region Private properties

        // The collection of disposable objects.
        private SharpDX.DisposeCollector DisposeCollector { get; set; }

        #endregion
    }
}
